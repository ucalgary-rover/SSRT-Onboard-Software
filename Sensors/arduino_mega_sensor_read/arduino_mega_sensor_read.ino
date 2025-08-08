#include <MQUnifiedsensor.h>
#include <SoftwareSerial.h>

// general settings
#define BOARD "Arduino UNO"
#define VOLTAGE_RESOLUTION 5
#define ADC_BIT_RESOLUTION 10

// enable/disale specific sensors (0 is disabled, 1 is enabled)
#define H2_1_ENABLED 1
#define H2_2_ENABLED 1
#define OZONE_ENABLED 1
#define GEIGER_ENABLED 1

// H2 sensor specific settings
#define H2_SENSOR_TYPE "MQ-8"
#define H2_CLEAN_AIR_RATIO 70.0
#define RL_VALUE 10.0 // Load resistor in kΩ

#if H2_1_ENABLED
#define H2_SENSOR_PIN_1 A0
MQUnifiedsensor h2Sensor1(BOARD, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION, H2_SENSOR_PIN_1, H2_SENSOR_TYPE);
#endif

#if H2_2_ENABLED
#define H2_SENSOR_PIN_2 A2
MQUnifiedsensor h2Sensor2(BOARD, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION, H2_SENSOR_PIN_2, H2_SENSOR_TYPE);
#endif

// Ozone sensor specific settings
#if OZONE_ENABLED
#define OZONE_SENSOR_PIN A1
#define OZONE_SENSOR_TYPE "MQ-131"
MQUnifiedsensor ozoneSensor(BOARD, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION, OZONE_SENSOR_PIN, OZONE_SENSOR_TYPE);
#endif

// geiger counter settings
#if GIEGER_ENABLED
#define SERIAL_RX 4
#define SERIAL_TX 5
#define BAUD_RATE 115200
SoftwareSerial geigerIn(SERIAL_RX, SERIAL_TX) l
#endif

    void initH2Sensor(MQUnifiedsensor h2Sensor)
{
  h2Sensor.setRegressionMethod(1); // Exponential/log-log regression
  h2Sensor.setA(-1.458);           // Curve slope (from H₂ sensitivity graph) SHOULD BE CHANGED AFTER CALCULATIONS!!!
  h2Sensor.setB(4.309);            // Curve intercept SHOULD BE CHANGED AFTER CALCULATIONS!!!

  h2Sensor.setRL(RL_VALUE);
  h2Sensor.init();
}

void setup()
{
  Serial.begin(115200);

  // intialize hydrogen sensors
  if (H2_1_ENABLED)
  {
    initH2Sensor(h2Sensor1);
  }
  if (H2_2_ENABLED)
  {
    initH2Sensor(h2Sensor2);
  }

  // initialize ozone sensor
  if (OZONE_ENABLED)
  {
    ozoneSensor.init();
    // TODO: Set A, B, and RL levels for ozone
  }

  Serial.println("Calibrating...");
  float H2R01 = 0, H2R02 = 0, ozoneR0 = 0;
  for (int i = 1; i <= 10; i++)
  {
    if (H2_1_ENABLED)
    {
      h2Sensor1.update();
      H2R01 += h2Sensor1.calibrate(H2_CLEAN_AIR_RATIO);
    }

    if (H2_2_ENABLED)
    {
      h2Sensor2.update();
      H2R02 += h2Sensor2.calibrate(H2_CLEAN_AIR_RATIO);
    }

    if (OZONE_ENABLED)
    {
      ozoneSensor.update();
      ozoneR0 += ozoneSensor.calibrate(5);
    }

    Serial.print(".");
    delay(500);
  }
  H2R01 /= 10.0;
  H2R02 /= 10.0;

  if (H2_1_ENABLED)
  {
    h2Sensor1.setR0(H2R01);
    h2Sensor1.serialDebug(true);
  }

  if (H2_2_ENABLED)
  {
    h2Sensor2.setR0(H2R02);
    h2Sensor2.serialDebug(true);
  }

  if (OZONE_ENABLED)
  {
    ozoneSensor.setR0(ozoneR0);
    ozoneSensor.serialDebug(true);
  }

  if (GEIGER_ENABLED)
  {
    geigerIn.begin(BAUD_RATE);
  }

  Serial.println("done!");
  Serial.println(H2R01);
}

void loop()
{
  double h2Sensor1Ratio = 0.0, h2Sensor2Ratio = 0.0, ozoneSensorRatio = 0.0, geigerReading = 0.0;
  if (H2_1_ENABLED)
  {
    h2Sensor1.udpate();
    h2Sensor1.readSensor();
    h2Sensor1Ratio = h2Sensor1.getRS() / h2Sensor1.getR0();
  }

  if (H2_2_ENABLED)
  {
    h2Sensor2.udpate();
    h2Sensor2.readSensor();
    h2Sensor2Ratio = h2Sensor2.getRS() / h2Sensor2.getR0();
  }

  if (OZONE_ENABLED)
  {
    ozoneSensor.udpate();
    ozoneSensor.readSensor();
    ozoneSensorRatio = ozoneSensor.getRS() / ozoneSensor.getR0();
  }

  if (GEIGER_ENABLED && geigerIn.available())
  {
    // read line
    String s = "";
    while (geigerIn.available())
    {
      char c = geigerIn.read();
      if (c == '\n')
        break;
      s += c;
    }

    // Convert 2-digit hex string to float
    int value = strtol(s.c_str(), NULL, 16);
    geigerReading = (float)value;
  }

  Serial.println(String(h2Sensor1Ratio) + "," + String(h2Sensor2Ratio) + ","  +  String(ozoneSensorRatio) + "," + String(geigerReading));

  delay(10);
}