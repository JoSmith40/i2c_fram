/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache 2.0
 *
 * Dieser Code wurde aus der Lib I2C_EEPROM erstellt.
 * Der Code wurde für die Verwendung mit FRAM angepasst.
 *
 * Date: 03.2025
 * Ersteller: Jo.Smith40
 */
#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "i2c_fram.h"

#define I2C_SCL CONFIG_I2C_MASTER_SCL
#define I2C_SDA CONFIG_I2C_MASTER_SDA
#define MASTER_FREQUENCY CONFIG_I2C_MASTER_FREQUENCY
#define PORT_NUMBER -1 // Standard Kanal im ESP32 für i2c
#define LENGTH 1024
#define FRAM_I2C_ADDR 0x50
#define FRAM_ADDR 0x0000 // start adress FRAM
uint16_t FRAMblock = 21; // block

// Output printf
static void print_buf(uint8_t *buf, int len) // len ist die Länge des Buffers
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%02x ", buf[i]); // Darstellung des Inhalt von buf (Hexadezimal 2Stellen)
        if ((i + 1) % FRAMblock == 0) { // Blockgröße für die Ausgabe
            printf("\n"); // Zeilenende
        }
    }
    printf("\n"); //Blockende
}

    void app_main(void)
    {
        // Config I2C-Bus
        i2c_master_bus_config_t i2c_bus_config = {
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .i2c_port = PORT_NUMBER,
            .scl_io_num = I2C_SCL,
            .sda_io_num = I2C_SDA,
            .glitch_ignore_cnt = 7, // Glitch
        };
        i2c_master_bus_handle_t bus_handle;
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));

        // Config FRAM
        i2c_fram_config_t fram_config = {
            .fram_device.scl_speed_hz = MASTER_FREQUENCY,
            .fram_device.device_address = FRAM_I2C_ADDR,
            .addr_wordlen = 2,
            .write_time_ms = 0,
        };
        i2c_fram_handle_t fram_handle;

        // Data buffer to write to FRAM
        uint8_t buf[LENGTH];
        for (int i = 0; i < LENGTH; i++)
        {
            buf[i] = i; // filling Buffer with i
            // buf[i] = 0xff;  // or filling Buffer with 0xff
            // buf[i] = 0x00;  // or filling Buffer with 0x00
        }

        // Buffer for data from the FRAM
        uint8_t read_buf[LENGTH];
        i2c_fram_init(bus_handle, &fram_config, &fram_handle);

        // loop
        while (1)
        {
            i2c_fram_write(fram_handle, FRAM_ADDR, buf, LENGTH);
            i2c_fram_read(fram_handle, FRAM_ADDR, read_buf, LENGTH);
            print_buf(read_buf, LENGTH);

            vTaskDelay(pdMS_TO_TICKS(1000)); // 1000ms delay
        }
    }
