#ifndef _GPIO_H_
#define _GPIO_H_

// Флаги прерываний для пина
enum pin_intr_type{
  PIN_INTR_DISABLE,
  PIN_INTR_RISING_EDGE,
  PIN_INTR_FALLING_EDGE,
  PIN_INTR_ANY_EDGE,
  PIN_INTR_LOW_LEVEL,
  PIN_INTR_HIGH_LEVEL
};

/*---------  Регистры  ---------*/
#define GPIO_BASE           0x3FF44000
#define GPIO_IN_REG         ((volatile uint32_t *)(GPIO_BASE+0x003c))
#define GPIO_ENABLE_REG     ((volatile uint32_t *)(GPIO_BASE+0x0020))
#define GPIO_OUT_W1TS_REG   ((volatile uint32_t *)(GPIO_BASE+0x0008))
#define GPIO_OUT_W1TC_REG   ((volatile uint32_t *)(GPIO_BASE+0x000c))
#define GPIO_PINn_REG(n)    ((volatile uint32_t *)(GPIO_BASE+0x88+0x4*n))

#define IO_MUX_BASE         0x3FF49000
#define IO_MUX_x_REG(x)     ((volatile uint32_t *)(IO_MUX_BASE+0x10+4*(x-1)))
/*---------            ---------*/


#endif // _GPIO_H_