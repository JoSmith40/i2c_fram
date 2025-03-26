/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_types.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2c_master.h"
#include "i2c_fram.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_FRAM_MAX_TRANS_UNIT (48)
// Different FRAM device might share one I2C bus

static const char TAG[] = "i2c-fram";

esp_err_t i2c_fram_init(i2c_master_bus_handle_t bus_handle, const i2c_fram_config_t *fram_config, i2c_fram_handle_t *fram_handle)
{
    esp_err_t ret = ESP_OK;
    i2c_fram_handle_t out_handle;
    out_handle = (i2c_fram_handle_t)calloc(1, sizeof(*out_handle));
    ESP_GOTO_ON_FALSE(out_handle, ESP_ERR_NO_MEM, err, TAG, "no memory for i2c fram device");

    i2c_device_config_t i2c_dev_conf = {
        .scl_speed_hz = fram_config->fram_device.scl_speed_hz,
        .device_address = fram_config->fram_device.device_address,
    };

    if (out_handle->i2c_dev == NULL) {
        ESP_GOTO_ON_ERROR(i2c_master_bus_add_device(bus_handle, &i2c_dev_conf, &out_handle->i2c_dev), err, TAG, "i2c new bus failed");
    }

    out_handle->buffer = (uint8_t*)calloc(1, fram_config->addr_wordlen + I2C_FRAM_MAX_TRANS_UNIT);
    ESP_GOTO_ON_FALSE(out_handle->buffer, ESP_ERR_NO_MEM, err, TAG, "no memory for i2c fram device buffer");

    out_handle->addr_wordlen = fram_config->addr_wordlen;
    out_handle->write_time_ms = fram_config->write_time_ms;
    *fram_handle = out_handle;

    return ESP_OK;

err:
    if (out_handle && out_handle->i2c_dev) {
        i2c_master_bus_rm_device(out_handle->i2c_dev);
    }
    free(out_handle);
    return ret;
}

esp_err_t i2c_fram_write(i2c_fram_handle_t fram_handle, uint32_t address, const uint8_t *data, uint32_t size)
{
    ESP_RETURN_ON_FALSE(fram_handle, ESP_ERR_NO_MEM, TAG, "no mem for buffer");
    for (int i = 0; i < fram_handle->addr_wordlen; i++) {
        fram_handle->buffer[i] = (address & (0xff << ((fram_handle->addr_wordlen - 1 - i) * 8))) >> ((fram_handle->addr_wordlen - 1 - i) * 8);
    }
    memcpy(fram_handle->buffer + fram_handle->addr_wordlen, data, size);

    return i2c_master_transmit(fram_handle->i2c_dev, fram_handle->buffer, fram_handle->addr_wordlen + size, -1);
}

esp_err_t i2c_fram_read(i2c_fram_handle_t fram_handle, uint32_t address, uint8_t *data, uint32_t size)
{
    ESP_RETURN_ON_FALSE(fram_handle, ESP_ERR_NO_MEM, TAG, "no mem for buffer");
    for (int i = 0; i < fram_handle->addr_wordlen; i++) {
        fram_handle->buffer[i] = (address & (0xff << ((fram_handle->addr_wordlen - 1 - i) * 8))) >> ((fram_handle->addr_wordlen - 1 - i) * 8);
    }

    return i2c_master_transmit_receive(fram_handle->i2c_dev, fram_handle->buffer, fram_handle->addr_wordlen, data, size, -1);
}

void i2c_fram_wait_idle(i2c_fram_handle_t fram_handle)
{
    // This is time for FRAM Self-Timed Write Cycle
    vTaskDelay(pdMS_TO_TICKS(fram_handle->write_time_ms));
}
