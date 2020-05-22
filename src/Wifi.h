#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "sys/time.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include <stdbool.h>
#include "math.h"

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "string.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "sys/param.h"
#include "esp_event.h"
#include "tcpip_adapter.h"


//#define SSID "INEA-7A57_2.4G"
#define SSID "Jezus cie widzi 2.4"
#define PASS "lubieplacki"
//#define PASS  "2982155898"
#define EXAMPLE_ESP_MAXIMUM_RETRY  5

//#define HOST_IP_ADDR "192.168.1.66"
#define HOST_IP_ADDR "192.168.0.106"
#define PORT 8080



    typedef struct
    {
        float P;
        float I;
        float D;
        float Fz;
    } recSockStruct;

        typedef struct
    {
        float R;
        float P;
        float Y;
        float u;
    } sendSockStruct;


    esp_err_t event_handler(void *ctx, system_event_t *event);

    void WifiStart();

    void tcp_client_task(void *pvParameters);
    void wait_for_ip();

    void startSocket();

    recSockStruct sockSendReceive(sendSockStruct sendData, recSockStruct lastData);