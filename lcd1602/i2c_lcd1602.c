#include "i2c_lcd1602.h"
// Some IDEs/translation units may not resolve the ESP-IDF driver header path.
// Provide a minimal fallback declaration so this file can be parsed by tools
// that cannot open "driver/i2c_master.h". At build time the real header
// from ESP-IDF should be available and used instead.
#ifndef DRIVER_I2C_MASTER_H
// Minimal forward declarations used in this translation unit
typedef void* i2c_master_dev_handle_t;
int i2c_master_transmit(i2c_master_dev_handle_t dev, const uint8_t* data, size_t len, int timeout);
#endif
#define LCD_RS          0x01
#define LCD_RW          0x02
#define LCD_EN          0x04
#define LCD_BACKLIGHT   0x08

static void lcd_send_nibble(i2c_master_dev_handle_t dev, uint8_t nibble, uint8_t rs) {
    nibble <<= 4; // Shift the nibble to the upper 4 bits
    uint8_t data = nibble | (rs ? LCD_RS : 0) | LCD_BACKLIGHT; // Set RS and backlight bits
    i2c_master_transmit(dev, &data, 1, -1);
    // Toggle the enable pin
    data |= LCD_EN; // Set enable bit
    i2c_master_transmit(dev, &data, 1, -1);
    data &= ~LCD_EN; // Clear enable bit
    i2c_master_transmit(dev, &data, 1, -1);  
}

static void lcd_send_byte(i2c_master_dev_handle_t dev, uint8_t byte, uint8_t rs) {
    lcd_send_nibble(dev, byte >> 4, rs); // Send upper nibble
    lcd_send_nibble(dev, byte & 0x0F, rs); // Send lower nibble
}

static void lcd_send_char(i2c_master_dev_handle_t dev, char c) {
    lcd_send_byte(dev, c, 1); // Send character with RS = 1
}

static void lcd_send_command(i2c_master_dev_handle_t dev, uint8_t cmd) {
    lcd_send_byte(dev, cmd, 0); // Send command with RS = 0
}

static void lcd_send_string(i2c_master_dev_handle_t dev, const char* str) {
    while(*str) {
        lcd_send_char(dev, *str++);
    }
}