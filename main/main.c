#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "unistd.h"
#include "string.h"


// GPIO-пины
#define GPIO_NUM_LED     2
#define GPIO_NUM_BUTTON  16

// Очередь для обработчика прерываний
#define QUEUE_SIZE  10
static QueueHandle_t gpio_event_queue = 0;

// Обработчик прерываний для кнопки. Положит номер
// прервавшей ножки в очередь gpio_event_queue
static void IRAM_ATTR
gpio_isr_handler(void *arg)
{
    unsigned int gpio_num = (unsigned int)arg;
    xQueueSendFromISR(gpio_event_queue, &gpio_num, NULL);
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
        if(xQueueReceive(gpio_event_queue, &io_num, 10)){
            int io_lvl = gpio_get_level(io_num);
            if(io_lvl > 0)
                gpio_set_level(GPIO_NUM_LED, 0);
            else
                gpio_set_level(GPIO_NUM_LED, 1);
            
            usleep(5);
        }
    }
}

void
configure_traps()
{
    // Инициализация очереди
    gpio_event_queue = xQueueCreate(QUEUE_SIZE, sizeof(unsigned int));
    
    // Включить прерывания
    gpio_install_isr_service(0);

    // Обработчик прерываний для кнопки, (void*)BUTTON - его входной параметр
    gpio_isr_handler_add(GPIO_NUM_BUTTON, gpio_isr_handler, (void*)GPIO_NUM_BUTTON);

    // Драйвер для кнопки, запущенный как отдельный процесс FreeRTOS 
    xTaskCreate(button_irq, "button_irq", 2048, NULL, 10, NULL);
}

void
configure_io()
{
    gpio_config_t io_conf;

    memset(&io_conf, 0, sizeof(gpio_config_t));
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_LED); // led 
    io_conf.intr_type = GPIO_INTR_DISABLE; // без прерываний
    io_conf.mode = GPIO_MODE_OUTPUT; // режим вывода

    gpio_config(&io_conf);
    
    memset(&io_conf, 0, sizeof(gpio_config_t));
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_BUTTON); // button
    // Прерывания при размыкании и при замыкании контактов
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT; // режим ввода
    io_conf.pull_up_en = 1; // начальный сигнал - логическая 1

    gpio_config(&io_conf);
}

int
app_main(int argc, char const *argv[])
{

    configure_io();
    configure_traps();

    while (1)
    {
        printf("it's infinite loop\n");
        sleep(2);
    }
    

    return 0;
}
