#include "unistd.h"

/* --- REGS --- */
#define GPIO_BASE           0x3FF44000
#define GPIO_ENABLE_REG     (GPIO_BASE+0x0020)
#define GPIO_OUT_W1TS_REG   (GPIO_BASE+0x0008)
#define GPIO_OUT_W1TC_REG   (GPIO_BASE+0x000c)
/* ---      --- */

/* --- FUNCS --- */
#define W_GPIO_ENABLE_REG(val) ((*(volatile uint32_t*)GPIO_ENABLE_REG) = val)
#define W_GPIO_OUT_W1TS_REG(val) ((*(volatile uint32_t*)GPIO_OUT_W1TS_REG) = val)
#define W_GPIO_OUT_W1TC_REG(val) ((*(volatile uint32_t*)GPIO_OUT_W1TC_REG) = val)
/* ---       --- */

int
app_main(int argc, char const *argv[])
{
    // Настройка портов
    uint32_t led_gpio = 2;
    
    // Настройить на вывод
    W_GPIO_ENABLE_REG(1 << led_gpio); // разрешить вывод

    while(1) {
        W_GPIO_OUT_W1TS_REG(1 << led_gpio);
        sleep(1);
        W_GPIO_OUT_W1TC_REG(1 << led_gpio);
        sleep(1);
    }

    return 0;
}
