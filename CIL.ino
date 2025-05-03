#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include "time.h"  // For timestamps
#include "ExampleFunctions.h"

#define WIFI_SSID "JARVIS"
#define WIFI_PASSWORD "yg5817m9"

#define API_KEY "AIzaSyD0cgSieBfDFptX_GSHu92B-HYyEYwMk98"
#define USER_EMAIL "MikeAlpha@gmail.com"
#define USER_PASSWORD "TangoCharlie"
#define DATABASE_URL "https://clar-geological--compass-default-rtdb.firebaseio.com/"

void sendDataToFirebase();

WiFiClientSecure ssl_client;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000);
FirebaseApp app;
RealtimeDatabase Database;

int sl_no = 1;  // Serial number counter

void setup()
{
    Serial.begin(115200);

    // Connect to Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConnected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Initialize Firebase
    Serial.println("Initializing Firebase...");
    set_ssl_client_insecure_and_buffer(ssl_client);
    initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);

    // Initialize time synchronization
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    waitForTimeSync();

    Serial.println("Firebase Initialized Successfully.");
}

void loop()
{
    sendDataToFirebase();
    delay(1000);  // Sending data every second
}

void waitForTimeSync() {
    struct tm timeinfo;
    Serial.print("Waiting for time synchronization");
    // Loop until time is synced; getLocalTime returns false if time isn't set
    while (!getLocalTime(&timeinfo)) {
        Serial.print(".");
        delay(500);
    }
    // Optionally, check the year to be sure it's not 1970
    if (timeinfo.tm_year + 1900 < 2020) {
        Serial.println("Time still not updated. Waiting further...");
        while (timeinfo.tm_year + 1900 < 2020) {
            delay(500);
            getLocalTime(&timeinfo);
        }
    }
    Serial.println();
    Serial.print("Time synchronized: ");
    Serial.printf("%d-%02d-%02d %02d:%02d:%02d\n",
                  timeinfo.tm_year + 1900,
                  timeinfo.tm_mon + 1,
                  timeinfo.tm_mday,
                  timeinfo.tm_hour,
                  timeinfo.tm_min,
                  timeinfo.tm_sec);
}

void sendDataToFirebase()
{
    // Get the current time
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    char timestamp[30];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);

    // Simulated sensor data (Replace with actual sensor readings)
    int strike_value = random(0, 360);
    int dip_value = random(0, 90);
    int dip_dir_value = (strike_value + 90) % 360;

    //gps
    float latitude = 37.7749;  
    float longitude = -122.4194;

    String gps_string = String(latitude, 6) + "," + String(longitude, 6);

    // Create JSON object with SL No., Timestamp, and Sensor Data
    object_t dip_json, sl_json, ts_json, send_json, strike_json, gps_json, dip_dir_json;
    JsonWriter writer;
    writer.create(sl_json, "slNo", sl_no); 
    writer.create(ts_json, "timestamp", String(timestamp));
    writer.create(strike_json, "strikeValue", strike_value);
    writer.create(dip_json, "dipValue", dip_value);
    writer.create(dip_dir_json, "dipDirValue", dip_dir_value);
    writer.create(gps_json, "gps", gps_string);
    writer.join(send_json, 6, sl_json, ts_json, strike_json, dip_json,dip_dir_json, gps_json);

    // Send data to Firebase
    String path = "/sensor_data/" + String(sl_no);
    bool status = Database.set<object_t>(aClient, path.c_str(), send_json);

    if (status)
    {
        Serial.printf("Data Sent: SL No.: %d, Timestamp: %s, Strike: %d, Dip: %d, Dip Direction: %d\n", sl_no, timestamp, strike_value, dip_value, dip_dir_value);
    }
    else
    {
        Firebase.printf("Error: %s, Code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
    }

    sl_no++;  // Increment SL No. for the next entry
}
