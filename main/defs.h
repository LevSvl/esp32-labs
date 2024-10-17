#ifndef _DEFS_H_
#define _DEFS_H_

/* Порты ввода-вывода */

#define SDA_GPIO_NUM 21
#define SCL_GPIO_NUM 22
#define LED_GPIO_NUM 2
#define BUTTON1_GPIO_NUM 16
#define INTERRUPT_GPIO_NUM 14


/* Параметры i2c */

// Настройки работы с шиной i2c
#define I2C_BUS_NUM 0   // Номер порта i2c 
#define SLAVE_ADDR 0x0A // Адрес slave-а

// Константы для обработчиков прерываний i2c,
// отражающие события во взаимодействии 
// соединенных устройств
#define I2C_WRITE  0
#define I2C_READ   1
#define I2C_TRAP   2

// Байт данных для отправки 
// на slave-устройства (событие - I2C_WRITE)
#define DATA_TO_SEND  0xFA

/* Объявления функций */

// i2c.c
void configure_i2c(i2c_port_t i2c_port_num, int gpio_sda_num, int gpio_scl_num,\
              i2c_mode_t mode, uint32_t clk_speed, uint32_t clk_flags);

esp_err_t
i2c_master_msg(i2c_port_t i2c_port_num, uint8_t slave_addr,\
                  const uint8_t *data_ptr, size_t data_len,\ 
                  int need_write);

// traps.c
void configure_traps();

// gpio.c
void configure_gpio();

#endif // _DEFS_H_
