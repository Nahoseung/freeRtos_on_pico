#include "pico/stdlib.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

SemaphoreHandle_t led_semaphore;

void led_on_task(void *pvParameters) 
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) 
    {
        gpio_put(LED_PIN, 1);
        printf("LED On Task: LED On\n");
        sleep_ms(2000); //LED ON 지속 시간
        xSemaphoreGive(led_semaphore);
        vTaskDelay(pdMS_TO_TICKS(5000));//LED OFF 지속시간

    }
}

void led_off_task(void *pvParameters) 
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;

    while (true) 
    {
        if (xSemaphoreTake(led_semaphore, portMAX_DELAY) == pdTRUE) 
        {
            gpio_put(LED_PIN, 0);
            printf("LED Off Task: LED Off\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

int main() 
{
    stdio_usb_init();
    sleep_ms(5000);

    led_semaphore = xSemaphoreCreateBinary();

    if (led_semaphore == NULL)
    {
        printf("Failed to create semaphore\n");
        while (true);
    }

    xTaskCreate(led_on_task, "LED_On_Task", 256, NULL, 1, NULL);
    xTaskCreate(led_off_task, "LED_Off_Task", 256, NULL, 1, NULL);
    printf("LED TASK PROGRAM START\n");
    vTaskStartScheduler();

    while (true) ;
}