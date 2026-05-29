#include "i2c_lcd1602.h"

#define LCD_RS          0x01
#define LCD_RW          0x02
#define LCD_EN          0x04
#define LCD_BACKLIGHT   0x08

#define LCD_DELAY_MS(ms) vTaskDelay((ms) / portTICK_PERIOD_MS)

/**
 * @brief Send a 4-bit nibble to the LCD display
 * @param dev I2C master device handle
 * @param nibble 4-bit data to send (lower 4 bits are used)
 * @param rs Register select bit (0 for command, 1 for data)
 * @note This function sends a 4-bit nibble to the LCD by shifting it to the upper 4 bits and setting the appropriate 
 * control bits for RS and backlight. 
 * It then toggles the enable pin to latch the data into the LCD.
 */
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

/**
 * @brief Send a byte to the LCD display
 * @param dev I2C master device handle
 * @param byte Byte to send (command or data)
 * @param rs Register select bit (0 for command, 1 for data)
 * @note This function sends a byte to the LCD by splitting it into two nibbles. The upper nibble is sent first, followed by the lower nibble. 
 * The RS bit determines whether the byte is a command or data.
 */
static void lcd_send_byte(i2c_master_dev_handle_t dev, uint8_t byte, uint8_t rs) {
    lcd_send_nibble(dev, byte >> 4, rs); // Send upper nibble
    lcd_send_nibble(dev, byte & 0x0F, rs); // Send lower nibble
}

/**
 * @brief Send a single character to the LCD display
 * @param dev I2C master device handle
 * @param c Character to send
 * @note This function sends a single character to the LCD by calling the lcd_send_byte function with the RS bit set to 1.
 */
static void lcd_send_char(i2c_master_dev_handle_t dev, char c) {
    lcd_send_byte(dev, c, 1); // Send character with RS = 1
}

/**
 * @brief Send a command to the LCD display
 * @param dev I2C master device handle
 * @param cmd Command byte to send
 * @note This function sends a command byte to the LCD by calling the lcd_send_byte function with the RS bit set to 0. 
 * The command byte should be one of the standard LCD commands defined in the LCD1602 datasheet.
 */
static void lcd_send_command(i2c_master_dev_handle_t dev, uint8_t cmd) {
    lcd_send_byte(dev, cmd, 0); // Send command with RS = 0
}

/**
 * @brief Send a string to the LCD display
 * @param dev I2C master device handle
 * @param str Null-terminated string to display
 * @note This function iterates through each character in the string and sends it to the LCD using the lcd_send_char function. The string must be null-terminated.
 */
void lcd_send_string(i2c_master_dev_handle_t dev, const char* str) {
    while(*str) {
        lcd_send_char(dev, *str++);
    }
}

/**
 * @brief Initialize the LCD1602 display in 4-bit mode
 * @param dev I2C master device handle
 * @note This function must be called before any other LCD operations. I2C has to be initialized.
 */
void lcd_init(i2c_master_dev_handle_t dev) {
    // Wait for LCD to power up
    LCD_DELAY_MS(50); 
    // Function set: 8-bit mode
    for(int i = 0; i < 3; i++) {
        lcd_send_nibble(dev, 0x03, 0); 
        LCD_DELAY_MS(5);
    }
    // Function set: 4-bit mode 
    lcd_send_nibble(dev, 0x02, 0);   
    // Function set: 4-bit mode, 2 lines, 5x8 dots
    lcd_send_command(dev, 0x28); 
    // Display off
    lcd_send_command(dev, 0x08); 
    // Clear display
    lcd_send_command(dev, 0x01);
    LCD_DELAY_MS(5); 
    // Entry mode - 4 bit
    lcd_send_command(dev, 0x06); 
    // Display on, cursor off, blink off
    lcd_send_command(dev, 0x0C);    
}

/**
 * @brief Clear the LCD display
 * @param dev I2C master device handle
 * @note This function sends the clear command to the LCD and waits for it to process.
 */
void lcd_clear(i2c_master_dev_handle_t dev) {
    lcd_send_command(dev, 0x01); 
    LCD_DELAY_MS(5);
}

/**
 * @brief Set the cursor position on the LCD
 * @param dev I2C master device handle
 * @param row Row number (0 or 1)
 * @param col Column number (0-15)
 * @note This function calculates the DDRAM address based on the row and column and sends the appropriate command to set the cursor position.
 */
void lcd_set_cursor(i2c_master_dev_handle_t dev, uint8_t row, uint8_t col) {
    lcd_send_command(dev, 0x80 + (row ? 0x40 : 0) + col);
}