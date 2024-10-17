#include "driver/gpio.h"
#include "driver/i2c.h"
#include "unistd.h"
#include "string.h"
#include "defs.h"

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
