#include "Wifi.h"

void WifiStart()
{

    wifi_scan_threshold_t scan = {0, WIFI_AUTH_OPEN};

    nvs_flash_init();
    wifi_init_config_t wifiInitializationConfig = WIFI_INIT_CONFIG_DEFAULT();

    esp_wifi_init(&wifiInitializationConfig);

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
            // 7,
            // 0,
            // WIFI_AUTH_OPEN,
            // 0,
            // 4,
            // 100
        }};

    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();
    esp_wifi_connect();
}