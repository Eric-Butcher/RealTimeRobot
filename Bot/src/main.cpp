#include <Scheduler.h>
#include <ArduinoBLE.h>


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

bool SAFETY = false;

int MOTOR_1_FWD_PIN = 7;
int MOTOR_1_RV_PIN = 6;

int MOTOR_2_FWD_PIN = 4;
int MOTOR_2_RV_PIN = 5;


char UUID[] = "ENTER UUID HERE";
char peripheral_name[] = "ENTER NAME HERE"; 

//the indexes for motor 1 and 2 characteristics
const int characteristicIndex1 = 0;
const int characteristicIndex2 = 1;


/*
 * Takes drive information, and handles motor control with it. 
 */
void motor_Driver(){

  
  while (SAFETY){
    

    //motor 1 drive
    if (MOTOR_1_DRIVE < 0){
      analogWrite(MOTOR_1_FWD_PIN,0);
      analogWrite(MOTOR_1_RV_PIN,abs(MOTOR_1_DRIVE));
    } else {
      analogWrite(MOTOR_1_RV_PIN,0);
      analogWrite(MOTOR_1_FWD_PIN,MOTOR_1_DRIVE);
    }

    //motor 2 drive
    if (MOTOR_2_DRIVE < 0){
      analogWrite(MOTOR_2_FWD_PIN,0);
      analogWrite(MOTOR_2_RV_PIN,abs(MOTOR_2_DRIVE));
    } else {
      analogWrite(MOTOR_2_RV_PIN,0);
      analogWrite(MOTOR_2_FWD_PIN,MOTOR_2_DRIVE);
    }
  
    
  }
  //stop all motor movement if safety fails
  analogWrite(MOTOR_1_FWD_PIN,0); 
  analogWrite(MOTOR_1_RV_PIN,0);
  analogWrite(MOTOR_2_RV_PIN,0);
  analogWrite(MOTOR_2_FWD_PIN,0);
  
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

  //discover peripheral attributes
  if (peripheral.discoverAttributes()){
     Serial.println("Found attributes of peripheral device!");
  } else {
    peripheral.disconnect();
    Serial.println("Was not able to find attributes of device. Disconnecting...");
    return;
  }

  //the service provided by the peripheral (controller) will have two characteristics, one for each motor
  BLECharacteristic throttleCharacteristic1 = peripheral.characteristic(UUID,characteristicIndex1);
  BLECharacteristic throttleCharacteristic2 = peripheral.characteristic(UUID,characteristicIndex2);

  if (!throttleCharacteristic1 || !throttleCharacteristic2){
    peripheral.disconnect();
    Serial.println("Was not able to find throttle characteristic. Disconnecting...");
    return;
  } else if (!throttleCharacteristic1.canRead() || !throttleCharacteristic2.canRead()){
    peripheral.disconnect(); 
    Serial.println("Throttle characteristic is not readable. Disconnecting... ");
  }

  uint16_t throttle1 = 0;
  uint16_t throttle2 = 0;
  
  //loop while connected
  while (peripheral.connected()){
    //allow movement while connected
    SAFETY = true;
  
    throttleCharacteristic1.readValue(throttle1);

    throttleCharacteristic2.readValue(throttle2);

    //process data if need be

    MOTOR_1_DRIVE = throttle1;

    MOTOR_2_DRIVE = throttle2;
    
  }

  //stop movement if disconnected
  SAFETY = false;

  Serial.println("Peripheral Disconnected");
  return;
  
}


/*
 * Handles the bluetooth connection after intialization
 */
void BLEconnection(){

  //loop
  while(true){

    BLEDevice peripheral = BLE.available();

    //if found peripheral
    if (peripheral){
       // discovered a peripheral, print out address, local name, and advertised service

      Serial.print("Found ");
      Serial.print(peripheral.address());
      Serial.print(" '");
      Serial.print(peripheral.localName());
      Serial.print("' ");
      Serial.print(peripheral.advertisedServiceUuid());
      Serial.println();

      if (peripheral.localName() != peripheral_name) {
        return;
      }

      BLE.stopScan();

      controlled(peripheral);
      

      //if disconnected from peripheral, start scanning again
      BLE.scanForUuid(UUID);
      
    }
    
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


  if (!BLE.scanForUuid(UUID)){
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

  delay(2000);

  //analogWrite(MOTOR_1_RV_PIN,60);
  //use scheduler to run bluetooth config here?
  testMotors();
}

void loop() {
  // put your main code here, to run repeatedly:

  //put function to change motor values here? 
}