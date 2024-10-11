#include "driver/gpio.h"
#include "driver/i2c.h"
#include "unistd.h"
#include "string.h"
#include "defs.h"

void
configure_gpio()
{
    gpio_config_t io_conf;

    // Светодиод
    memset(&io_conf, 0, sizeof(gpio_config_t));
    io_conf.pin_bit_mask = (1ULL << LED_GPIO_NUM);
    io_conf.intr_type = GPIO_INTR_DISABLE; // без прерываний
    io_conf.mode = GPIO_MODE_OUTPUT; // режим вывода
    gpio_config(&io_conf);
    
    // Кнопка
    memset(&io_conf, 0, sizeof(gpio_config_t));
    io_conf.pin_bit_mask = (1ULL << BUTTON1_GPIO_NUM);
    // Прерывания при замыкании контактов
    io_conf.intr_type = GPIO_INTR_LOW_LEVEL;
    io_conf.mode = GPIO_MODE_INPUT; // режим ввода
    io_conf.pull_up_en = 1; // начальный сигнал - логическая 1
    gpio_config(&io_conf);

    // Информирующий пин
    memset(&io_conf, 0, sizeof(gpio_config_t));
    io_conf.pin_bit_mask = (1ULL << INTERRUPT_GPIO_NUM);
    io_conf.intr_type = GPIO_INTR_HIGH_LEVEL;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    gpio_config(&io_conf);
}

int
app_main(int argc, char const *argv[])
{
    // Настройка портов ввода-вывода
    configure_gpio();
    // Инициализация прерываний по кнопке
    configure_traps();
    // Инициализация шины i2c
    configure_i2c(I2C_BUS_NUM, SDA_GPIO_NUM, SCL_GPIO_NUM,\
         I2C_MODE_MASTER, 100000, 0);

    while (1) {
        vTaskDelay(1);
    }
    
}
