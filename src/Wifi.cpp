#include "Wifi.h"

const char *payload = "zadzialaj w koncu chujcu";

const int WIFI_CONNECTED_BIT = BIT0;

const char *TAG = "websocket ";
const char *WIFITAG = "wifi ";

int s_retry_num = 0;

EventGroupHandle_t s_wifi_event_group;

xQueueHandle wifiRxQueue;
xQueueHandle wifiTxQueue;

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(WIFITAG, "got ip:%s",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    {
        if (s_retry_num < 5)
        {
            esp_wifi_connect();
            xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            s_retry_num++;
            ESP_LOGI(WIFITAG, "retry to connect to the AP");
        }
        ESP_LOGI(WIFITAG, "connect to the AP fail\n");
        break;
    }
    default:
        break;
    }
    return ESP_OK;
}

void WifiStart()
{

    wifi_scan_threshold_t scan = {0, WIFI_AUTH_OPEN};

    //nvs flash init
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    //create event group for wifi events
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    tcpip_adapter_init();

    //init default wifi config
    wifi_init_config_t wifiInitializationConfig = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&wifiInitializationConfig));

    esp_wifi_set_storage(WIFI_STORAGE_RAM);

    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t ap_config = {
        .sta = {
            SSID,
            PASS,
            WIFI_FAST_SCAN,
            false,
            {236, 8, 107, 55, 106, 12},
            0,
            0,
            WIFI_CONNECT_AP_BY_SIGNAL,
            scan

        }};

    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();
    esp_wifi_connect();
    ESP_LOGI(WIFITAG, "wifi_init_sta finished.");
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

        int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        int err = connect(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err != 0)
        {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Successfully connected");
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
                    ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                    ESP_LOGI(TAG, "%f, %f, %f", receivedData.P, receivedData.I, receivedData.D);
                    if (xQueueSendToBack(wifiRxQueue, (void *)&receivedData, (TickType_t)10) != pdPASS)
                    {
                        ESP_LOGE(TAG, "couldn't send received data to main task");
                    }
                }
            }

            // //vTaskDelay(200 / portTICK_PERIOD_MS);
        }
        if (sock != -1)
        {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
            //break;
        }
    }
    vTaskDelete(NULL);
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
    wait_for_ip();

    wifiRxQueue = xQueueCreate(10, sizeof(recSockStruct));
    wifiTxQueue = xQueueCreate(10, sizeof(sendSockStruct));
    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 1, NULL);
}