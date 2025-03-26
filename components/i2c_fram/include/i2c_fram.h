/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <stdint.h>
#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    i2c_device_config_t fram_device;  /*!< Configuration for fram device */
    uint8_t addr_wordlen;               /*!< block address wordlen */
    uint8_t write_time_ms;              /*!< fram write time, typically 10ms*/
} i2c_fram_config_t;

struct i2c_fram_t {
    i2c_master_dev_handle_t i2c_dev;      /*!< I2C device handle */
    uint8_t addr_wordlen;                 /*!< block address wordlen */
    uint8_t *buffer;                      /*!< I2C transaction buffer */
    uint8_t write_time_ms;                /*!< I2C fram write time(ms)*/
};

typedef struct i2c_fram_t i2c_fram_t;

/* handle of FRAM device */
typedef struct i2c_fram_t *i2c_fram_handle_t;

/**
 * @brief Init an FRAM device.
 *
 * @param[in] bus_handle I2C master bus handle
 * @param[in] fram_config Configuration of FRAM
 * @param[out] fram_handle Handle of FRAM
 * @return ESP_OK: Init success. ESP_FAIL: Not success.
 */
esp_err_t i2c_fram_init(i2c_master_bus_handle_t bus_handle, const i2c_fram_config_t *fram_config, i2c_fram_handle_t *fram_handle);

/**
 * @brief Write data to FRAM
 *
 * @param[in] fram_handle FRAM handle
 * @param[in] address Block address inside FRAM
 * @param[in] data Data to write
 * @param[in] size Data write size
 * @return ESP_OK: Write success. Otherwise failed, please check I2C function fail reason.
 */
esp_err_t i2c_fram_write(i2c_fram_handle_t fram_handle, uint32_t address, const uint8_t *data, uint32_t size);

/**
 * @brief Read data from FRAM
 *
 * @param fram_handle FRAM handle
 * @param address Block address inside FRAM
 * @param data Data read from FRAM
 * @param size Data read size
 * @return ESP_OK: Read success. Otherwise failed, please check I2C function fail reason.
 */
esp_err_t i2c_fram_read(i2c_fram_handle_t fram_handle, uint32_t address, uint8_t *data, uint32_t size);

/**
 * @brief Wait fram finish. Typically 5ms
 *
 * @param fram_handle FRAM handle
 */
void i2c_fram_wait_idle(i2c_fram_handle_t fram_handle);

#ifdef __cplusplus
}
#endif
