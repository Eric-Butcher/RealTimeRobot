#include <Arduino.h>
#include <ArduinoBLE.h>
#include "bt_uuids.hpp"

// Setup this device to act as a Bluetooth Low Energy Peripheral
// see Arduino's BLE documentation to better understand the library:
// https://docs.arduino.cc/libraries/arduinoble/#Usage/Examples


BLEService controller_service(CONTROLLER_UUID);

BLEIntCharacteristic left_thumb_stick_x_axis_characteristic(LEFT_THUMB_STICK_X_AXIS_UUID, BLERead | BLENotify);
BLEIntCharacteristic left_thumb_stick_y_axis_characteristic(LEFT_THUMB_STICK_Y_AXIS_UUID, BLERead | BLENotify);
BLEByteCharacteristic left_thumb_stick_button_characteristic(LEFT_THUMB_STICK_BUTTON_UUID, BLERead | BLENotify);
BLEIntCharacteristic right_thumb_stick_x_axis_characteristic(RIGHT_THUMB_STICK_X_AXIS_UUID, BLERead | BLENotify);
BLEIntCharacteristic right_thumb_stick_y_axis_characteristic(RIGHT_THUMB_STICK_Y_AXIS_UUID, BLERead | BLENotify);
BLEByteCharacteristic right_thumb_stick_button_characteristic(RIGHT_THUMB_STICK_BUTTON_UUID, BLERead | BLENotify);
BLEByteCharacteristic yellow_button_characteristic(YELLOW_BUTTON_UUID, BLERead | BLENotify);
BLEByteCharacteristic red_button_characteristic(RED_BUTTON_UUID, BLERead | BLENotify);
BLEByteCharacteristic green_button_characteristic(GREEN_BUTTON_UUID, BLERead | BLENotify);
BLEByteCharacteristic blue_button_characteristic(BLUE_BUTTON_UUID, BLERead | BLENotify);


// Define all of the pin outs for this sketch:

static const constexpr int ANALOG_PIN_0 = A0;
static const constexpr int ANALOG_PIN_1 = A1;
static const constexpr int ANALOG_PIN_2 = A2;
static const constexpr int ANALOG_PIN_3 = A3;


static const constexpr int DIGITAL_PIN_2 = 2;
static const constexpr int DIGITAL_PIN_3 = 3;
static const constexpr int DIGITAL_PIN_4 = 4;
static const constexpr int DIGITAL_PIN_5 = 5;
static const constexpr int DIGITAL_PIN_6 = 6;
static const constexpr int DIGITAL_PIN_7 = 7;
static const constexpr int DIGITAL_PIN_8 = 8;
static const constexpr int DIGITAL_PIN_9 = 9;



static const constexpr int LEFT_THUMB_STICK_X_AXIS = ANALOG_PIN_0;
static const constexpr int LEFT_THUMB_STICK_Y_AXIS = ANALOG_PIN_1;
static const constexpr int LEFT_THUMB_STICK_BUTTON = DIGITAL_PIN_2;

static const constexpr int RIGHT_THUMB_STICK_X_AXIS = ANALOG_PIN_2;
static const constexpr int RIGHT_THUMB_STICK_Y_AXIS = ANALOG_PIN_3;
static const constexpr int RIGHT_THUMB_STICK_BUTTON = DIGITAL_PIN_3;


static const constexpr int YELLOW_BUTTON = DIGITAL_PIN_4;
static const constexpr int RED_BUTTON = DIGITAL_PIN_5;
static const constexpr int GREEN_BUTTON = DIGITAL_PIN_6;
static const constexpr int BLUE_BUTTON = DIGITAL_PIN_7;

static const constexpr int POWER_STATUS_LED = DIGITAL_PIN_9;
static const constexpr int BLUETOOTH_STATUS_LED = DIGITAL_PIN_8;

// Define constants for the IO ports

static const constexpr int BUTTON_PRESSED = LOW;
static const constexpr int BUTTON_BOT_PRESSED = HIGH;

static const constexpr int JOYSTICK_MIN = 0;
static const constexpr int JOYSTICK_MAX = 0xFFF;
static const constexpr int JOYSTICK_MIDDLE = JOYSTICK_MAX / 2;

static const constexpr int BUTTON_DEFAULT = BUTTON_BOT_PRESSED;
static const constexpr int JOYSTICK_DEFAULT = JOYSTICK_MIDDLE;


inline void setup_pin_configurations()
{
    pinMode(LEFT_THUMB_STICK_X_AXIS, INPUT);
    pinMode(LEFT_THUMB_STICK_Y_AXIS, INPUT);
    pinMode(LEFT_THUMB_STICK_BUTTON, INPUT_PULLUP);
    pinMode(RIGHT_THUMB_STICK_X_AXIS, INPUT);
    pinMode(RIGHT_THUMB_STICK_Y_AXIS, INPUT);
    pinMode(RIGHT_THUMB_STICK_BUTTON, INPUT_PULLUP);
    pinMode(YELLOW_BUTTON, INPUT_PULLUP);
    pinMode(RED_BUTTON, INPUT_PULLUP);
    pinMode(GREEN_BUTTON, INPUT_PULLUP);
    pinMode(BLUE_BUTTON, INPUT_PULLUP);
}

void serial_debug_inputs()
{
    int left_x_axis = analogRead(LEFT_THUMB_STICK_X_AXIS);
    int left_y_axis = analogRead(LEFT_THUMB_STICK_Y_AXIS);
    int left_thumb_stick_button = digitalRead(LEFT_THUMB_STICK_BUTTON);
    int right_x_axis = analogRead(RIGHT_THUMB_STICK_X_AXIS);
    int right_y_axis = analogRead(RIGHT_THUMB_STICK_Y_AXIS);
    int right_thumb_stick_button = digitalRead(RIGHT_THUMB_STICK_BUTTON);
    int yellow_button = digitalRead(YELLOW_BUTTON);
    int red_button = digitalRead(RED_BUTTON);
    int green_button = digitalRead(GREEN_BUTTON);
    int blue_button = digitalRead(BLUE_BUTTON);

    Serial.println("The left_x_axis is: " + String(left_x_axis));
    Serial.println("The left_y_axis is: " + String(left_y_axis));
    Serial.println("The left_thumb_stick_button is: " + String(left_thumb_stick_button));

    Serial.println("The right_x_axis is: " + String(right_x_axis));
    Serial.println("The right_y_axis is: " + String(right_y_axis));
    Serial.println("The right_thumb_stick_button is: " + String(right_thumb_stick_button));

    Serial.println("The yellow_button is: " + String(yellow_button));
    Serial.println("The red_button is: " + String(red_button));
    Serial.println("The green_button is: " + String(green_button));
    Serial.println("The blue_button is: " + String(blue_button));
    Serial.println();
}

// Called when a core component fails to initialize
// Pulsates the power LED forever indicating to the user that something went wrong
void initialization_error_loop()
{
    const int two_hundred_milliseconds = 200;

    digitalWrite(BLUETOOTH_STATUS_LED, 0);
    bool error_led_on = false;
    while (true){
        error_led_on = true;
        digitalWrite(POWER_STATUS_LED, error_led_on);
        error_led_on = !error_led_on;
        delay(two_hundred_milliseconds);
    }
}

void indicate_bluetooth_connection(BLEDevice central_device){
    digitalWrite(BLUETOOTH_STATUS_LED, HIGH);
    Serial.println("Connected to the central_device at: " + central_device.address());
}

void indicate_bluetooth_disconnection(BLEDevice central_device){
    digitalWrite(BLUETOOTH_STATUS_LED, LOW);
    Serial.println("Disconnected from central_device: " + central_device.address());
}

void update_controller_state(){

    int left_x_axis = analogRead(LEFT_THUMB_STICK_X_AXIS);
    int left_y_axis = analogRead(LEFT_THUMB_STICK_Y_AXIS);
    int left_thumb_stick_button = digitalRead(LEFT_THUMB_STICK_BUTTON);
    int right_x_axis = analogRead(RIGHT_THUMB_STICK_X_AXIS);
    int right_y_axis = analogRead(RIGHT_THUMB_STICK_Y_AXIS);
    int right_thumb_stick_button = digitalRead(RIGHT_THUMB_STICK_BUTTON);
    int yellow_button = digitalRead(YELLOW_BUTTON);
    int red_button = digitalRead(RED_BUTTON);
    int green_button = digitalRead(GREEN_BUTTON);
    int blue_button = digitalRead(BLUE_BUTTON);

    left_thumb_stick_x_axis_characteristic.writeValue(left_x_axis);
    left_thumb_stick_y_axis_characteristic.writeValue(left_y_axis);
    left_thumb_stick_button_characteristic.writeValue(left_thumb_stick_button);

    right_thumb_stick_x_axis_characteristic.writeValue(right_x_axis);
    right_thumb_stick_y_axis_characteristic.writeValue(right_y_axis);
    right_thumb_stick_button_characteristic.writeValue(right_thumb_stick_button);

    yellow_button_characteristic.writeValue(yellow_button);
    red_button_characteristic.writeValue(red_button);
    green_button_characteristic.writeValue(green_button);
    blue_button_characteristic.writeValue(blue_button);
}


void setup()
{
    

    setup_pin_configurations();

    digitalWrite(POWER_STATUS_LED, HIGH);

    Serial.begin(9600);
    while (!Serial);


    if (!BLE.begin()) {
        Serial.println("ERROR: Failed to initialize bluetooth low energy.");
        initialization_error_loop();
    }

    BLE.setLocalName("DUCKS_Controller");
    BLE.setAdvertisedService(controller_service);
    BLE.setDeviceName("DUCKS_Controller");

    controller_service.addCharacteristic(left_thumb_stick_x_axis_characteristic);
    controller_service.addCharacteristic(left_thumb_stick_y_axis_characteristic);
    controller_service.addCharacteristic(left_thumb_stick_button_characteristic);

    controller_service.addCharacteristic(right_thumb_stick_x_axis_characteristic);
    controller_service.addCharacteristic(right_thumb_stick_y_axis_characteristic);
    controller_service.addCharacteristic(right_thumb_stick_button_characteristic);

    controller_service.addCharacteristic(yellow_button_characteristic);
    controller_service.addCharacteristic(red_button_characteristic);
    controller_service.addCharacteristic(green_button_characteristic);
    controller_service.addCharacteristic(blue_button_characteristic);

    BLE.addService(controller_service);

    // write defaults values to the characteristics to start
    left_thumb_stick_x_axis_characteristic.writeValue(JOYSTICK_DEFAULT);
    left_thumb_stick_y_axis_characteristic.writeValue(JOYSTICK_DEFAULT);
    left_thumb_stick_button_characteristic.writeValue(BUTTON_DEFAULT);

    right_thumb_stick_x_axis_characteristic.writeValue(JOYSTICK_DEFAULT);
    right_thumb_stick_y_axis_characteristic.writeValue(JOYSTICK_DEFAULT);
    right_thumb_stick_button_characteristic.writeValue(BUTTON_DEFAULT);
    
    yellow_button_characteristic.writeValue(BUTTON_DEFAULT);
    red_button_characteristic.writeValue(BUTTON_DEFAULT);
    green_button_characteristic.writeValue(BUTTON_DEFAULT);
    blue_button_characteristic.writeValue(BUTTON_DEFAULT);

    BLE.advertise();
    Serial.println("Controller is now advertising...");

    
}

void loop()
{
    // reference: BatteryMonitor.ino sketch from ArduinoBLE/examples/Peripheral/BatteryMonitor/BatteryMonitor.ino
    // accessed 11/15/2025
    
    long previous_time = 0;

    BLEDevice central_device = BLE.central(); // hang out here and wait for something to connect
    
    if (central_device){

        if (central_device.address() == CENTRAL_ADDRESS){
            Serial.println("Found the correct central device!");
            indicate_bluetooth_connection(central_device);
        } else {
            Serial.println("The address was: " + central_device.address());
            central_device.disconnect();
        }
        
    
        while (central_device.connected()) {
            
              update_controller_state();
          


        }

        indicate_bluetooth_disconnection(central_device);
        

    }

    
}
