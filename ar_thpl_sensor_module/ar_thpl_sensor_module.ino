#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_HTU21DF.h>
#include <Adafruit_TSL2561_U.h>


typedef struct SensorData
{
    float tempF;
    float tempC;
    float humidity;
    float pressure;
    float luminosity;
};

// Set some global variables
SensorData sd;
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);


void setup(void)
{
    Serial.begin(9600);

    if(!bmp.begin()) {
        Serial.println("ERROR: Check BMP180 wiring");
        while(1);
    }

    if (!htu.begin()) {
        Serial.println("ERROR: Check HTU21DF wiring");
        while (1);
    }

    if(!tsl.begin()){
        Serial.print("ERROR: Check TSL2561 wiring");
        while(1);
    }

    configureLuminositySensor();
}


void loop(void)
{
    readBMP180(&sd);
    readHTU21DF(&sd);
    readTSL2561(&sd);
    writeAllSensorDataToSerial();
    delay(10000);
}


void configureLuminositySensor(void)
{
  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
}


float celsiusToFahrenheit(float celsius)
{
    return (1.8 * celsius) + 32;
}


void readBMP180(struct SensorData *sd)
{
    sensors_event_t event;
    bmp.getEvent(&event);

    float tempC;
    bmp.getTemperature(&tempC);

    sd->pressure = event.pressure;
    sd->tempC = tempC;
    sd->tempF = celsiusToFahrenheit(tempC);
}


void readHTU21DF(struct SensorData *sd)
{
    sd->humidity = htu.readHumidity();
}


void readTSL2561(struct SensorData *sd)
{
    sensors_event_t event;
    tsl.getEvent(&event);
    sd->luminosity = event.light;
}


void writeAllSensorDataToSerial(void)
{
    String jsonString = "{\"temp_f\":";
    jsonString += sd.tempF;
    jsonString += ",\"temp_c\":";
    jsonString += sd.tempC;
    jsonString += ",\"humidity\":";
    jsonString += sd.humidity;
    jsonString += ",\"pressure\":";
    jsonString += sd.pressure;
    jsonString += ",\"luminosity\":";
    jsonString += sd.luminosity;
    jsonString += "}";
    Serial.println(jsonString);
}
