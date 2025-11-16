#include <Arduino.h>
#include <ArduinoBLE.h>

// Setup this device to act as a Bluetooth Low Energy Peripheral
// see Arduino's BLE documentation to better understand the library:
// https://docs.arduino.cc/libraries/arduinoble/#Usage/Examples

/*
We will need to stream a service with characteristics for each
type of user input which should include:
- X-axs from the thumbstick
- Y-axis from the thumstick
- Button press from the thumbstick
- Button press (Yellow)
- Button press (Red)
- Button press (Green)
- Button press (Blue)
  */
static const constexpr char *CENTRAL_NAME = "DUCKS_Central";
static const constexpr char *CENTRAL_ADDRESS = "f4:12:fa:6d:71:2d";

static const constexpr char *CONTROLLER_UUID = "547b5676-0377-480f-b6f8-2a94873c07ec";
static const constexpr char *THUMB_STICK_X_AXIS_UUID = "5b03b0ef-c8db-4ef0-adf6-09e23d41a68d";
static const constexpr char *THUMB_STICK_Y_AXIS_UUID = "b1169c28-5e12-4213-a4ff-0aa316f233cc";
static const constexpr char *THUMB_STICK_BUTTON_UUID = "c2dd566c-65bb-4d28-8708-227c923433cf";
static const constexpr char *YELLOW_BUTTON_UUID = "b273ac1b-e05f-4e47-a508-6c87d89e46eb";
static const constexpr char *RED_BUTTON_UUID = "6db0dbd7-8830-4e3d-b885-baf0e4c75d93";
static const constexpr char *GREEN_BUTTON_UUID = "ec339d10-06c3-4ad0-80dc-0066f0fea2b9";
static const constexpr char *BLUE_BUTTON_UUID = "473abf6f-2591-427d-9ae9-9546d01e2287";

BLEService controller_service(CONTROLLER_UUID);

BLEIntCharacteristic thumb_stick_x_axis_characteristic(THUMB_STICK_X_AXIS_UUID, BLERead | BLENotify);
BLEIntCharacteristic thumb_stick_y_axis_characteristic(THUMB_STICK_Y_AXIS_UUID, BLERead | BLENotify);
BLEByteCharacteristic thumb_stick_button_characteristic(THUMB_STICK_BUTTON_UUID, BLERead | BLENotify);
BLEByteCharacteristic yellow_button_characteristic(YELLOW_BUTTON_UUID, BLERead | BLENotify);
BLEByteCharacteristic red_button_characteristic(RED_BUTTON_UUID, BLERead | BLENotify);
BLEByteCharacteristic green_button_characteristic(GREEN_BUTTON_UUID, BLERead | BLENotify);
BLEByteCharacteristic blue_button_characteristic(BLUE_BUTTON_UUID, BLERead | BLENotify);


// Define all of the pin outs for this sketch:

static const constexpr int ANALOG_PIN_0 = A0;
static const constexpr int ANALOG_PIN_1 = A1;

static const constexpr int DIGITAL_PIN_2 = 2;
static const constexpr int DIGITAL_PIN_3 = 3;
static const constexpr int DIGITAL_PIN_4 = 4;
static const constexpr int DIGITAL_PIN_5 = 5;
static const constexpr int DIGITAL_PIN_6 = 6;

static const constexpr int DIGITAL_PIN_7 = 7;
static const constexpr int DIGITAL_PIN_8 = 8;


static const constexpr int THUMB_STICK_X_AXIS = ANALOG_PIN_0;
static const constexpr int THUMB_STICK_Y_AXIS = ANALOG_PIN_1;
static const constexpr int THUMB_STICK_BUTTON = DIGITAL_PIN_2;

static const constexpr int YELLOW_BUTTON = DIGITAL_PIN_3;
static const constexpr int RED_BUTTON = DIGITAL_PIN_4;
static const constexpr int GREEN_BUTTON = DIGITAL_PIN_5;
static const constexpr int BLUE_BUTTON = DIGITAL_PIN_6;

static const constexpr int BLUETOOTH_STATUS_LED = DIGITAL_PIN_7;
static const constexpr int POWER_STATUS_LED = DIGITAL_PIN_8;

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
    pinMode(THUMB_STICK_X_AXIS, INPUT);
    pinMode(THUMB_STICK_Y_AXIS, INPUT);
    pinMode(THUMB_STICK_BUTTON, INPUT_PULLUP);
    pinMode(YELLOW_BUTTON, INPUT_PULLUP);
    pinMode(RED_BUTTON, INPUT_PULLUP);
    pinMode(GREEN_BUTTON, INPUT_PULLUP);
    pinMode(BLUE_BUTTON, INPUT_PULLUP);
}

void serial_debug_inputs()
{
    int x_axis = analogRead(THUMB_STICK_X_AXIS);
    int y_axis = analogRead(THUMB_STICK_Y_AXIS);
    int thumb_stick_button = digitalRead(THUMB_STICK_BUTTON);
    int yellow_button = digitalRead(YELLOW_BUTTON);
    int red_button = digitalRead(RED_BUTTON);
    int green_button = digitalRead(GREEN_BUTTON);
    int blue_button = digitalRead(BLUE_BUTTON);

    Serial.println("The x_axis is: " + String(x_axis));
    Serial.println("The y_axis is: " + String(y_axis));
    Serial.println("The thumb_stick_button is: " + String(thumb_stick_button));
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

    int x_axis = analogRead(THUMB_STICK_X_AXIS);
    int y_axis = analogRead(THUMB_STICK_Y_AXIS);
    int thumb_stick_button = digitalRead(THUMB_STICK_BUTTON);
    int yellow_button = digitalRead(YELLOW_BUTTON);
    int red_button = digitalRead(RED_BUTTON);
    int green_button = digitalRead(GREEN_BUTTON);
    int blue_button = digitalRead(BLUE_BUTTON);

    thumb_stick_x_axis_characteristic.writeValue(x_axis);
    thumb_stick_y_axis_characteristic.writeValue(y_axis);
    thumb_stick_button_characteristic.writeValue(thumb_stick_button);
    yellow_button_characteristic.writeValue(yellow_button);
    red_button_characteristic.writeValue(red_button);
    green_button_characteristic.writeValue(green_button);
    blue_button_characteristic.writeValue(blue_button);
}


void setup()
{
    Serial.begin(9600);
    while (!Serial);

    setup_pin_configurations();

    if (!BLE.begin()) {
        Serial.println("ERROR: Failed to initialize bluetooth low energy.");
        initialization_error_loop();
    }

    BLE.setLocalName("DUCKS_Controller");
    BLE.setAdvertisedService(controller_service);
    BLE.setDeviceName("DUCKS_Controller");

    controller_service.addCharacteristic(thumb_stick_x_axis_characteristic);
    controller_service.addCharacteristic(thumb_stick_y_axis_characteristic);
    controller_service.addCharacteristic(thumb_stick_button_characteristic);
    controller_service.addCharacteristic(yellow_button_characteristic);
    controller_service.addCharacteristic(red_button_characteristic);
    controller_service.addCharacteristic(green_button_characteristic);
    controller_service.addCharacteristic(blue_button_characteristic);

    BLE.addService(controller_service);

    // write defaults values to the characteristics to start
    thumb_stick_x_axis_characteristic.writeValue(JOYSTICK_DEFAULT);
    thumb_stick_y_axis_characteristic.writeValue(JOYSTICK_DEFAULT);
    thumb_stick_button_characteristic.writeValue(BUTTON_DEFAULT);
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
            long current_time = millis();
            if (current_time - previous_time >= 200) {
                previous_time = current_time;
                update_controller_state();
            }


        }

        indicate_bluetooth_disconnection(central_device);
        BLE.advertise();

    }

    
}

