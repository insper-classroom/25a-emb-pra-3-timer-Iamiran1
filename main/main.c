/**
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #include <stdio.h>
 #include "hardware/rtc.h"
 #include "pico/stdlib.h"
 #include "pico/util/datetime.h"
 
 static volatile bool fired = false;
 
 static void alarm_callback(void) {
     fired = true;
 }

 const int ECHO_PIN = 6;
 const int TRIG_PIN = 7;

 bool echo_got = false;
 volatile uint32_t start_us;
 volatile uint32_t end_us;

 void gpio_callback(uint gpio, uint32_t events)
{
    if (gpio_get(ECHO_PIN) == 1){
        echo_got = true;
        start_us =  get_absolute_time();
        end_us = 0;
    }
    else if(gpio_get(ECHO_PIN)==0){
        end_us = get_absolute_time();
    }
}

void send_trig_pulse(){   
    gpio_put(TRIG_PIN,1);
    sleep_ms(5);
    gpio_put(TRIG_PIN,0);
}
 int main() {
    sleep_ms(1000);
     stdio_init_all();
     printf("RTC Alarm Repeat!\n");
     gpio_init(ECHO_PIN);
     gpio_set_dir(ECHO_PIN, GPIO_IN);
     gpio_set_irq_enabled_with_callback(ECHO_PIN,
         GPIO_IRQ_EDGE_RISE |
         GPIO_IRQ_EDGE_FALL,
         true,
         &gpio_callback);
 
     gpio_init(TRIG_PIN);
     gpio_set_dir(TRIG_PIN, GPIO_OUT);
     
     // Start on Wednesday 13th January 2021 11:20:00
     datetime_t t = {
         .year  = 2020,
         .month = 01,
         .day   = 13,
         .dotw  = 3, // 0 is Sunday, so 3 is Wednesday
         .hour  = 11,
         .min   = 20,
         .sec   = 00
     };
 
     // Start the RTC
     rtc_init();
     rtc_set_datetime(&t);
 
     // Alarm once a minute
     datetime_t alarm = {
         .year  = -1,
         .month = -1,
         .day   = -1,
         .dotw  = -1,
         .hour  = -1,
         .min   = -1,
         .sec   = 00
     };
     rtc_set_alarm(&alarm, &alarm_callback);
     // Alarm will keep firing forever
     while(1){
        send_trig_pulse();
        rtc_enable_alarm();
         if (fired) {
             fired = 0;
             datetime_t t = {0};
             rtc_get_datetime(&t);
             char datetime_buf[256];
             char *datetime_str = &datetime_buf[0];
             datetime_to_str(datetime_str, sizeof(datetime_buf), &t);
             printf("%s - Falha \n",datetime_str);
            }
         if(!fired){
            if(echo_got){
                rtc_disable_alarm();
                uint32_t delta_t;
                while(end_us == 0  ){
                    delta_t = end_us - start_us;
                }
                delta_t = end_us - start_us;
                float distancia = (float) delta_t * 0.017015;
                datetime_t t = {0};
                rtc_get_datetime(&t);
                char datetime_buf[256];
                char *datetime_str = &datetime_buf[0];
                datetime_to_str(datetime_str, sizeof(datetime_buf), &t);
                printf("%s - %f cm \n",datetime_str,distancia);
                echo_got = false;
            }
         }
 
     }
 }