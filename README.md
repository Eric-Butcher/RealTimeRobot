# D.U.C.K.S Robot

The Drivable User-Controlled Kinematics System (DUCKS) is a robot that can be controlled in real time using a controller using Bluetooth Low Energy!

## Controller

The controller was designed using an Arduino Nano ESP32, two joysticks, and four push buttons.Once the controller pairs with the host device, it will automatically send data packets to the controller every time new inputs are detected over Bluetooth Low Energy. 

![The breadboard schematic of the controller.](./images/DUCKS_Controller_Schematic.png)

![The actual controller on a breadboard.](./images/DUCKS_Controller.webp)

## Robot

The robot has an Arduino Giga on board as well as two TT gearbox motors and an H-bridge circuit. The microcontroller receives commands from th controller and controls the motors accordingly to have the robot move through space. 

![The front of the DUCKS robot.](./images/DUCKS_Robot.webp)

![The behind of the DUCKS robot.](./images/DUCKS_Robot_Behind.webp)



