#ifndef I2C_WRAPPER_H_
#define I2C_WRAPPER_H_

// Параметры i2c
#define I2C_BUS_NUM 0   // Номер порта i2c 
#define SLAVE_ADDR 0x0A // Адрес slave-а

void configure_i2c(i2c_port_t i2c_port_num, int gpio_sda_num, int gpio_scl_num,\
              i2c_mode_t mode, uint32_t clk_speed, uint32_t clk_flags);

esp_err_t i2c_master_send_nb(i2c_port_t i2c_port_num, uint8_t slave_addr,\
                  const uint8_t *data_ptr, size_t data_len);

#endif // I2C_WRAPPER_H_
