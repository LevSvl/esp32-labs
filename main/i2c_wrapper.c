#include "driver/i2c.h"
#include "i2c_wrapper.h"
#include "string.h"
#include "unistd.h"

void
configure_i2c(i2c_port_t i2c_port_num, int gpio_sda_num, int gpio_scl_num,\
              i2c_mode_t mode, uint32_t clk_speed, uint32_t clk_flags)
{
    i2c_config_t config;
    memset(&config, 0, sizeof(i2c_config_t));

    config.mode = mode;
    config.sda_io_num = gpio_sda_num;
    config.scl_io_num = gpio_scl_num;
    config.master.clk_speed = clk_speed;
    config.clk_flags = clk_flags;

    i2c_param_config(i2c_port_num, &config);

    i2c_driver_install(i2c_port_num, config.mode, 0, 0, 0);
}

esp_err_t
i2c_master_send_nb(i2c_port_t i2c_port_num, uint8_t slave_addr,\
                  const uint8_t *data_ptr, size_t data_len)
{
    esp_err_t rc = ESP_OK;
    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    
    if(handle == NULL)
      return ESP_ERR_NO_MEM;

    int debug_function_id = 0;

    // Стартовый бит
    rc = i2c_master_start(handle);
    if(rc != ESP_OK)
        goto stop;
    debug_function_id = 1;

    // Отправить адресацию
    rc = i2c_master_write_byte(handle, (slave_addr << 1) | I2C_MASTER_WRITE, 1);
    if(rc != ESP_OK)
        goto stop;
    debug_function_id = 2;

    // Отправить байты данных
    rc = i2c_master_write(handle, data_ptr, data_len, I2C_MASTER_LAST_NACK);
    if(rc != ESP_OK)
        goto stop;
    debug_function_id = 3;
    
    // Стоп бит
    i2c_master_stop(handle);

    // Отправить пакет
    rc = i2c_master_cmd_begin(i2c_port_num, handle, 1000 / portTICK_PERIOD_MS);
    if(rc != ESP_OK)
        goto stop;
    debug_function_id = 4;
    usleep(2*1000);

stop:
    if(rc != ESP_OK){
        printf("err after func %d\n", debug_function_id);
        printf("err = %d\n", rc);
    }
    i2c_cmd_link_delete(handle);

    return rc;
}