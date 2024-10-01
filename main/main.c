#include "driver/gpio.h"
#include "driver/i2c.h"

#include "unistd.h"
#include "string.h"
#include "i2c_wrapper.h"

// Используемые порты
#define SDA_GPIO_NUM 21
#define SCL_GPIO_NUM 22
#define LED_GPIO_NUM 2

void
configure_gpio()
{
    gpio_reset_pin(LED_GPIO_NUM);
    gpio_set_direction(LED_GPIO_NUM, GPIO_MODE_OUTPUT);
}

void
blink_led_once()
{
    gpio_set_level(LED_GPIO_NUM, 1);
    sleep(2);
    gpio_set_level(LED_GPIO_NUM, 0);
}

int
app_main(int argc, char const *argv[])
{
    configure_gpio();
    configure_i2c(I2C_BUS_NUM, SDA_GPIO_NUM, SCL_GPIO_NUM, I2C_MODE_MASTER, 100000, 0);

    char *data = "Hello_World!!";
    while (1)
    {
        esp_err_t rc = i2c_master_send_nb(I2C_BUS_NUM, SLAVE_ADDR, \
            (const uint8_t*)data, strlen(data));
            
        printf("status = %d\n", rc);
        sleep(5);
    }
    

}
