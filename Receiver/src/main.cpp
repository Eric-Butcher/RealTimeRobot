#include <Arduino.h>
#include <ArduinoBLE.h>
#include <array>

static constexpr const char *CENTRAL_NAME = "DUCKS_Central";
static constexpr const char *CENTRAL_ADDRESS = "f4:12:fa:6d:71:2d";

static constexpr const char *CONTROLLER_UUID = "547b5676-0377-480f-b6f8-2a94873c07ec";
static constexpr const char *LEFT_THUMB_STICK_X_AXIS_UUID = "5b03b0ef-c8db-4ef0-adf6-09e23d41a68d";
static constexpr const char *LEFT_THUMB_STICK_Y_AXIS_UUID = "b1169c28-5e12-4213-a4ff-0aa316f233cc";
static constexpr const char *LEFT_THUMB_STICK_BUTTON_UUID = "c2dd566c-65bb-4d28-8708-227c923433cf";
static constexpr const char *RIGHT_THUMB_STICK_X_AXIS_UUID = "37aa7d87-3161-4b8a-8f84-afddaf53ced3";
static constexpr const char *RIGHT_THUMB_STICK_Y_AXIS_UUID = "88d80b83-dcf1-47cf-b7dd-6b8c6d01b072";
static constexpr const char *RIGHT_THUMB_STICK_BUTTON_UUID = "c817eaef-8d21-4df8-a7d7-a4e27c63b390";
static constexpr const char *YELLOW_BUTTON_UUID = "b273ac1b-e05f-4e47-a508-6c87d89e46eb";
static constexpr const char *RED_BUTTON_UUID = "6db0dbd7-8830-4e3d-b885-baf0e4c75d93";
static constexpr const char *GREEN_BUTTON_UUID = "ec339d10-06c3-4ad0-80dc-0066f0fea2b9";
static constexpr const char *BLUE_BUTTON_UUID = "473abf6f-2591-427d-9ae9-9546d01e2287";
static constexpr int NUMBER_OF_CHARACTERISTICS = 10;

typedef enum Result {
    SUCCESS,
    ERROR
};

struct ControllerState {
    int left_thumb_stick_x_axis;
    int left_thumb_stick_y_axis;
    int right_thumb_stick_x_axis;
    int right_thumb_stick_y_axis;
    byte left_thumb_stick_button;
    byte right_thumb_stick_button;
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

    BLECharacteristic left_thumb_stick_x_axis_characteristic = controller.characteristic(LEFT_THUMB_STICK_X_AXIS_UUID);
    BLECharacteristic left_thumb_stick_y_axis_characteristic = controller.characteristic(LEFT_THUMB_STICK_Y_AXIS_UUID);
    BLECharacteristic left_thumb_stick_button_characteristic = controller.characteristic(LEFT_THUMB_STICK_BUTTON_UUID);
    BLECharacteristic right_thumb_stick_x_axis_characteristic = controller.characteristic(RIGHT_THUMB_STICK_X_AXIS_UUID);
    BLECharacteristic right_thumb_stick_y_axis_characteristic = controller.characteristic(RIGHT_THUMB_STICK_Y_AXIS_UUID);
    BLECharacteristic right_thumb_stick_button_characteristic = controller.characteristic(RIGHT_THUMB_STICK_BUTTON_UUID);
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
        left_thumb_stick_x_axis_characteristic,
        left_thumb_stick_y_axis_characteristic,
        left_thumb_stick_button_characteristic,
        right_thumb_stick_x_axis_characteristic,
        right_thumb_stick_y_axis_characteristic,
        right_thumb_stick_button_characteristic,
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
        
        if (left_thumb_stick_x_axis_characteristic.valueUpdated()){
            left_thumb_stick_x_axis_characteristic.readValue(&controller_state.left_thumb_stick_x_axis, sizeof(int));
        } 
        if (left_thumb_stick_y_axis_characteristic.valueUpdated()){
            left_thumb_stick_y_axis_characteristic.readValue(&controller_state.left_thumb_stick_y_axis, sizeof(int));
        } 
        if (left_thumb_stick_button_characteristic.valueUpdated()){
            left_thumb_stick_button_characteristic.readValue(&controller_state.left_thumb_stick_button, sizeof(byte));
        }

        if (right_thumb_stick_x_axis_characteristic.valueUpdated()){
            right_thumb_stick_x_axis_characteristic.readValue(&controller_state.right_thumb_stick_x_axis, sizeof(int));
        } 
        if (right_thumb_stick_y_axis_characteristic.valueUpdated()){
            right_thumb_stick_y_axis_characteristic.readValue(&controller_state.right_thumb_stick_y_axis, sizeof(int));
        } 
        if (right_thumb_stick_button_characteristic.valueUpdated()){
            right_thumb_stick_button_characteristic.readValue(&controller_state.right_thumb_stick_button, sizeof(byte));
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

        Serial.print("The left_x_axis is: " + String(controller_state.left_thumb_stick_x_axis) + "The right_x_axis is: " + String(controller_state.right_thumb_stick_x_axis)+ "\n");
        Serial.print("The left_y_axis is: " + String(controller_state.left_thumb_stick_y_axis) + "The right_y_axis is: " + String(controller_state.right_thumb_stick_y_axis)+ "\n");
        Serial.print("The left_thumb_stick_button is: " + String(controller_state.left_thumb_stick_button) + "The right_thumb_stick_button is: " + String(controller_state.right_thumb_stick_button)+ "\n");
        Serial.print("The yellow_button is: " + String(controller_state.yellow_button)+ "\n");
        Serial.print("The red_button is: " + String(controller_state.red_button)+ "\n");
        Serial.print("The green_button is: " + String(controller_state.green_button)+ "\n");
        Serial.print("The blue_button is: " + String(controller_state.blue_button)+ "\n");
    }
 
    return;
}


void setup()
{
    Serial.begin(9600);
    
    while (!Serial);
    Serial.println("Hellooooooo.");
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
