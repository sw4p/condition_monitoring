#include <ArduinoMotorCarrier.h>
#define INTERRUPT_PIN 6

//Variable to store the battery voltage
static int batteryVoltage;

//Variable to change the motor speed and direction
static int duty = 0;

static float batteryThreshold = 8.0;
static int timeOutDuration_ms = 1;

void setup() 
{
  //Serial port initialization
  Serial.begin(115200);
  Serial.setTimeout(timeOutDuration_ms);
  while (!Serial);

  //Establishing the communication with the Motor Carrier
  if (controller.begin()) 
    {
      Serial.print("MKR Motor Connected connected, firmware version ");
      Serial.println(controller.getFWVersion());
    } 
  else 
    {
      Serial.println("Couldn't connect! Is the red LED blinking? You may need to update the firmware with FWUpdater sketch");
      while (1);
    }

  // Reboot the motor controller; brings every value back to default
  Serial.println("reboot");
  controller.reboot();
  delay(500);
  
  //Take the battery status
  float batteryVoltage = (float)battery.getConverted();
  Serial.print("Battery voltage: ");
  Serial.print(batteryVoltage);
  Serial.print("V, Raw ");
  Serial.println(battery.getRaw());
}


void loop()
{
  //batteryCheck();
  parseInput();
  
  //Keep active the communication between MKR board & MKR Motor Carrier
  //Ping the SAMD11
  controller.ping();
  //wait
  delay(50);
}

/**
 * Parse the input from the user
 *
 * MOTORON - turn on motor
 * MOTOROFF - turn off motor
 * MOTORSPEED <-100 to 100> - set duty cycle of the motor
 */
void parseInput()
{
  const String motor_on = "MOTORON";
  const String motor_off = "MOTOROFF";
  const String motor_speed = "MOTORSPEED";
  
  String str = Serial.readString();
  str.trim();

  // Make sense of input
  if (str.length() > 0)
  {
    if (str.equals(motor_on))
      motorCommand(100);
    else if (str.equals(motor_off))
      motorCommand(0);
    else if (str.substring(0, motor_speed.length()) == motor_speed)
    {
      String duty_cycle = str.substring(motor_speed.length()+1);
      motorCommand(duty_cycle.toInt());
      Serial.println(duty_cycle.toInt());
    }
    else
      Serial.print("Input Unknown - ");

    Serial.println(str);
  }
}

/**
 * If the battery is less than threshold value, this method will not return
 */
void batteryCheck()
{
  //Take the battery status
  float batteryVoltage = (float)battery.getConverted();
  
  //Reset to the default values if the battery level is lower than threshold
  if (batteryVoltage < batteryThreshold) 
  {
    Serial.println(" ");
    Serial.println("WARNING: LOW BATTERY");
    Serial.println("ALL SYSTEMS DOWN");
    M1.setDuty(0);

    while (batteryVoltage < batteryThreshold) 
    {
      batteryVoltage = (float)battery.getConverted();
    }
  }
}

/**
 * Set M1's duty cycle
 */
void motorCommand(int duty)
{
  M1.setDuty(duty);
}
