#include "stdint.h"
#include "string.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "defs.h"
#include "gpio.h"
static void
write_reg(volatile uint32_t *reg, uint32_t val)
{
  *reg |= val;
}

void
configure_gpio()
{
    // Светодиод
    write_reg(GPIO_PINn_REG(LED_GPIO_NUM), PIN_INTR_DISABLE << 9);
    write_reg(GPIO_ENABLE_REG, 1 << LED_GPIO_NUM);

    // Кнопка
    write_reg(GPIO_ENABLE_REG, 0 << BUTTON1_GPIO_NUM);
    write_reg(IO_MUX_x_REG(BUTTON1_GPIO_NUM), 1 << 9); // Разрешить ввод
    write_reg(IO_MUX_x_REG(BUTTON1_GPIO_NUM), 1 << 8); // Поддяжка резистора
    write_reg(GPIO_PINn_REG(BUTTON1_GPIO_NUM), PIN_INTR_LOW_LEVEL << 7);

    // Информирующий пин
    write_reg(GPIO_ENABLE_REG, 1 << INTERRUPT_GPIO_NUM); // Разрешить вывод
    write_reg(IO_MUX_x_REG(INTERRUPT_GPIO_NUM), 1 << 9); // Разрешить ввод
    write_reg(GPIO_PINn_REG(INTERRUPT_GPIO_NUM), PIN_INTR_HIGH_LEVEL << 7);
}
