#include <MQUnifiedsensor.h>

// general settings
#define BOARD "Arduino UNO"
#define VOLTAGE_RESOLUTION 5
#define ADC_BIT_RESOLUTION 10

// H2 sensor specific settings
#define H2_SENSOR_PIN_1 A0
#define H2_SENSOR_PIN_2 A2
#define H2_SENSOR_TYPE "MQ-8"
#define H2_CLEAN_AIR_RATIO 70.0      
#define RL_VALUE 10.0             // Load resistor in kΩ

// Ozone sensor specific settings
#define OZONE_SENSOR_PIN A1
#define OZONE_SENSOR_TYPE "MQ-131"


MQUnifiedsensor h2Sensor1(BOARD, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION, H2_SENSOR_PIN_1, H2_SENSOR_TYPE);
MQUnifiedsensor h2Sensor2(BOARD, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION, H2_SENSOR_PIN_2, H2_SENSOR_TYPE);
MQUnifiedsensor ozoneSensor(BOARD, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION,  OZONE_SENSOR_PIN, OZONE_SENSOR_TYPE);


void initH2Sensor(MQUnifiedsensor h2Sensor) {
  h2Sensor.setRegressionMethod(1);       // Exponential/log-log regression
  h2Sensor.setA(-1.458);                 // Curve slope (from H₂ sensitivity graph) SHOULD BE CHANGED AFTER CALCULATIONS!!!
  h2Sensor.setB(4.309);                  // Curve intercept SHOULD BE CHANGED AFTER CALCULATIONS!!!

  h2Sensor.setRL(RL_VALUE);            
  h2Sensor.init();  
}

void setup() {
  Serial.begin(9600);
 
  // intialize hydrogen sensors
  initH2Sensor(h2Sensor1);                   
  initH2Sensor(h2Sensor2);                   

  // initialize ozone sensor
  ozoneSensor.init();
  // TODO: Set A, B, and RL levels for ozone

  Serial.println("Calibrating...");
  float H2R01 = 0, H2R02 = 0, ozoneR0 = 0;
  for (int i = 1; i <= 10; i++) {
    h2Sensor1.update();  
    H2R01 += h2Sensor1.calibrate(H2_CLEAN_AIR_RATIO);

    h2Sensor2.update();  
    H2R02 += h2Sensor2.calibrate(H2_CLEAN_AIR_RATIO);

    ozoneSensor.update();
    ozoneR0 += ozoneSensor.calibrate(5);

    Serial.print(".");
    delay(500);   
  }
  H2R01 /= 10.0;
  H2R02 /= 10.0;
  h2Sensor1.setR0(H2R01);  
  h2Sensor2.setR0(H2R02);  
  ozoneSensor.setR0(ozoneR0);
  

  Serial.println("done!");
  Serial.println(H2R01);
  h2Sensor1.serialDebug(true);
  h2Sensor2.serialDebug(true);
  ozoneSensor.serialDebug(true);
}

void loop() {
  h2Sensor1.update();
  h2Sensor2.update();
  ozoneSensor.update();

  h2Sensor1.readSensor();
  h2Sensor2.readSensor();
  ozoneSensor.readSensor();

  double h2Sensor1Ratio = h2Sensor1.getRS() / h2Sensor1.getR0();
  double h2Sensor2Ratio = h2Sensor2.getRS() / h2Sensor2.getR0();
  double ozoneSensorRatio = ozoneSensor.getRS() / ozoneSensor.getR0();

  Serial.println("First H2: " + String(h2Sensor1Ratio) +  ", Second H2: " + String(h2Sensor2Ratio) + ", Ozone: " + String(ozoneSensorRatio));


  // h2Sensor1.update();             
  // h2Sensor1.readSensor();        
  // h2Sensor1.serialDebug();  
  // ozoneSensor.update();             
  // ozoneSensor.readSensor();        
  // ozoneSensor.serialDebug();           
  delay(10);              
}