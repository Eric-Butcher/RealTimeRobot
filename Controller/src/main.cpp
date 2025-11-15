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
- Button press (left)
- Button press (right)
- Button press (Yellow)
- Button press (Red)
- Button press (Green)
- Button press (Blue)
  */
static const constexpr char *CONTROLLER_UUID = "547b5676-0377-480f-b6f8-2a94873c07ec";
static const constexpr char *THUMB_STICK_X_AXIS_UUID = "5b03b0ef-c8db-4ef0-adf6-09e23d41a68d";
static const constexpr char *THUMB_STICK_Y_AXIS_UUID = "b1169c28-5e12-4213-a4ff-0aa316f233cc";
static const constexpr char *THUMB_STICK_BUTTON_UUID = "c2dd566c-65bb-4d28-8708-227c923433cf";
static const constexpr char *LEFT_BUTTON_UUID = "b31360d1-67a1-4109-ad9e-6e9b94dbcaf3";
static const constexpr char *RIGHT_BUTTON_UUID = "4b2c5edb-89a0-4794-86c9-40a6398bb085";
static const constexpr char *YELLOW_BUTTON_UUID = "b273ac1b-e05f-4e47-a508-6c87d89e46eb";
static const constexpr char *RED_BUTTON_UUID = "6db0dbd7-8830-4e3d-b885-baf0e4c75d93";
static const constexpr char *GREEN_BUTTON_UUID = "ec339d10-06c3-4ad0-80dc-0066f0fea2b9";
static const constexpr char *BLUE_BUTTON_UUID = "473abf6f-2591-427d-9ae9-9546d01e2287";

BLEService controllerService(CONTROLLER_UUID);

// Define all of the pin outs for this sketch
static const constexpr int ANALOG_PIN_0 = 0;
static const constexpr int ANALOG_PIN_1 = 1;

static const constexpr int DIGITAL_PIN_2 = 2;
static const constexpr int DIGITAL_PIN_3 = 3;

static const constexpr int DEADMAN_BUTTON = DIGITAL_PIN_2;
static const constexpr int THUMB_STICK_X_AXIS = ANALOG_PIN_0;
static const constexpr int THUMB_STICK_Y_AXIS = ANALOG_PIN_1;
static const constexpr int THUMB_STICK_BUTTON = DIGITAL_PIN_3;

inline void setup_pin_configurations()
{
    pinMode(DEADMAN_BUTTON, INPUT_PULLUP);
    pinMode(THUMB_STICK_X_AXIS, INPUT);
    pinMode(THUMB_STICK_Y_AXIS, INPUT);
    pinMode(THUMB_STICK_BUTTON, INPUT_PULLUP);
}

void setup()
{
    Serial.begin(9600);
    setup_pin_configurations();
}

void loop()
{
    // put your main code here, to run repeatedly:

    int deadman_value = digitalRead(DEADMAN_BUTTON);
    int x_axis = analogRead(THUMB_STICK_X_AXIS);
    int y_axis = analogRead(THUMB_STICK_Y_AXIS);
    int thumb_stick_button = digitalRead(THUMB_STICK_BUTTON);

    Serial.println("The deadman_value is: " + deadman_value);
    Serial.println("The x_axis is: " + x_axis);
    Serial.println("The y_axis is: " + y_axis);
    Serial.println("The thumb_stick_button is: " + thumb_stick_button);

    delay(100);
}
