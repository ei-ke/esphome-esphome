// Implementation based on:
//  - ESPEasy: https://github.com/letscontrolit/ESPEasy/blob/mega/src/_P034_DHT12.ino
//  - DHT12_sensor_library: https://github.com/xreef/DHT12_sensor_library/blob/master/DHT12.cpp
//  - Arduino - AM2320: https://github.com/EngDial/AM2320/blob/master/src/AM2320.cpp

#include "am2320.h"
#include "esphome/core/log.h"

namespace esphome {
namespace am2320 {

static const char *TAG = "am2320";

float temperatureC;
float humidity;
float temperatureF;

uint16_t CRC16(uint8_t *ptr, uint8_t length) {
  uint16_t crc = 0xFFFF;
  uint8_t s = 0x00;

  while(length--) {
    crc ^= *ptr++;
    for(s = 0; s < 8; s++) {
      if((crc & 0x01) != 0) {
        crc >>= 1;
        crc ^= 0xA001;
      } else crc >>= 1;
    }
  }
  return crc;
}

void AM2320Component::update() {
  delay(2000);
static const int buffLen = 8;
  uint8_t buf[buffLen];

  // Wakeup sensor
  this->parent_->raw_begin_transmission(this->address_);
  //this->parent_->raw_end_transmission(this->address_);
  delay(10);
  uint8_t data1[1] = {0x03};
  uint8_t data2[1] = {0x00};
  uint8_t data3[1] = {0x04};
  int leng = 1;
  this->parent_->raw_begin_transmission(this->address_);
  this->parent_->raw_write(this->address_, data1, leng);
  this->parent_->raw_write(this->address_, data2, leng);
  this->parent_->raw_write(this->address_, data3, leng);

  int result = this->parent_->raw_end_transmission(this->address_);
  delay(2); // >1.5ms
  for (uint8_t i = 0; i < buffLen; ++i) {
    buf[i] = this->parent_->raw_request_from(this->address_, leng);
  }


  // Fusion Code check

  // CRC check


  uint16_t t = (((uint16_t) buf[4] & 0x7F) << 8) | buf[5];
  temperatureC = t / 10.0;
  temperatureC = ((buf[4] & 0x80) >> 7) == 1 ? temperatureC * (-1) : temperatureC;
  temperatureF = temperatureC * 9 / 5 + 32;

  uint16_t h = ((uint16_t) buf[2] << 8) | buf[3];
  humidity = h / 10.0;

  ESP_LOGD(TAG, "Got temperature=%.1f°C humidity=%.1f%%", temperatureC, humidity);
  this->temperature_sensor_->publish_state(temperatureC);
  this->humidity_sensor_->publish_state(humidity);
}
void AM2320Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up AM2320...");
}

float AM2320Component::get_setup_priority() const { return setup_priority::DATA; }


}  // namespace am2320
}  // namespace esphome
