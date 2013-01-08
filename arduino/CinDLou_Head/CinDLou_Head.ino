
/* CinDLou Head Controller Sketch */

/* HW Setup using PWM Shield

    http://alex.kathack.com/codes/tlc5940arduino/html_r014/tlc__servos_8h.html
    
    To connect a servo:
     1. Put a 2k-5k pull-up resistor (R0 below; I've tried with 3.3k) between the
          servo control output pin and +5v.
     2. Connect that same pin to the servo's control line like so

        servo data pin
              |     _____
     OUTn ----+----[_____]---+5v
                      R0

    Steve Pomeroy <steve ~AT~ staticfree.info>, 2009-01-20 */

/* Serial Protocol */
/*
  HNR<#>  - Head Neck Rotation # = Degree (0-180)
  HNL<#>  - Head Neck Lift # = Degree (0-180)
*/

#include "Tlc5940.h"
#include "tlc_servos.h"

#define IDLE_COMMAND               0
#define HEAD_COMMAND               1
#define HEAD_NECK_COMMAND          2
#define HEAD_NECK_ROTATE_COMMAND   3
#define HEAD_NECK_LIFT_COMMAND     4

#define NECK_ROTATION_CHANNEL   0
#define NECK_LIFT_CHANNEL       1

int proto_state = IDLE_COMMAND; // State of incoming API

int neck_rotation_degree = -1;
int neck_lift_degree = -1;

int current_neck_rotation_degree = -1;
int current_neck_lift_degree = -1;

void setup()
{
  tlc_initServos();  // Note: this will drop the PWM freqency down to 50Hz.
  Serial.begin(9600);
}

// Handle the Serial API commands
void handleAPI()
{
  boolean valid = false;
  if(proto_state == IDLE_COMMAND) {
    if(Serial.read() == 'H') {
     proto_state = HEAD_COMMAND;
     valid = true;
    }
  } else if(proto_state == HEAD_COMMAND) {
     if(Serial.read() == 'N') {
       proto_state = HEAD_NECK_COMMAND;
       valid = true;
     }
  } else if(proto_state == HEAD_NECK_COMMAND) {
    if(Serial.read() == 'R') {
      proto_state = HEAD_NECK_ROTATE_COMMAND;
      valid = true;
    } else if(Serial.read() == 'L') {
      proto_state = HEAD_NECK_LIFT_COMMAND;
      valid = true;
    }
  } else if(proto_state == HEAD_NECK_ROTATE_COMMAND) {
    neck_rotation_degree = Serial.parseInt();
    if(Serial.read() == '\n') {
     valid = true;
    } 
  } else if(proto_state == HEAD_NECK_LIFT_COMMAND) {
    neck_lift_degree = Serial.parseInt();
    if(Serial.read() == '\n') {
     valid = true;
    } 
  }
  
  if(!valid) {
    proto_state = IDLE_COMMAND;
  }
}

void do_neck_rotation()
{
  // If desired rotation is equal to current this should not be called
  if(neck_rotation_degree > current_neck_rotation_degree) {
    current_neck_rotation_degree += 1;
    tlc_setServo(NECK_ROTATION_CHANNEL, current_neck_rotation_degree);
    Tlc.update();
  } else {
    current_neck_rotation_degree -= 1;
    tlc_setServo(NECK_ROTATION_CHANNEL, current_neck_rotation_degree);
    Tlc.update();    
  }
}

void do_neck_lift()
{
  // If desired lift is equal to current this should not be called
  if(neck_lift_degree > current_neck_lift_degree) {
    current_neck_lift_degree += 1;
    tlc_setServo(NECK_LIFT_CHANNEL, current_neck_lift_degree);
    Tlc.update();
  } else {
    current_neck_lift_degree -= 1;
    tlc_setServo(NECK_LIFT_CHANNEL, current_neck_lift_degree);
    Tlc.update();    
  }
}

void loop()
{
  if (Serial.available() > 0) {
     handleAPI();
  }
  if(current_neck_rotation_degree != neck_rotation_degree) {
     do_neck_rotation(); 
  }
  if(current_neck_lift_degree != neck_lift_degree) {
     do_neck_lift();
  }
  
}

