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


#define SSID "Jezus cie widzi 2.4"
#define PASS  "lubieplacki"

#define HOST_IP_ADDR "192.168.0.107"
#define PORT 8080



    typedef struct
    {
        float P;
        float I;
        float D;
    } recSockStruct;

        typedef struct
    {
        float R;
        float P;
        float Y;
    } sendSockStruct;

    esp_err_t event_handler(void *ctx, system_event_t *event);

    void WifiStart();

    void tcp_client_task(void *pvParameters);
    void wait_for_ip();

    void startSocket();

    recSockStruct sockSendReceive(sendSockStruct sendData);