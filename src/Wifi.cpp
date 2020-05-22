#include "Wifi.h"

const char *payload = "zadzialaj w koncu chujcu";

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "websocket ";
const char *WIFITAG = "wifi ";

static int s_retry_num = 0;

static EventGroupHandle_t s_wifi_event_group;

xQueueHandle wifiRxQueue;
xQueueHandle wifiTxQueue;


static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void WifiStart()
{

        esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");



    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            SSID,
            PASS,
            WIFI_FAST_SCAN,
            false,
            //{236, 8, 107, 55, 106, 12},
            {116, 60, 24, 74, 40, 64},
            0,
            0,
            WIFI_CONNECT_AP_BY_SIGNAL,
            0

        }};
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 SSID, PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 SSID, PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    vEventGroupDelete(s_wifi_event_group);
}

void tcp_client_task(void *pvParameters)
{
    printf("starting client task \n");
    //char rx_buffer[128];
    char addr_str[128];
    recSockStruct receivedData;
    sendSockStruct sendData;
    int addr_family;
    int ip_protocol;

    while (1)
    {
        //
        struct sockaddr_in destAddr;
        destAddr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);
        ESP_LOGE(TAG, "ronning on core: %d", xPortGetCoreID());
        int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", HOST_IP_ADDR, PORT);
        // while(true){
        // vTaskDelay(1000);
        // ESP_LOGI(TAG, "zz");
        // }
        int err = connect(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Succesfully connected");
        //fcntl(sock, O_NONBLOCK, 0);
        //
        while (1)
        {
            //f f f

            if (uxQueueMessagesWaiting(wifiTxQueue) != 0)
            {
                // printf("chcialbym wyslac: %d \n", uxQueueMessagesWaiting(wifiTxQueue));
                xQueueReceive(wifiTxQueue, &(sendData), (TickType_t)0);

                int err = send(sock, &sendData, sizeof(sendData), 0);
                if (err < 0)
                {
                    ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
                    break;
                }

                //}

                //printf("jestem przed \n");
                int len = recv(sock, &receivedData, sizeof(receivedData), 0); //-1
                //printf("doszedlem \n");
                // Error occured during receiving
                if (len < 0)
                {
                    ESP_LOGE(TAG, "recv failed: errno %d", errno);
                    //break;
                }
                // Data received
                else
                {
                    //rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                    //ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                    //ESP_LOGI(TAG, "%f, %f, %f, %f", receivedData.P, receivedData.I, receivedData.D, receivedData.Fz);
                    if (xQueueSendToBack(wifiRxQueue, (void *)&receivedData, (TickType_t)10) != pdPASS)
                    {
                        ESP_LOGE(TAG, "couldn't send received data to main task");
                    }
                }
            }

            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        if (sock != -1)
        {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
            //break;
        }
         vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    //vTaskDelete(NULL);
    vTaskDelay(1);
}

void wait_for_ip()
{
    ESP_LOGI(TAG, "Waiting for AP connection...");
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to AP");
}

void startSocket()
{
    printf("\n \nstarting socket \n \n\n");
    vTaskDelay(5000/portTICK_PERIOD_MS);

    wifiRxQueue = xQueueCreate(10, sizeof(recSockStruct));
    wifiTxQueue = xQueueCreate(10, sizeof(sendSockStruct));
    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 1, NULL);
}

recSockStruct sockSendReceive(sendSockStruct sendData, recSockStruct lastData)
{
    recSockStruct receivedData;
    if (uxQueueMessagesWaiting(wifiTxQueue) < 10)
    {
        if (xQueueSendToBack(wifiTxQueue, (void *)&sendData, (TickType_t)10) != pdPASS)
        {
            ESP_LOGE("sendSocket", "couldn't send data to socket task");
        }
    }
    //printf("sendQueue length: %d \n", uxQueueMessagesWaiting(wifiTxQueue));

    if (uxQueueMessagesWaiting(wifiRxQueue) != 0)
    {
        xQueueReceive(wifiRxQueue, &(receivedData), (TickType_t)0);
        //printf("Main task got: %f, %f, %f \n", receivedData.P, receivedData.I, receivedData.D);
    }
    else
    {
        receivedData = lastData;
    }
    
    return receivedData;
}