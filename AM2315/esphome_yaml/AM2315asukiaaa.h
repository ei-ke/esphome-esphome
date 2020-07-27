#include "esphome.h"
#include "AM2320_asukiaaa.h"

class AM2315 : public PollingComponent, public Sensor {
 public:
  AM2320_asukiaaa mySensor;

  Sensor *temperature = new Sensor();
  Sensor *humidity = new Sensor();

  AM2315() : PollingComponent (5000) {}

  void setup() override {
    // Initialize the device here. Usually Wire.begin() will be called in here,
    // though that call is unnecessary if you have an 'i2c:' entry in your config

    Wire.begin();
    mySensor.setWire(&Wire);
  }
  void update() override {
    if (mySensor.update() == 0) {
      float temp = mySensor.temperatureC;
      temperature->publish_state(temp);
      float humid = mySensor.humidity;
      humidity->publish_state(humid);
    }
  }
};
