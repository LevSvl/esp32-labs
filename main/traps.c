#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "unistd.h"
#include "string.h"
#include "defs.h"

// Очередь для отправки по i2c
#define I2C_EVENT_QUEUE_SIZE  10
static QueueHandle_t i2c_event_queue = 0;

// Быстрый обработчик прерываний для шины i2c
// i2c_isr_handler добавляет прервавшее 
// событие и его параметры в глобальную
// очередь i2c_event_queue, которую далее
// видит медленный обработчик прерываний 
// i2c - i2c_irq
static void IRAM_ATTR
i2c_isr_handler(void *arg)
{
    unsigned int event_flag = (unsigned int)arg;
    xQueueSendFromISR(i2c_event_queue, &event_flag, NULL);  
}

// Медленный обработчик прерываний для шины i2c.
// Постоянно проверяет очередь i2c_event_queue в 
// ожидании события I2C_READ, и когда такое наступает -
// заставляет i2c_master_msg сформировать отправить запрос 
// на чтение от master-а к slave-у.
void
i2c_irq()
{
    unsigned int event;
    while (1){
        if(xQueueReceive(i2c_event_queue, &event, 10)){
            uint8_t byte_of_data;
            if(event == I2C_READ){
                printf("Reading data...\n");

                i2c_master_msg(I2C_BUS_NUM, SLAVE_ADDR,\
                    &byte_of_data, sizeof(byte_of_data), 0);
                
                printf("Recieved data: %d\n", byte_of_data);
            }
        }
    }
}

void
configure_traps()
{
    // Инициализация очереди событий для i2c
    i2c_event_queue = xQueueCreate(I2C_EVENT_QUEUE_SIZE, sizeof(unsigned int));
    
    // Включить прерывания
    gpio_install_isr_service(0);

    // Обработчик прерываний для отправки i2c
    gpio_isr_handler_add(BUTTON1_GPIO_NUM, i2c_isr_handler, (void*)I2C_READ);

    // // Обработчик прерываний для чтения i2c
    // // по сигналу на новую ножку INTERRUPT_GPIO_NUM. 
    // // Сигнал будет посылать slave-устройство
    // // при отправке байтов master-устройству 
    // gpio_isr_handler_add(INTERRUPT_GPIO_NUM, i2c_isr_handler, (void*)I2C_TRAP);

    // Драйвер для работы с i2c, запущенный как отдельный процесс FreeRTOS 
    xTaskCreate(i2c_irq, "i2c_irq", 2048, NULL, 10, NULL);
}
