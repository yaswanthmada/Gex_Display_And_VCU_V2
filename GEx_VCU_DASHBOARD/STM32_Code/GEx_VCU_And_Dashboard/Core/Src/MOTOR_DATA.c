/*
 * Odometer.c
 *
 *  Created on: Sep 09, 2026
 *      Author: MadaYaswanth
 */

#include "MOTOR_DATA.h"
#include "FLASH.h"
#include "main.h"
#include <string.h>
#include <math.h>

#define ODO_PAGE_COUNT       2U
#define ODO_PAGE_A_IDX       0U
#define ODO_PAGE_B_IDX       1U

static const uint32_t Odo_Page_Addr[ODO_PAGE_COUNT] =
{
    [ODO_PAGE_A_IDX] = 0x0801F400U,
    [ODO_PAGE_B_IDX] = 0x0801F800U,
};

#define ODO_PAGE_SIZE        1024U
#define ODO_MAGIC            0x4F444F31U
#define ODO_ERASED_WORD      0xFFFFFFFFU
#define WATCHDOG_PAGE_ADDR   0x0801FC00U

typedef struct __attribute__((packed))
{
    uint32_t Magic;
    uint32_t Sequence;
    uint32_t Total_M;
    uint32_t Trip_M;
    uint32_t Crc;
} Odo_Record_t;

#define ODO_RECORD_SIZE      (sizeof(Odo_Record_t))
#define ODO_SLOTS_PER_PAGE   (ODO_PAGE_SIZE / ODO_RECORD_SIZE)   /* 51 slots */
#define ODO_UPDATE_INTERVAL_MS    100U
#define ODO_SAVE_INTERVAL_MS      300000U   /* 5 min - balances data-loss window vs flash wear */
#define MAX_REALISTIC_SPEED_KMH   120.0f
#define MIN_SPEED_THRESHOLD_KMH   0.5f

static double   Total_Distance_Km    = 0.0;
static double   Trip_Distance_Km     = 0.0;

static uint8_t  Active_Page_Idx      = ODO_PAGE_A_IDX;
static uint32_t Next_Slot_Index      = 0;
static uint32_t Current_Sequence     = 0;

static uint32_t Last_Odo_Update_Tick = 0;
static uint32_t Last_Odo_Save_Tick   = 0;



static bool     Trip_Hold_First_Call  = true;
static bool     Trip_Hold_Active      = false;   /* switch is being held          */
static bool     Trip_Hold_Done        = false;   /* reset already done this press */
static uint32_t Trip_Hold_Start_Tick  = 0U;
/*******************************************************************************
 * Function Name : Calculate_Crc32_Bytes
 * Description   : Computes a standard CRC-32 checksum over a byte buffer
 *                 using bit-by-bit calculation, with no lookup table -
 *                 trades a small amount of extra CPU time for zero additional
 *                 RAM/flash usage.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the byte buffer to checksum
 *                 Len  - Number of bytes in the buffer
 * Return Value  : uint32_t (calculated CRC-32 value)
 ******************************************************************************/
static uint32_t Calculate_Crc32_Bytes(const uint8_t* Data, uint32_t Len)
{
    uint32_t crc = 0xFFFFFFFFU;

    for (uint32_t i = 0; i < Len; i++)
    {
        crc ^= Data[i];

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 1U)
            {
                crc = (crc >> 1) ^ 0xEDB88320U;
            }
            else
            {
                crc = crc >> 1;
            }
        }
    }

    return crc ^ 0xFFFFFFFFU;
}

/*******************************************************************************
 * Function Name : Calculate_Record_Crc
 * Description   : Computes the CRC-32 over the Magic, Sequence, Total_M, and
 *                 Trip_M fields of a record (excluding the Crc field itself),
 *                 used to detect corrupted or partially written flash records.
 * Scope         : Static (Private to this file)
 * Parameters    : Rec - Pointer to the record to checksum
 * Return Value  : uint32_t (calculated CRC-32 value)
 ******************************************************************************/
static uint32_t Calculate_Record_Crc(const Odo_Record_t* Rec)
{
    uint32_t fields[4] = { Rec->Magic, Rec->Sequence, Rec->Total_M, Rec->Trip_M };
    return Calculate_Crc32_Bytes((const uint8_t*)fields, sizeof(fields));
}

/*******************************************************************************
 * Function Name : Record_Is_Valid
 * Description   : Checks whether a flash record is trustworthy by verifying
 *                 its magic number and recalculating its CRC-32, catching
 *                 blank, corrupted, or partially written records.
 * Scope         : Static (Private to this file)
 * Parameters    : Rec - Pointer to the record to validate
 * Return Value  : bool (true = record is valid, false = corrupted/blank/mismatched)
 ******************************************************************************/
static bool Record_Is_Valid(const Odo_Record_t* Rec)
{
    if (Rec->Magic != ODO_MAGIC)
    {
        return false;
    }
    return (Rec->Crc == Calculate_Record_Crc(Rec));
}

/*******************************************************************************
 * Function Name : Scan_Page
 * Description   : Walks every slot in the given flash page, skipping any
 *                 record that fails its CRC check, and reports the highest-
 *                 sequence valid record found along with the index of the
 *                 first blank (erased) slot available for the next write.
 * Scope         : Static (Private to this file)
 * Parameters    : Page_Idx           - ODO_PAGE_A_IDX or ODO_PAGE_B_IDX
 *                 Best_Out           - Pointer to receive the best valid record found
 *                 Found_Valid_Out    - Pointer to receive whether any valid record was found
 *                 Next_Free_Slot_Out - Pointer to receive the index of the next blank slot
 * Return Value  : None
 ******************************************************************************/
static void Scan_Page(uint8_t Page_Idx, Odo_Record_t* Best_Out,
                       bool* Found_Valid_Out, uint32_t* Next_Free_Slot_Out)
{
    uint32_t page_base = Odo_Page_Addr[Page_Idx];
    bool     found_any = false;
    uint32_t best_seq  = 0;
    Odo_Record_t best_rec;
    memset(&best_rec, 0, sizeof(best_rec));
    uint32_t free_slot = ODO_SLOTS_PER_PAGE;

    for (uint32_t i = 0; i < ODO_SLOTS_PER_PAGE; i++)
    {
        const Odo_Record_t* rec = (const Odo_Record_t*)(page_base + (i * ODO_RECORD_SIZE));

        if (rec->Magic == ODO_ERASED_WORD)
        {
            free_slot = i;
            break;
        }

        if (Record_Is_Valid(rec))
        {
            if (!found_any || rec->Sequence > best_seq)
            {
                found_any = true;
                best_seq  = rec->Sequence;
                memcpy(&best_rec, rec, sizeof(best_rec));
            }
        }
        /* invalid, non-erased slot (corrupt/partial write) - skip, keep scanning */
    }

    *Found_Valid_Out    = found_any;
    *Next_Free_Slot_Out = free_slot;
    if (found_any)
    {
        memcpy(Best_Out, &best_rec, sizeof(best_rec));
    }
}

/*******************************************************************************
 * Function Name : Erase_Page
 * Description   : Erases one flash page using the register-level flash driver,
 *                 then verifies the erase actually succeeded by reading back
 *                 the first and last word of the page and confirming both
 *                 read as blank.
 * Scope         : Static (Private to this file)
 * Parameters    : Page_Idx - ODO_PAGE_A_IDX or ODO_PAGE_B_IDX
 * Return Value  : bool (true = erase verified successful, false = erase failed)
 ******************************************************************************/
static bool Erase_Page(uint8_t Page_Idx)
{
    Flash_Erase_Page(Odo_Page_Addr[Page_Idx]);
    Flash_Wait_Busy();

    const uint32_t* first_word = (const uint32_t*)Odo_Page_Addr[Page_Idx];
    const uint32_t* last_word  = (const uint32_t*)(Odo_Page_Addr[Page_Idx] + ODO_PAGE_SIZE - 4);

    return (*first_word == ODO_ERASED_WORD) && (*last_word == ODO_ERASED_WORD);
}

/*******************************************************************************
 * Function Name : Write_Record
 * Description   : Writes one odometer record into the given slot using the
 *                 register-level half-word flash driver, verifying each
 *                 half-word by reading it back immediately after writing.
 * Scope         : Static (Private to this file)
 * Parameters    : Page_Idx   - ODO_PAGE_A_IDX or ODO_PAGE_B_IDX
 *                 Slot_Index - Index of the slot within the page to write to
 *                 Rec        - Pointer to the record to write
 * Return Value  : bool (true = all half-words verified written correctly,
 *                       false = a mismatch was detected partway through)
 ******************************************************************************/
static bool Write_Record(uint8_t Page_Idx, uint32_t Slot_Index, const Odo_Record_t* Rec)
{
    uint32_t addr = Odo_Page_Addr[Page_Idx] + (Slot_Index * ODO_RECORD_SIZE);
    const uint16_t* half_words = (const uint16_t*)Rec;
    uint32_t count = ODO_RECORD_SIZE / 2;

    for (uint32_t i = 0; i < count; i++)
    {
        uint32_t word_addr = addr + (i * 2);
        Flash_Write_Half_Word(word_addr, half_words[i]);
        Flash_Wait_Busy();

        uint16_t written_back = *(__IO uint16_t*)word_addr;
        if (written_back != half_words[i])
        {
            return false;
        }
    }
    return true;
}
/*******************************************************************************
 * Function Name : Other_Page
 * Description   : Returns the index of the page that is NOT currently active,
 *                 used to determine which page to erase and switch to during
 *                 an A/B page rollover.
 * Scope         : Static (Private to this file)
 * Parameters    : Page_Idx - The current page index
 * Return Value  : uint8_t (the opposite page index)
 ******************************************************************************/
static inline uint8_t Other_Page(uint8_t Page_Idx)
{
    return (Page_Idx == ODO_PAGE_A_IDX) ? ODO_PAGE_B_IDX : ODO_PAGE_A_IDX;
}
/*******************************************************************************
 * Function Name : Save_Odometer_To_Storage
 * Description   : Writes the current Total/Trip distance into the next free
 *                 slot of the active flash page. If the active page is full,
 *                 erases the other page first and switches to it. A failed
 *                 write always advances past the slot rather than retrying
 *                 the same address, since flash bits cannot be un-programmed
 *                 without a full page erase.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Save_Odometer_To_Storage(void)
{
    if (Next_Slot_Index >= ODO_SLOTS_PER_PAGE)
    {
        uint8_t target_page = Other_Page(Active_Page_Idx);

        //__disable_irq();
        Flash_Unlock();
        bool erased = Erase_Page(target_page);
        Flash_Lock();
        //__enable_irq();

        if (!erased)
        {
            return;
        }

        Active_Page_Idx = target_page;
        Next_Slot_Index = 0;
    }
    Odo_Record_t rec;
    rec.Magic    = ODO_MAGIC;
    rec.Sequence = Current_Sequence + 1;
    rec.Total_M  = (uint32_t)(Total_Distance_Km * 1000.0);
    rec.Trip_M   = (uint32_t)(Trip_Distance_Km  * 1000.0);
    rec.Crc      = Calculate_Record_Crc(&rec);

    //__disable_irq();
    Flash_Unlock();
    bool written = Write_Record(Active_Page_Idx, Next_Slot_Index, &rec);
    Flash_Lock();
    //__enable_irq();

    Next_Slot_Index++;
    if (written)
    {
        Current_Sequence = rec.Sequence;
    }
}

/*******************************************************************************
 * Function Name : Odometer_Init
 * Description   : Scans both Page A and Page B for their highest-sequence
 *                 valid record, and restores Total/Trip distance from
 *                 whichever page holds the most recent trustworthy data.
 *                 Call once at boot, before the main loop starts.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Odometer_Init(void)
{
    Odo_Record_t rec[ODO_PAGE_COUNT];
    bool     valid[ODO_PAGE_COUNT];
    uint32_t free_slot[ODO_PAGE_COUNT];

    Scan_Page(ODO_PAGE_A_IDX, &rec[ODO_PAGE_A_IDX], &valid[ODO_PAGE_A_IDX], &free_slot[ODO_PAGE_A_IDX]);
    Scan_Page(ODO_PAGE_B_IDX, &rec[ODO_PAGE_B_IDX], &valid[ODO_PAGE_B_IDX], &free_slot[ODO_PAGE_B_IDX]);

    if (!valid[ODO_PAGE_A_IDX] && !valid[ODO_PAGE_B_IDX])
    {
        Total_Distance_Km = 0.0;
        Trip_Distance_Km  = 0.0;
        Current_Sequence  = 0;
        Active_Page_Idx   = ODO_PAGE_A_IDX;
        Next_Slot_Index   = free_slot[ODO_PAGE_A_IDX];
        return;
    }

    uint8_t winner_idx;
    if (valid[ODO_PAGE_A_IDX] &&
        (!valid[ODO_PAGE_B_IDX] || rec[ODO_PAGE_A_IDX].Sequence >= rec[ODO_PAGE_B_IDX].Sequence))
    {
        winner_idx = ODO_PAGE_A_IDX;
    }
    else
    {
        winner_idx = ODO_PAGE_B_IDX;
    }

    Total_Distance_Km = rec[winner_idx].Total_M / 1000.0;
    Trip_Distance_Km  = rec[winner_idx].Trip_M  / 1000.0;
    Current_Sequence  = rec[winner_idx].Sequence;
    Active_Page_Idx   = winner_idx;
    Next_Slot_Index   = free_slot[winner_idx];
}

/*******************************************************************************
 * Function Name : Update_Odometer
 * Description   : Non-blocking distance accumulator using vehicle speed in
 *                 km/h. Rejects implausible speed spikes, ignores idle sensor
 *                 noise below a minimum threshold, accumulates distance into
 *                 both the Total and Trip counters, and periodically persists
 *                 both to flash.
 * Scope         : Global
 * Parameters    : Speed_Kmh - Current vehicle speed in km/h (always positive)
 * Return Value  : None
 ******************************************************************************/
void Update_Odometer(float Speed_Kmh)
{
    uint32_t now = Get_Tick_Ms();
    uint32_t elapsed_ms = now - Last_Odo_Update_Tick;

    if (elapsed_ms < ODO_UPDATE_INTERVAL_MS)
    {
        return;
    }

    if (Speed_Kmh > MAX_REALISTIC_SPEED_KMH)
    {
        Last_Odo_Update_Tick = now;
        return;
    }
    if (Speed_Kmh < MIN_SPEED_THRESHOLD_KMH)
    {
        Speed_Kmh = 0.0f;
    }

    double elapsed_hours = elapsed_ms / 3600000.0;
    double distance_km   = (double)Speed_Kmh * elapsed_hours;

    Total_Distance_Km += distance_km;
    Trip_Distance_Km  += distance_km;
    Last_Odo_Update_Tick = now;

    if ((now - Last_Odo_Save_Tick) >= ODO_SAVE_INTERVAL_MS)
    {
        Save_Odometer_To_Storage();
        Last_Odo_Save_Tick = now;
    }
}

/*******************************************************************************
 * Function Name : Reset_Trip_Odometer
 * Description   : Resets only the trip distance to zero, leaving the total
 *                 odometer untouched, and saves immediately since this is a
 *                 deliberate user action worth protecting right away rather
 *                 than waiting for the periodic save interval.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Reset_Trip_Odometer(void)
{
    Trip_Distance_Km = 0.0;
    Save_Odometer_To_Storage();
    Last_Odo_Save_Tick = Get_Tick_Ms();
}


void Check_Trip_Reset_Hold(uint8_t Switch_Level)
{
    uint32_t now = Get_Tick_Ms();
    bool Switch_Pressed = (Switch_Level == TRIP_SWITCH_PRESSED_LEVEL);

    if (Trip_Hold_First_Call)
    {
        Trip_Hold_First_Call = false;
        Trip_Hold_Active     = Switch_Pressed;
        Trip_Hold_Done       = Switch_Pressed;
        Trip_Hold_Start_Tick = now;
        return;
    }

    if (Switch_Pressed)
    {
        if (!Trip_Hold_Active)
        {
            Trip_Hold_Active     = true;
            Trip_Hold_Start_Tick = now;
        }
        else if (!Trip_Hold_Done &&
                 ((now - Trip_Hold_Start_Tick) >= TRIP_HOLD_RESET_MS))
        {
            Trip_Hold_Done = true;
            Reset_Trip_Odometer();

        }
    }
    else
    {
        Trip_Hold_Active = false;
        Trip_Hold_Done   = false;
    }
}
/*******************************************************************************
 * Function Name : Get_Odometer_Km
 * Description   : Returns the current total accumulated distance.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : double (total distance in kilometers)
 ******************************************************************************/
double Get_Odometer_Km(void)
{
    return Total_Distance_Km;
}

/*******************************************************************************
 * Function Name : Get_Trip_Km
 * Description   : Returns the current trip accumulated distance.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : double (trip distance in kilometers)
 ******************************************************************************/
double Get_Trip_Km(void)
{
    return Trip_Distance_Km;
}

