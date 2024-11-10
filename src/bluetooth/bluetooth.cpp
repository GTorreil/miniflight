#include "bluetooth.hpp"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

BLEServer *pServer = NULL;
BLECharacteristic *pRollPCharacteristic = NULL;
BLECharacteristic *pPitchPCharacteristic = NULL;
BLECharacteristic *pYawPCharacteristic = NULL;
BLECharacteristic *pLPFAlphaCharacteristic = NULL;

// Unique IDs for our service and characteristics
#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"
#define CHARACTERISTIC_UUID_ROLL "cba1d466-344c-4be3-ab3f-189f80dd7518"
#define CHARACTERISTIC_UUID_PITCH "cba1d466-344c-4be3-ab3f-189f80dd7519"
#define CHARACTERISTIC_UUID_YAW "cba1d466-344c-4be3-ab3f-189f80dd7520"
#define CHARACTERISTIC_UUID_LPF_ALPHA "cba1d466-344c-4be3-ab3f-189f80dd7521"

class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0)
        {
            float newValue = atof(value.c_str());

            if (pCharacteristic == pRollPCharacteristic)
            {
                Serial.print("New roll P value: ");
                Serial.println(newValue);
                // TODO set new value
            }
            else if (pCharacteristic == pPitchPCharacteristic)
            {
                Serial.print("New pitch P value: ");
                Serial.println(newValue);
                // TODO set new value
            }
            else if (pCharacteristic == pYawPCharacteristic)
            {
                Serial.print("New yaw P value: ");
                Serial.println(newValue);
                // TODO set new value
            }
            else if (pCharacteristic == pLPFAlphaCharacteristic)
            {
                Serial.print("New LPF alpha value: ");
                Serial.println(newValue);
                // TODO set new value
            }
        }
    }
};

void bluetooth_setup()
{
    BLEDevice::init("MiniFlight");
    pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    pRollPCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_ROLL,
        BLECharacteristic::PROPERTY_WRITE);
    pRollPCharacteristic->setCallbacks(new MyCallbacks());

    pPitchPCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_PITCH,
        BLECharacteristic::PROPERTY_WRITE);
    pPitchPCharacteristic->setCallbacks(new MyCallbacks());

    pYawPCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_YAW,
        BLECharacteristic::PROPERTY_WRITE);
    pYawPCharacteristic->setCallbacks(new MyCallbacks());

    pLPFAlphaCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_LPF_ALPHA,
        BLECharacteristic::PROPERTY_WRITE);
    pLPFAlphaCharacteristic->setCallbacks(new MyCallbacks());

    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
}

void bluetooth_loop()
{
    // BLE handle events if necessary
}