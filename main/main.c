#include "driver/gpio.h"
#include "unistd.h"

int
app_main(int argc, char const *argv[])
{
    // Настройка портов
    int led_gpio = GPIO_NUM_2;
    int button_gpio = GPIO_NUM_16;

    gpio_reset_pin(led_gpio);
    gpio_reset_pin(button_gpio);

    // Пин светодиода - вывод, Пин кнопки - ввод
    gpio_set_direction(led_gpio, GPIO_MODE_OUTPUT);
    gpio_set_direction(button_gpio, GPIO_MODE_INPUT);

    // Изначальный сигнал от кнопки - 1
    gpio_set_pull_mode(button_gpio, GPIO_PULLUP_ONLY);

    while(1) {
        int button_level = gpio_get_level(button_gpio);

        if(button_level > 0)
            gpio_set_level(led_gpio, 0);
        else
            gpio_set_level(led_gpio, 1);

        usleep(500);
    }

    return 0;
}
