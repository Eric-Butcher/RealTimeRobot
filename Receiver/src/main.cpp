#include <Arduino.h>
#include <ArduinoBLE.h>
#include <array>

static const constexpr char *CENTRAL_NAME = "DUCKS_Central";

static const constexpr char *CONTROLLER_UUID = "547b5676-0377-480f-b6f8-2a94873c07ec";
static const constexpr char *THUMB_STICK_X_AXIS_UUID = "5b03b0ef-c8db-4ef0-adf6-09e23d41a68d";
static const constexpr char *THUMB_STICK_Y_AXIS_UUID = "b1169c28-5e12-4213-a4ff-0aa316f233cc";
static const constexpr char *THUMB_STICK_BUTTON_UUID = "c2dd566c-65bb-4d28-8708-227c923433cf";
static const constexpr char *YELLOW_BUTTON_UUID = "b273ac1b-e05f-4e47-a508-6c87d89e46eb";
static const constexpr char *RED_BUTTON_UUID = "6db0dbd7-8830-4e3d-b885-baf0e4c75d93";
static const constexpr char *GREEN_BUTTON_UUID = "ec339d10-06c3-4ad0-80dc-0066f0fea2b9";
static const constexpr char *BLUE_BUTTON_UUID = "473abf6f-2591-427d-9ae9-9546d01e2287";

static const constexpr int NUMBER_OF_CHARACTERISTICS = 7;

typedef enum Result {
    SUCCESS,
    ERROR
};

struct ControllerState {
    int thumb_stick_x_axis;
    int thumb_stick_y_axis;
    byte thumb_stick_button;
    byte yellow_button;
    byte red_button;
    byte green_button;
    byte blue_button;
};


Result subscribe_to_characteristics(std::array<BLECharacteristic, NUMBER_OF_CHARACTERISTICS> characteristics){
    for (int i = 0; i < characteristics.size(); ++i){
        BLECharacteristic characteristic = characteristics[i];
        String id_string = "index: " + String(i) + " and UUID:  " + String(characteristic.uuid());
        Serial.println("Attempting to subscribe to a characteristic at " + id_string + ".");
        if (!characteristic){
            Serial.println("The characteristic at " + id_string + " does not exist.");
            return ERROR;
        }
        if (!characteristic.canSubscribe()){
            Serial.println("The characteristic at " + id_string + " is not subscribable.");
            return ERROR;
        }
        if (!characteristic.subscribe()){
            Serial.println("Failed to subscribe to characteristic at " + id_string + ".");
            return ERROR;
        }
        Serial.println("Subscribed to characteristic at " + id_string + ".");
    }
    return SUCCESS;
}

void monitor_controller_state(BLEDevice controller)
{
    // reference: SensorTagButton.ino sketch from ArduinoBLE/examples/Central/SensorTagButton/SensorTagButton.ino
    // accessed 11/16/2025

    Serial.println("Connecting to the controller device...");
    if (controller.connect()) {
        Serial.println("Connected to the controller device. ");
    } else {
        Serial.println("Failed to connect to the controller device.");
        return;
    }

    bool res = controller.discoverService(CONTROLLER_UUID);
    Serial.println("Result of discover service: " + String(res));
    res = controller.discoverAttributes();
    Serial.println("Result of discoverAttributes: " + String(res));



    if (!controller.hasService(CONTROLLER_UUID)){
        Serial.println("The controller does not have the controller service as expected!");
    }
    BLEService controller_service = controller.service(CONTROLLER_UUID);

    BLECharacteristic thumb_stick_x_axis_characteristic = controller.characteristic(THUMB_STICK_X_AXIS_UUID);
    BLECharacteristic thumb_stick_y_axis_characteristic = controller.characteristic(THUMB_STICK_Y_AXIS_UUID);
    BLECharacteristic thumb_stick_button_characteristic = controller.characteristic(THUMB_STICK_BUTTON_UUID);
    BLECharacteristic yellow_button_characteristic = controller.characteristic(YELLOW_BUTTON_UUID);
    BLECharacteristic red_button_characteristic = controller.characteristic(RED_BUTTON_UUID);
    BLECharacteristic green_button_characteristic = controller.characteristic(GREEN_BUTTON_UUID);
    BLECharacteristic blue_button_characteristic = controller.characteristic(BLUE_BUTTON_UUID);

    // BLECharacteristic thumb_stick_x_axis_characteristic = controller.characteristic(0);
    // BLECharacteristic thumb_stick_y_axis_characteristic = controller.characteristic(1);
    // BLECharacteristic thumb_stick_button_characteristic = controller.characteristic(2);
    // BLECharacteristic yellow_button_characteristic = controller.characteristic(3);
    // BLECharacteristic red_button_characteristic = controller.characteristic(4);
    // BLECharacteristic green_button_characteristic = controller.characteristic(5);
    // BLECharacteristic blue_button_characteristic = controller.characteristic(6);


    std::array<BLECharacteristic, NUMBER_OF_CHARACTERISTICS> characteristics = {
        thumb_stick_x_axis_characteristic,
        thumb_stick_y_axis_characteristic,
        thumb_stick_button_characteristic,
        yellow_button_characteristic,
        red_button_characteristic,
        green_button_characteristic,
        blue_button_characteristic
    };

    Result rs = subscribe_to_characteristics(characteristics);
    if (rs == ERROR){
        return;
    }

    ControllerState controller_state = {};
    const int wait_interval_ms = 500;
    int previous_time_ms = 0;
    while (controller.connected()){

        int current_time_ms = millis();
        if (current_time_ms > (previous_time_ms + wait_interval_ms)){
            previous_time_ms = current_time_ms;
        } else {
            continue;
        }
        
        if (thumb_stick_x_axis_characteristic.valueUpdated()){
            thumb_stick_x_axis_characteristic.readValue(&controller_state.thumb_stick_x_axis, sizeof(int));
        } 
        if (thumb_stick_y_axis_characteristic.valueUpdated()){
            thumb_stick_y_axis_characteristic.readValue(&controller_state.thumb_stick_y_axis, sizeof(int));
        } 
        if (thumb_stick_button_characteristic.valueUpdated()){
            thumb_stick_button_characteristic.readValue(&controller_state.thumb_stick_button, sizeof(byte));
        } 
        if (yellow_button_characteristic.valueUpdated()){
            yellow_button_characteristic.readValue(&controller_state.yellow_button, sizeof(byte));
        } 
        if (red_button_characteristic.valueUpdated()){
            red_button_characteristic.readValue(&controller_state.red_button, sizeof(byte));
        } 
        if (green_button_characteristic.valueUpdated()){
            green_button_characteristic.readValue(&controller_state.green_button, sizeof(byte));
        } 
        if (blue_button_characteristic.valueUpdated()){
            blue_button_characteristic.readValue(&controller_state.blue_button, sizeof(byte));
        } 

        Serial.println("The x_axis is: " + String(controller_state.thumb_stick_x_axis));
        Serial.println("The y_axis is: " + String(controller_state.thumb_stick_y_axis));
        Serial.println("The thumb_stick_button is: " + String(controller_state.thumb_stick_button));
        Serial.println("The yellow_button is: " + String(controller_state.yellow_button));
        Serial.println("The red_button is: " + String(controller_state.red_button));
        Serial.println("The green_button is: " + String(controller_state.green_button));
        Serial.println("The blue_button is: " + String(controller_state.blue_button));
        Serial.println();
    }
 
    return;
}


void setup()
{
    Serial.begin(9600);
    while (!Serial);

    if (!BLE.begin())
    {
        Serial.println("ERROR: Failed to initialize bluetooth low energy.");
        while (true);
    }

    BLE.setDeviceName(CENTRAL_NAME);
    BLE.setLocalName(CENTRAL_NAME);

    Serial.println("Scanning for controller.");
    BLE.scanForUuid(CONTROLLER_UUID);
}

void loop()
{
    // reference: SensorTagButton.ino sketch from ArduinoBLE/examples/Central/SensorTagButton/SensorTagButton.ino
    // accessed 11/16/2025

    BLEDevice peripheral = BLE.available();

    if (peripheral){
        BLEDevice& p = peripheral;
        String msg = "Found a peripheral with address: " + p.address() + ", localName: " + p.localName()  + ", serviceUUID: " + p.advertisedServiceUuid(); + ".";
        Serial.println(msg);
        if (!p.hasService(CONTROLLER_UUID)){
                Serial.println("The controller does not have the controller service in loop to start!");
        }

        if (p.advertisedServiceUuid() == CONTROLLER_UUID) {
            BLE.stopScan();

            if (!p.hasService(CONTROLLER_UUID)){
                Serial.println("The controller does not have the controller service in loop after stop scan!");
            }

            monitor_controller_state(p);

            BLE.scan();
        }
    }
}
