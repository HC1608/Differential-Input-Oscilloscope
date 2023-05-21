/*#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/adc.h"
#include "hardware/timer.h"

#define ADC_PIN 26
#define UART_ID uart0
#define BAUD_RATE 115200
#define BUFFER_SIZE 1024

static uint16_t buffer[BUFFER_SIZE];
static uint16_t buffer_index = 0;
static uint32_t duration_ms = 0;

void timer_callback(void) {
    uint16_t value = adc_read();
    buffer[buffer_index] = value;
    buffer_index = (buffer_index + 1) % BUFFER_SIZE;
}

void uart_rx_callback() {
    uint8_t data = uart_getc(UART_ID);
    if (data == 'T') {
        // Transmit buffer over UART
        uart_write(UART_ID, buffer, BUFFER_SIZE * sizeof(uint16_t));
    } else if (data == 'D') {
        // Get duration from UART
        uint8_t duration_buffer[4];
        uart_read_blocking(UART_ID, duration_buffer, 4);
        duration_ms = *(uint32_t*)duration_buffer;

        // Reset buffer index and timer interval
        buffer_index = 0;
        uint32_t timer_interval_us = duration_ms * 1000;

        // Update timer interval
        cancel_alarm(timer_callback);
        add_alarm_in_us(timer_interval_us, timer_callback, true);
    }
}

void uart_tx_callback() {
    // Nothing to do here
}

int main() {
    stdio_init_all();

    // Set up ADC on channel 0
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);

    // Set up UART with baud rate 115200
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(UART_ID, true);

    // Set up UART interrupt handlers
    uart_set_irq_enables(UART_ID, true, false);
    irq_set_exclusive_handler(UART_ID == uart0 ? UART0_IRQ : UART1_IRQ, uart_rx_callback);
    irq_set_enabled(UART_ID == uart0 ? UART0_IRQ : UART1_IRQ, true);
    irq_set_exclusive_handler(UART_ID == uart0 ? UART0_IRQ : UART1_IRQ, uart_tx_callback);
    irq_set_enabled(UART_ID == uart0 ? UART0_IRQ : UART1_IRQ, true);

    // Set up timer to trigger callback every duration_ms milliseconds
    uint32_t timer_interval_us = duration_ms * 1000;
    add_alarm_in_us(timer_interval_us, timer_callback, true);

    while (1) {
        // Do nothing here
    }

    return 0;
}
*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <string.h>
#include <unistd.h>

#define UART_ID uart0
#define BAUD_RATE 9600

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

const uint LED_PIN = 25;
#define BUFFER_SIZE 250


float values1[BUFFER_SIZE];
float values2[BUFFER_SIZE];

int main() {
    stdio_init_all();
    
    uart_init(UART_ID, BAUD_RATE);
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
   // printf("ADC Example, measuring GPIO26\n");

    adc_init();

    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    adc_gpio_init(27);
    // Select ADC input 0 (GPIO26)
    //adc_select_input(0);
    int temp=0;
    
    while (1) {
        //gpio_put(LED_PIN, 0);
        //sleep_ms(250);
        //gpio_put(LED_PIN, 1);
        //printf("Hello World\n");
        //sleep_ms(1000);
        const float conversion_factor = 3.3f / (1 << 12);
        adc_select_input(0);
        uint16_t result1 = adc_read();
        adc_select_input(1);
        uint16_t result2 = adc_read();
        //printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
        //uart_puts(UART_ID, "Bluetooth working!\n");
        //sleep_ms(1);
        values1[temp]=result1 * conversion_factor;
        values2[temp]=result2 * conversion_factor;
        temp+=1;

        if(temp%BUFFER_SIZE==0){
            //int t;
            //scanf("%d",t);
            printf("CH1");
            uart_puts(UART_ID, "CH1 ");
            for(int i=0;i<BUFFER_SIZE;i++){
              //  write(1,&values[i],sizeof(float));
                char buf[6];
                gcvt(values1[i], 6, buf);
                printf(" %f", values1[i]);
                uart_puts(UART_ID, buf);
                uart_puts(UART_ID, " ");
            }
            //char c = 'p';
            //fwrite(&c, sizeof(char), 1, stdout);
            //char *c = "Hello world";
            //write (1,c,strlen(c)+1);
            printf("\n");
            uart_puts(UART_ID, "\n");

            printf("CH2");
            uart_puts(UART_ID, "CH2 ");
            for(int i=0;i<BUFFER_SIZE;i++){
                char buf[6];
                gcvt(values2[i], 6, buf);
                printf(" %f", values2[i]);
                uart_puts(UART_ID, buf);
                uart_puts(UART_ID, " ");
            }
            printf("\n");
            uart_puts(UART_ID, "\n");
            temp=0;
            sleep_ms(1);
        }
    }
}
