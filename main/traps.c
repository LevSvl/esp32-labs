#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "unistd.h"
#include "string.h"
#include "defs.h"


// Очередь для обработчика прерываний кнопок
#define BUTTON_QUEUE_SIZE  10
static QueueHandle_t gpio_event_queue = 0;

// Очередь для отправки по i2c
#define I2C_EVENT_QUEUE_SIZE  10
static QueueHandle_t i2c_event_queue = 0;


// Обработчик прерываний для кнопок. Положит номер
// прервавшей ножки в очередь gpio_event_queue
static void IRAM_ATTR
button_isr_handler(void *arg)
{
    unsigned int gpio_num = (unsigned int)arg;
    xQueueSendFromISR(gpio_event_queue, &gpio_num, NULL);
}
static void IRAM_ATTR
i2c_isr_handler(void *arg)
{
    unsigned int event_flag = (unsigned int)arg;
    xQueueSendFromISR(i2c_event_queue, &event_flag, NULL);  
}

// Драйвер для кнопки, при прерывании обработчик положит
// номер прервавшей ножки в очередь, button_irq в отдельной
// задаче достанет номер из очереди, считает с ножки сигнал
// и если кнопка зажата - включит светодиод
void
button_irq()
{
    unsigned int io_num;
    while(1){
        if(xQueueReceive(gpio_event_queue, &io_num, 100)){
            // Если кнопка уже была зажата - не реагировать
            int io_lvl = gpio_get_level(io_num);
            if(io_lvl == 1)
                continue;

            gpio_set_level(LED_GPIO_NUM, 1);

            usleep(5);
        }
    }
}

void
i2c_irq()
{
    unsigned int event;
    while (1){
        if(xQueueReceive(i2c_event_queue, &event, 1000)){
            uint8_t byte_of_data;
            if(event == I2C_READ){
                i2c_master_read_nb(I2C_BUS_NUM, SLAVE_ADDR,\
                    &byte_of_data, sizeof(byte_of_data));

                printf("Recieved data: %d", byte_of_data);
            } else if(event == I2C_WRITE){
                byte_of_data = DATA_TO_SEND;

                i2c_master_send_nb(I2C_BUS_NUM, SLAVE_ADDR,\
                    &byte_of_data, sizeof(byte_of_data));
                
                printf("Sent data: %d\n", byte_of_data);
            }
        }
    }
    

}

void
configure_traps()
{
    // Инициализация очередей
    gpio_event_queue = xQueueCreate(BUTTON_QUEUE_SIZE, sizeof(unsigned int));
    i2c_event_queue = xQueueCreate(I2C_EVENT_QUEUE_SIZE, sizeof(unsigned int));
    
    
    // Включить прерывания
    gpio_install_isr_service(0);

    
    // Обработчик прерываний для кнопки, (void*)BUTTON - его входной параметр
    gpio_isr_handler_add(BUTTON1_GPIO_NUM, button_isr_handler, (void*)BUTTON1_GPIO_NUM);

    // Обработчик прерываний для отправки i2c
    gpio_isr_handler_add(BUTTON1_GPIO_NUM, i2c_isr_handler, (void*)I2C_WRITE);

    // Обработчик прерываний для чтения i2c
    gpio_isr_handler_add(SDA_GPIO_NUM, i2c_isr_handler, (void*)I2C_READ);


    // Драйвер для кнопки, запущенный как отдельный процесс FreeRTOS 
    xTaskCreate(button_irq, "button_irq", 2048, NULL, 10, NULL);

    // Драйвер для работы с i2c, запущенный как отдельный процесс FreeRTOS 
    xTaskCreate(i2c_irq, "i2c_irq", 2048, NULL, 10, NULL);
}
