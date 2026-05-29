#pragma once

#include "driver/i2c_master.h"

#define LCD_I2C_ADDRESS 0x27

void lcd_init(i2c_master_dev_handle_t dev);
void lcd_clear(i2c_master_dev_handle_t dev);
void lcd_set_cursor(i2c_master_dev_handle_t dev, uint8_t row, uint8_t col);
void lcd_send_string(i2c_master_dev_handle_t dev, const char *str);