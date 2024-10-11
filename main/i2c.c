#include "driver/i2c.h"
#include "defs.h"
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

    // Ведомое устройство отреагирует на запрос от master-а
    // не сразу, таймаут по времени поможет избежать ошибок,
    // связанных со временем ожидания ответа, и ситуаций,
    // когда master получил неверные данные из-за того, что
    // опрашиваемое устройство не успело выставить часть данных
    // на шину
    i2c_set_timeout(i2c_port_num, 0xFFFFF);
}

// i2c_master_msg инициирует сеанс передачи информации
// между устройствами. 
//
// Во избежание задержек сначала создается
// так называемый буфер команд (команды буфера формируют пакет 
// данных, на которые указывает data_ptr, для отправки), затем
// с помощью функции i2c_master_cmd_begin сформированный пакет
// отправляется к назначенному устройству с адресом slave_addr.
// 
// За направление передачи отвечает переменная need_write, 
// тем не менее в любом случае передачу начинает master, 
// отправляя байт адресации заданному slave-устройству
esp_err_t
i2c_master_msg(i2c_port_t i2c_port_num, uint8_t slave_addr,\
                const uint8_t *data_ptr, size_t data_len,\
                int need_write)
{
    // Далее будет отражать состояние пакета
    esp_err_t rc = ESP_OK;

    // Создание буфера команд в динамической памяти ->
    // -> в конце работы требуется освободить память
    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    
    // Если не получилось выделить память
    if(handle == NULL)
      return ESP_ERR_NO_MEM;

    // В случае проблем будет показывать, где именно произошла ошибка
    int debug_function_id = 0;

    // Стартовый бит
    rc = i2c_master_start(handle);
    if(rc != ESP_OK)
        goto stop;
    debug_function_id = 1;

    // Направление передачи, необходимо для байта адресации
    i2c_rw_t direction;
    if(need_write)
        direction = I2C_MASTER_WRITE;
    else
        direction = I2C_MASTER_READ;

    // Отправить байт адресации
    rc = i2c_master_write_byte(handle, (slave_addr << 1) | direction, 1);
    if(rc != ESP_OK)
        goto stop;
    debug_function_id = 2;

    // Передача
    if(need_write)
        rc = i2c_master_write(handle, data_ptr, data_len, I2C_MASTER_LAST_NACK);
    else
        rc = i2c_master_read(handle, data_ptr, data_len, I2C_MASTER_LAST_NACK);  
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
    
    // Дополнительная задержка
    usleep(3*1000);

stop:
    if(rc != ESP_OK){
        printf("err after func %d\n", debug_function_id);
        printf("err = %d\n", rc);
    }
    // Освобождение памяти, выделенной под буфер команд
    i2c_cmd_link_delete(handle);

    return rc;
}
