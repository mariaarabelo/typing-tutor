
#include "rtc.h"

// Global variables
real_time_info time_info;   // Structure containing all time information
int rtc_hook_id = RTC_MASK; // Máscara constante = 5

// Subscribing to RTC interrupts
int rtc_subscribe_interrupts() {
    return sys_irqsetpolicy(IRQ_RTC, IRQ_REENABLE | IRQ_EXCLUSIVE, &rtc_hook_id);
}

// Unsubscribing from RTC interrupts
int rtc_unsubscribe_interrupts() {
    return sys_irqrmpolicy(&rtc_hook_id);
}

// Helper function to convert BCD to binary
uint8_t bcd_to_bin(uint8_t bcd_value) {
    return (bcd_value >> 4) * 10 + (bcd_value & 0x0F);
}

// Function to read the current time from the RTC and update the provided structure
int rtc_read_time(real_time_info *time_info) {
    uint32_t temp; // Temporary storage for the values read from the RTC

    if (time_info == NULL) return 1;

    // Reading seconds in BCD and converting to binary
    sys_outb(REGISTER_INPUT, SECONDS);
    sys_inb(REGISTER_OUTPUT, &temp);
    time_info->seconds = bcd_to_bin((uint8_t)temp);

    // Repeat for minutes, hours, day, month, and year
    sys_outb(REGISTER_INPUT, MINUTES);
    sys_inb(REGISTER_OUTPUT, &temp);
    time_info->minutes = bcd_to_bin((uint8_t)temp);

    sys_outb(REGISTER_INPUT, HOURS);
    sys_inb(REGISTER_OUTPUT, &temp);
    time_info->hours = bcd_to_bin((uint8_t)temp);

    sys_outb(REGISTER_INPUT, DAY);
    sys_inb(REGISTER_OUTPUT, &temp);
    time_info->day = bcd_to_bin((uint8_t)temp);

    sys_outb(REGISTER_INPUT, MONTH);
    sys_inb(REGISTER_OUTPUT, &temp);
    time_info->month = bcd_to_bin((uint8_t)temp);

    sys_outb(REGISTER_INPUT, YEAR);
    sys_inb(REGISTER_OUTPUT, &temp);
    time_info->year = bcd_to_bin((uint8_t)temp);

    return 0;
}

