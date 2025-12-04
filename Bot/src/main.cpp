#include <Scheduler.h>
#include <ArduinoBLE.h>
#include <array>
#include <Arduino.h>

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

//defines a deadzone area where input is ignored
int DEAD_ZONE_POS = 2348;
int DEAD_ZONE_NEG = 1748;


//motor 1 is right motor, motor 2 left
int MOTOR_1_FWD_PIN = 6;
int MOTOR_1_RV_PIN = 7;

int MOTOR_2_FWD_PIN = 5;
int MOTOR_2_RV_PIN = 4;

int BLUETOOTH_STATUS_PIN = 36;
int POWER_STATUS_PIN = 34;



static const constexpr char *CONTROLLER_UUID = "547b5676-0377-480f-b6f8-2a94873c07ec";
static const constexpr char *L_THUMB_STICK_X_AXIS_UUID = "5b03b0ef-c8db-4ef0-adf6-09e23d41a68d";
static const constexpr char *L_THUMB_STICK_Y_AXIS_UUID = "b1169c28-5e12-4213-a4ff-0aa316f233cc";
static const constexpr char *L_THUMB_STICK_BUTTON_UUID = "c2dd566c-65bb-4d28-8708-227c923433cf";

static const constexpr char *R_THUMB_STICK_X_AXIS_UUID = "37aa7d87-3161-4b8a-8f84-afddaf53ced3";
static const constexpr char *R_THUMB_STICK_Y_AXIS_UUID = "88d80b83-dcf1-47cf-b7dd-6b8c6d01b072";
static const constexpr char *R_THUMB_STICK_BUTTON_UUID = "c817eaef-8d21-4df8-a7d7-a4e27c63b390";

static const constexpr char *YELLOW_BUTTON_UUID = "b273ac1b-e05f-4e47-a508-6c87d89e46eb";
static const constexpr char *RED_BUTTON_UUID = "6db0dbd7-8830-4e3d-b885-baf0e4c75d93";
static const constexpr char *GREEN_BUTTON_UUID = "ec339d10-06c3-4ad0-80dc-0066f0fea2b9";
static const constexpr char *BLUE_BUTTON_UUID = "473abf6f-2591-427d-9ae9-9546d01e2287";
static const constexpr char *CENTRAL_NAME = "DUCKS_Central";

//the indexes for motor 1 and 2 characteristics
const int characteristicIndex1 = 0;
const int characteristicIndex2 = 1;

static const constexpr int NUMBER_OF_CHARACTERISTICS = 10;

typedef enum Result {
  SUCCESSFULL,
  FAILURE
};

/*
 * Forward right
 * Forward left,
 * Forward,
 * Reverse right
 * Reverse left,
 * Reverse,
 * stationary
 */
typedef enum Direction {
  FR,
  FL,
  FA,
  RR,
  RL,
  RA,
  S
};


struct ControllerState {
  int L_thumb_stick_x_axis;
  int L_thumb_stick_y_axis;
  byte L_thumb_stick_button;
  int R_thumb_stick_x_axis;
  int R_thumb_stick_y_axis;
  byte R_thumb_stick_button;
  byte yellow_button;
  byte red_button;
  byte green_button;
  byte blue_button;
};

/*
 * Stops all motor movement 
 */
void STOP(){
  analogWrite(MOTOR_1_RV_PIN,0);
  analogWrite(MOTOR_1_FWD_PIN,0);
  analogWrite(MOTOR_2_RV_PIN,0);
  analogWrite(MOTOR_2_FWD_PIN,0);
  return;
}

/*
 * Stops reverse drive 
 */
void STOP_RV(){
  analogWrite(MOTOR_1_RV_PIN,0);
  analogWrite(MOTOR_2_RV_PIN,0);
  return;
}

/*
 * Stops forward drive 
 */
void STOP_FWD(){
  analogWrite(MOTOR_1_FWD_PIN,0);
  analogWrite(MOTOR_2_FWD_PIN,0);
  return;
}

//gets current direction of robot based on controller input data
Direction get_direction(ControllerState *state){
  if (state->L_thumb_stick_y_axis > DEAD_ZONE_POS){
       if (state->L_thumb_stick_x_axis > DEAD_ZONE_POS){
        //moving forward and to right
        return FR;
       } else if (state->L_thumb_stick_x_axis < DEAD_ZONE_NEG){
        //moving forward and to left
        return FL;
       }else {
        //moving directly forward
        return FA;
       }
    } else if (state->L_thumb_stick_y_axis < DEAD_ZONE_NEG){
      if (state->L_thumb_stick_x_axis > DEAD_ZONE_POS){
        //moving forward and to right
        return RR;
       } else if (state->L_thumb_stick_x_axis < DEAD_ZONE_NEG){
        //moving forward and to left
        return RL;
       }else {
        //moving directly forward
        return RA;
       }
    } else {
      return S;
    }
    return S;
}

/*
 * Takes the input data and converts it to usable drive values
 * 
 * Returns converted value
 * 
 * Takes raw input value, and if value is for reverse direction
 */
int converter(int input,bool reverse){
  //convert new value based on if input is in fwd or negative direction
  //equation for conversion:
  //outputStart + ((outputEnd - outputStart) / (inputEnd - inputStart)) * (input - inputStart)
  if (reverse){
    return (70+((float)(255-70) / (float)(0-DEAD_ZONE_NEG)) * (input-DEAD_ZONE_NEG));
  } else {
    return (70+((float)(255-70) / (float)(4096-DEAD_ZONE_POS)) * (input-DEAD_ZONE_POS));
    //(70+((255-70) / (4096-DEAD_ZONE_POS)) * (input-DEAD_ZONE_POS))
    
  }


  return 0;
  
}




/*
 * Takes drive information, and handles motor control with it. 
 */
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
 * interprets joystick data for one stick operation.
 * 
 * sets motor drive accordingly
 */
void one_stick_operation(ControllerState *controller_state){
  Direction current_direction = get_direction(controller_state);
  //just skip if not being moved
  if (current_direction == S){
    STOP();
    return;
  }

  int MOTOR_1_DRIVE=0;
  int MOTOR_2_DRIVE=0;
  switch (current_direction){
    case (FA):
      STOP_RV();
      MOTOR_1_DRIVE = converter(controller_state->L_thumb_stick_y_axis,false);
      MOTOR_2_DRIVE = MOTOR_1_DRIVE;
      analogWrite(MOTOR_1_FWD_PIN,MOTOR_1_DRIVE);
      analogWrite(MOTOR_2_FWD_PIN,MOTOR_2_DRIVE);
      break;
    case (FR):
      STOP_RV();
      MOTOR_2_DRIVE = converter(controller_state->L_thumb_stick_y_axis,false);
      MOTOR_1_DRIVE = MOTOR_2_DRIVE - converter(controller_state->L_thumb_stick_x_axis,false);
      analogWrite(MOTOR_1_FWD_PIN,MOTOR_1_DRIVE);
      analogWrite(MOTOR_2_FWD_PIN,MOTOR_2_DRIVE);
      break;           
    case (FL):
      STOP_RV();
      MOTOR_1_DRIVE = converter(controller_state->L_thumb_stick_y_axis,false);
      MOTOR_2_DRIVE = MOTOR_1_DRIVE - converter(controller_state->L_thumb_stick_x_axis,true);
      analogWrite(MOTOR_1_FWD_PIN,MOTOR_1_DRIVE);
      analogWrite(MOTOR_2_FWD_PIN,MOTOR_2_DRIVE);
      break;
    case (RA):
      STOP_FWD();
      MOTOR_1_DRIVE = converter(controller_state->L_thumb_stick_y_axis,true);
      MOTOR_2_DRIVE = MOTOR_1_DRIVE;
      analogWrite(MOTOR_1_RV_PIN,MOTOR_1_DRIVE);
      analogWrite(MOTOR_2_RV_PIN,MOTOR_2_DRIVE);
      break;
    case (RR):
      STOP_FWD();
      MOTOR_2_DRIVE = converter(controller_state->L_thumb_stick_y_axis,true);
      MOTOR_1_DRIVE = MOTOR_2_DRIVE - converter(controller_state->L_thumb_stick_x_axis,false);
      analogWrite(MOTOR_1_RV_PIN,MOTOR_1_DRIVE);
      analogWrite(MOTOR_2_RV_PIN,MOTOR_2_DRIVE);
      break;
    case (RL):
      STOP_FWD();
      MOTOR_1_DRIVE = converter(controller_state->L_thumb_stick_y_axis,true);
      MOTOR_2_DRIVE = MOTOR_1_DRIVE - converter(controller_state->L_thumb_stick_x_axis,true);
      analogWrite(MOTOR_1_RV_PIN,MOTOR_1_DRIVE);
      analogWrite(MOTOR_2_RV_PIN,MOTOR_2_DRIVE);
    
  }
  
}



/*
 * Interprets joystick data for two stick operation. 
 */
void two_stick_operation(ControllerState *controller_state){


  int MOTOR_1_DRIVE = 0;
  int MOTOR_2_DRIVE = 0;
  if (controller_state->R_thumb_stick_y_axis > DEAD_ZONE_POS){
    //left stick is moving up
    analogWrite(MOTOR_2_RV_PIN,0);
    MOTOR_2_DRIVE = converter(controller_state->R_thumb_stick_y_axis,false);
    analogWrite(MOTOR_2_FWD_PIN,MOTOR_2_DRIVE);
    Serial.println("LEFT STICK MOVING UP : DRIVEN AT: " + String(MOTOR_2_DRIVE));
  } else if (controller_state->R_thumb_stick_y_axis < DEAD_ZONE_NEG){
    //left stick moving down
    analogWrite(MOTOR_2_FWD_PIN,0);
    MOTOR_2_DRIVE = converter(controller_state->R_thumb_stick_y_axis,true);
    analogWrite(MOTOR_2_RV_PIN,MOTOR_2_DRIVE);
    Serial.println("LEFT STICK MOVING DOWN: DRIVEN AT: " + String(MOTOR_2_DRIVE));
  } else {
    //left stick not moving
    analogWrite(MOTOR_2_FWD_PIN,0);
    analogWrite(MOTOR_2_RV_PIN,0);
  }

  if (controller_state->L_thumb_stick_y_axis > DEAD_ZONE_POS){
    //right stick is moving up
    analogWrite(MOTOR_1_RV_PIN,0);
    MOTOR_1_DRIVE = converter(controller_state->L_thumb_stick_y_axis,false);
    analogWrite(MOTOR_1_FWD_PIN,MOTOR_1_DRIVE);
    Serial.println("RIGHT STICK IS MOVING UP: DRIVEN AT: " + String(MOTOR_1_DRIVE));
  } else if (controller_state->L_thumb_stick_y_axis < DEAD_ZONE_NEG){
    //right stick moving down
    analogWrite(MOTOR_1_FWD_PIN,0);
    MOTOR_1_DRIVE = converter(controller_state->L_thumb_stick_y_axis,true);
    analogWrite(MOTOR_1_RV_PIN,MOTOR_1_DRIVE);
    Serial.println("RIGHT STICK IS MOVING DOWN: DRIVEN AT: " + String(MOTOR_1_DRIVE));
  } else {
    //right stick not moving
    analogWrite(MOTOR_1_FWD_PIN,0);
    analogWrite(MOTOR_1_RV_PIN,0);
  }

  return;
  
}



/*
 * Maintains connection to bluetooth peripheral, 
 * and retrieves throttle information. Will continuously update readings
 * as long as connection holds. 
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

  //NEW ONES FOR NEW STICK
  BLECharacteristic R_thumb_stick_x_axis_characteristic = peripheral.characteristic(R_THUMB_STICK_X_AXIS_UUID);
  BLECharacteristic R_thumb_stick_y_axis_characteristic = peripheral.characteristic(R_THUMB_STICK_Y_AXIS_UUID);
  BLECharacteristic R_thumb_stick_button_characteristic = peripheral.characteristic(R_THUMB_STICK_BUTTON_UUID);
  
  BLECharacteristic L_thumb_stick_x_axis_characteristic = peripheral.characteristic(L_THUMB_STICK_X_AXIS_UUID);
  BLECharacteristic L_thumb_stick_y_axis_characteristic = peripheral.characteristic(L_THUMB_STICK_Y_AXIS_UUID);
  BLECharacteristic L_thumb_stick_button_characteristic = peripheral.characteristic(L_THUMB_STICK_BUTTON_UUID);
  BLECharacteristic yellow_button_characteristic = peripheral.characteristic(YELLOW_BUTTON_UUID);
  BLECharacteristic red_button_characteristic = peripheral.characteristic(RED_BUTTON_UUID);
  BLECharacteristic green_button_characteristic = peripheral.characteristic(GREEN_BUTTON_UUID);
  BLECharacteristic blue_button_characteristic = peripheral.characteristic(BLUE_BUTTON_UUID);

  std::array<BLECharacteristic, NUMBER_OF_CHARACTERISTICS> characteristics = {
        L_thumb_stick_x_axis_characteristic,
        L_thumb_stick_y_axis_characteristic,
        L_thumb_stick_button_characteristic,
        R_thumb_stick_x_axis_characteristic,
        R_thumb_stick_y_axis_characteristic,
        R_thumb_stick_button_characteristic,
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

  digitalWrite(BLUETOOTH_STATUS_PIN,HIGH);
  //loop while connected
  while (peripheral.connected()){

    if (L_thumb_stick_x_axis_characteristic.valueUpdated()){
        L_thumb_stick_x_axis_characteristic.readValue(&controller_state.L_thumb_stick_x_axis, sizeof(int));
    } 
    if (L_thumb_stick_y_axis_characteristic.valueUpdated()){
        L_thumb_stick_y_axis_characteristic.readValue(&controller_state.L_thumb_stick_y_axis, sizeof(int));
    } 
    if (L_thumb_stick_button_characteristic.valueUpdated()){
        L_thumb_stick_button_characteristic.readValue(&controller_state.L_thumb_stick_button, sizeof(byte));
    } 
    //CHARACTERISTICS FOR NEW STICK!!!
    if (R_thumb_stick_x_axis_characteristic.valueUpdated()){
        R_thumb_stick_x_axis_characteristic.readValue(&controller_state.R_thumb_stick_x_axis, sizeof(int));
    } 
    if (R_thumb_stick_y_axis_characteristic.valueUpdated()){
        R_thumb_stick_y_axis_characteristic.readValue(&controller_state.R_thumb_stick_y_axis, sizeof(int));
    } 
    if (R_thumb_stick_button_characteristic.valueUpdated()){
        R_thumb_stick_button_characteristic.readValue(&controller_state.R_thumb_stick_button, sizeof(byte));
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

    Serial.println("The x_axis is: " + String(controller_state.L_thumb_stick_x_axis));
    Serial.println("The y_axis is: " + String(controller_state.L_thumb_stick_y_axis));
    Serial.println("The thumb_stick_button is: " + String(controller_state.L_thumb_stick_button));
    Serial.println("The yellow_button is: " + String(controller_state.yellow_button));
    Serial.println("The red_button is: " + String(controller_state.red_button));
    Serial.println("The green_button is: " + String(controller_state.green_button));
    Serial.println("The blue_button is: " + String(controller_state.blue_button));
    
    Serial.println("The x_axis is: " + String(controller_state.R_thumb_stick_x_axis));
    Serial.println("The y_axis is: " + String(controller_state.R_thumb_stick_y_axis));
    Serial.println("The thumb_stick_button is: " + String(controller_state.R_thumb_stick_button));
    Serial.println();
    
    /*
     * DO ANY DATA PROCESSING HERE!!!!!!!!!!!!!!!!! 
     */

   //one_stick_operation(&controller_state);  
   two_stick_operation(&controller_state);
   
    
  }

  //stop movement if disconnected
  digitalWrite(BLUETOOTH_STATUS_PIN,LOW);
  STOP();
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

  pinMode(BLUETOOTH_STATUS_PIN,OUTPUT);
  pinMode(POWER_STATUS_PIN,OUTPUT);

  digitalWrite(POWER_STATUS_PIN,HIGH);
  digitalWrite(BLUETOOTH_STATUS_PIN,LOW);

  BLEinit();
  Scheduler.startLoop(BLEconnection);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
}