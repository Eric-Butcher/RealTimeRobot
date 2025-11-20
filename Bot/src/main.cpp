#include <Scheduler.h>
#include <ArduinoBLE.h>
#include <array>
#include <Arduino.h>


/*
 * Values used to determine the speed at which the respective motor is driven. 
 * These values will be sent along either the foward or reverse pins depending on the signal from bluetooth. 
 * A possible way to differentiate between forward and reverse signals, is to make reverse values negative, then take the absolute value
 * before sending to pin. 
 * 
 */
int MOTOR_1_DRIVE = 0;
int MOTOR_2_DRIVE = 0;

/*
 * Specifies minimum drive for the motors
 * Testing revealed values below this fail to drive the motors.
 * 
 * Due to this, motors should be at 0 control indicates such.
 * When controls indicate movement, drive should start and scale from this value. 
 * 
 * This determines, a minimum speed, and should be decreased if min speed is too fast. 
 * 
 */
int MOTOR_MIN = 70;

int MOTOR_1_FWD_PIN = 7;
int MOTOR_1_RV_PIN = 6;

int MOTOR_2_FWD_PIN = 4;
int MOTOR_2_RV_PIN = 5;


static const constexpr char *CONTROLLER_UUID = "547b5676-0377-480f-b6f8-2a94873c07ec";
static const constexpr char *THUMB_STICK_X_AXIS_UUID = "5b03b0ef-c8db-4ef0-adf6-09e23d41a68d";
static const constexpr char *THUMB_STICK_Y_AXIS_UUID = "b1169c28-5e12-4213-a4ff-0aa316f233cc";
static const constexpr char *THUMB_STICK_BUTTON_UUID = "c2dd566c-65bb-4d28-8708-227c923433cf";
static const constexpr char *YELLOW_BUTTON_UUID = "b273ac1b-e05f-4e47-a508-6c87d89e46eb";
static const constexpr char *RED_BUTTON_UUID = "6db0dbd7-8830-4e3d-b885-baf0e4c75d93";
static const constexpr char *GREEN_BUTTON_UUID = "ec339d10-06c3-4ad0-80dc-0066f0fea2b9";
static const constexpr char *BLUE_BUTTON_UUID = "473abf6f-2591-427d-9ae9-9546d01e2287";
static const constexpr char *CENTRAL_NAME = "DUCKS_Central";

//the indexes for motor 1 and 2 characteristics
const int characteristicIndex1 = 0;
const int characteristicIndex2 = 1;

static const constexpr int NUMBER_OF_CHARACTERISTICS = 7;

typedef enum Result {
  SUCCESSFULL,
  FAILURE
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

/*
 * Takes drive information, and handles motor control with it. 
 */
void motor_Driver(){

  
  while (true){
    
    /*
    //motor 1 drive
    if (ControllerState.thumb_stick_y_axis > 1000){
      analogWrite(MOTOR_1_FWD_PIN,0);
      analogWrite(MOTOR_1_RV_PIN,255);
    } else if (ControllerState.thumb_stick_y_axis < 1000) {
      analogWrite(MOTOR_1_RV_PIN,0);
      analogWrite(MOTOR_1_FWD_PIN,255);
    } else {
      analogWrite(MOTOR_1_RV_PIN,0);
      analogWrite(MOTOR_1_FWD_PIN,0);
    }
    */
    /*
    //motor 2 drive
    if (MOTOR_2_DRIVE < 0){
      analogWrite(MOTOR_2_FWD_PIN,0);
      analogWrite(MOTOR_2_RV_PIN,abs(MOTOR_2_DRIVE));
    } else {
      analogWrite(MOTOR_2_RV_PIN,0);
      analogWrite(MOTOR_2_FWD_PIN,MOTOR_2_DRIVE);
    }
  */
    
  }
  //stop all motor movement if safety fails
  
}



Result subscribe_to_characteristics(std::array<BLECharacteristic, NUMBER_OF_CHARACTERISTICS> characteristics){
  for (int i = 0; i < characteristics.size(); ++i){
      BLECharacteristic characteristic = characteristics[i];
      String id_string = "index: " + String(i) + " and UUID:  " + String(characteristic.uuid());
      Serial.println("Attempting to subscribe to a characteristic at " + id_string + ".");
      if (!characteristic){
          Serial.println("The characteristic at " + id_string + " does not exist.");
          return FAILURE;
      }
      if (!characteristic.canSubscribe()){
          Serial.println("The characteristic at " + id_string + " is not subscribable.");
          return FAILURE;
      }
      if (!characteristic.subscribe()){
          Serial.println("Failed to subscribe to characteristic at " + id_string + ".");
          return FAILURE;
      }
      Serial.println("Subscribed to characteristic at " + id_string + ".");
  }
  return SUCCESSFULL;
}



/*
 * Maintains connection to bluetooth peripheral, 
 * and retrieves throttle information. Will continuously update readings
 * as long as connection holds. 
 * 
 * Motor throttle information is stored in drive global variables
 * 
 * If connection drops, will return back to BLEconnection to attempt another connection
 */
void controlled(BLEDevice peripheral){
  Serial.println("attempting to connect to device...");


  //attempt to establish connection
  if (peripheral.connect()){
    Serial.println("Established connected to peripheral device!");
  } else {
    Serial.println("Was unable to establish connection to peripheral device. Returning to retry");
    return;
  }


  bool res = peripheral.discoverService(CONTROLLER_UUID);
  Serial.println("Result of discover service: " + String(res));
  res = peripheral.discoverAttributes();
  Serial.println("Result of discoverAttributes: " + String(res));

  if (!peripheral.hasService(CONTROLLER_UUID)){
    Serial.println("The controller does not have the controller service as expected!");
  }

  BLEService controller_service = peripheral.service(CONTROLLER_UUID);

  BLECharacteristic thumb_stick_x_axis_characteristic = peripheral.characteristic(THUMB_STICK_X_AXIS_UUID);
  BLECharacteristic thumb_stick_y_axis_characteristic = peripheral.characteristic(THUMB_STICK_Y_AXIS_UUID);
  BLECharacteristic thumb_stick_button_characteristic = peripheral.characteristic(THUMB_STICK_BUTTON_UUID);
  BLECharacteristic yellow_button_characteristic = peripheral.characteristic(YELLOW_BUTTON_UUID);
  BLECharacteristic red_button_characteristic = peripheral.characteristic(RED_BUTTON_UUID);
  BLECharacteristic green_button_characteristic = peripheral.characteristic(GREEN_BUTTON_UUID);
  BLECharacteristic blue_button_characteristic = peripheral.characteristic(BLUE_BUTTON_UUID);

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
    if (rs == FAILURE){
        return;
    }

    ControllerState controller_state = {};
    const int wait_interval_ms = 500;
    int previous_time_ms = 0;


  //loop while connected
  while (peripheral.connected()){

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


    //THIS SETS THE MOTOR SPEED BASED ON THE JOYSTICK INPUT
    //RIGHT NOW IT JUST SIMPLY SETS TO MAX AT A CERTAIN POINT
    if (controller_state.thumb_stick_y_axis > 3000){
      analogWrite(MOTOR_1_FWD_PIN,0);
      analogWrite(MOTOR_1_RV_PIN,255);
      analogWrite(MOTOR_2_FWD_PIN,0);
      analogWrite(MOTOR_2_RV_PIN,255);
    } else if (controller_state.thumb_stick_y_axis < 1000) {
      analogWrite(MOTOR_1_RV_PIN,0);
      analogWrite(MOTOR_2_RV_PIN,0);
      analogWrite(MOTOR_1_FWD_PIN,255);
      analogWrite(MOTOR_2_FWD_PIN,255);
    } else {
      analogWrite(MOTOR_1_RV_PIN,0);
      analogWrite(MOTOR_1_FWD_PIN,0);
      analogWrite(MOTOR_2_RV_PIN,0);
      analogWrite(MOTOR_2_FWD_PIN,0);
    }


    
    
    
    
  }

  //stop movement if disconnected


  Serial.println("Peripheral Disconnected");
  return;
  
}


/*
 * Handles the bluetooth connection after intialization
 */
void BLEconnection(){

  BLE.scanForUuid(CONTROLLER_UUID);

  //loop
  while(true){

    BLEDevice peripheral = BLE.available();

    //if found peripheral
    if (peripheral){
       // discovered a peripheral, print out address, local name, and advertised service
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

            controlled(p);

            BLE.scan();
        }
    }

    Serial.println("No peripheral found");
    delay(500);
    
  }
  
}

void testMotors(){
    /*
     * test motor 1 for 0-255 speed forward 
     */


  //Ramp up and down motor 1 forward speed
  
  for (int i=MOTOR_MIN;i<256;i++){
    analogWrite(MOTOR_1_FWD_PIN,i);
    delay(75);
  }
  
  for (int i=255;i>=0;i--){

    if (i < MOTOR_MIN){
      analogWrite(MOTOR_1_FWD_PIN,0);
      break;
    } else {
      analogWrite(MOTOR_1_FWD_PIN,i);
      delay(75);
    }
  }

   //ramp up and down motor 1 reverse speed
  for (int i=MOTOR_MIN;i<256;i++){
    analogWrite(MOTOR_1_RV_PIN,i);
    delay(75);
  }
  
  for (int i=255;i>=0;i--){
    if (i< MOTOR_MIN){
      analogWrite(MOTOR_1_RV_PIN,0);
      break;
    } else {
      analogWrite(MOTOR_1_RV_PIN,i);
      delay(75);
    }
  }


  //ramp up and dowm motor 2 forward speed
 
  for (int i=MOTOR_MIN;i<256;i++){
    analogWrite(MOTOR_2_FWD_PIN,i);
    delay(75);
  }
  for (int i=255;i>=0;i--){
    if (i < MOTOR_MIN){
      analogWrite(MOTOR_2_FWD_PIN,0);
      break;
    } else { 
      analogWrite(MOTOR_2_FWD_PIN,i);
      delay(75);
    }
  }
  //ramp up and down motor 2 reverse speed
  for (int i=MOTOR_MIN;i<256;i++){
    analogWrite(MOTOR_2_RV_PIN,i);
    delay(75);
  }
  for (int i=255;i>=0;i--){

    if (i < MOTOR_MIN){
      analogWrite(MOTOR_2_RV_PIN,0);
      break;
    } else {
      analogWrite(MOTOR_2_RV_PIN,i);
      delay(75);
    }
  }

  return;
     
}




/*
 * Initializes bluetooth
 */
int BLEinit(){
  if (!BLE.begin()){
    Serial.println("An error occured when starting BLE");
    return 1;
  }


   BLE.setDeviceName(CENTRAL_NAME);
   BLE.setLocalName(CENTRAL_NAME);
   


  if (!BLE.scanForUuid(CONTROLLER_UUID)){
    Serial.println("An error occured when scanning");
    return 1;
  }

  

  return 0;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  analogWrite(MOTOR_1_RV_PIN,0);
  analogWrite(MOTOR_2_RV_PIN,0);
  analogWrite(MOTOR_1_FWD_PIN,0);
  analogWrite(MOTOR_2_FWD_PIN,0);

  BLEinit();
  Scheduler.startLoop(BLEconnection);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
}