#include "pace_modbus.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/util.h"
#include "esphome/components/network/util.h"

namespace esphome {
namespace pace_modbus {

static const char *const TAG = "pace_modbus";

static const uint16_t MAX_RESPONSE_SIZE = 2048;//max size with data length is 0xFFF but no packet can reach that size, even with 0xFF (interrogate all packs) device id

void PaceModbus::setup() {
  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->setup();
  }
}

void PaceModbus::loop() {
  // if no network should we loop? 
  if (wait_network_ && (!network::is_connected() || !remote_is_connected())) {
    while (this->available()) {
      uint8_t byte;
      this->read_byte(&byte);
      this->rx_buffer_.clear();
    }

    delay(50);
    return;
  }

  const uint32_t now = millis();
  
  if (now < this->last_send_) {  // timer will go back to zero after some time
    this->last_send_ = now;
    this->next_send_ = now + this->update_interval_;
  }

  if (this->send_device_ == 0xFF) {  // first start
    for (auto *device : this->devices_) {
      if (device->address_ > this->last_device_) {
        this->last_device_ = device->address_;
      }
      if (this->send_device_ > device->address_) {
        this->send_device_ = device->address_;
        this->first_device_ = device->address_;
      }
    }
    this->next_send_ = now + this->rx_timeout_;
  } else if (this->next_send_ < now &&
             now - this->last_send_ > this->rx_timeout_) {  // we'll start on the next loop
    if (this->request_all_packs_) {
      auto *device = this->get_device(this->first_device_);
      uint8_t proto = 0x25;
      if (device != nullptr) {
        proto = device->get_protocol_version();
      }
      if (this->status_send_) {
        this->send(proto, 0xFF, 0x44, 0xFF);
        this->status_send_ = false;
        this->next_send_ = now + this->update_interval_; 
      } else {
        this->send(proto, 0xFF, 0x42, 0xFF);
        this->status_send_ = true;
      }
      this->last_send_ = now;
    } else {
      auto *device = this->get_device(send_device_);
      if (device != nullptr) {
        if (this->status_send_) {
          device->send(0x44, this->send_device_);  // status info
          this->status_send_ = false;
          ++this->send_device_;
          if (this->send_device_ > this->last_device_) {
            this->send_device_ = this->first_device_;
            this->next_send_ = now + this->update_interval_;  // interval restarts from here
          }
        } else {
          device->send(0x42, this->send_device_);  // telemetry info
          this->status_send_ = true;
        }
        this->last_send_ = now;
      } else {  // non-existant pack? try going forward and repeat on next cycle
        ++this->send_device_;
        if (this->send_device_ > this->last_device_) {
          this->send_device_ = this->first_device_;
          this->next_send_ = now + this->update_interval_;  // interval restarts from here
        }
      }
    }
  }

  if (now < this->last_pace_modbus_byte_) { // timer will go back to zero after some time
    this->last_pace_modbus_byte_ = now;
  }

  if (now - this->last_pace_modbus_byte_ > this->rx_timeout_) {
    ESP_LOGVV(TAG, "Buffer cleared due to timeout: %s",
              format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());
    this->rx_buffer_.clear();
    this->last_pace_modbus_byte_ = now;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_pace_modbus_byte_(byte)) {
      this->last_pace_modbus_byte_ = now;
    } else {
      ESP_LOGVV(TAG, "Buffer cleared due to reset: %s",
                format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());
      this->rx_buffer_.clear();
    }
  }
}

uint16_t chksum(const uint8_t data[], const uint16_t len) {
  uint16_t checksum = 0x00;
  for (uint16_t i = 0; i < len; i++) {
    checksum = checksum + data[i];
  }
  checksum = ~checksum;
  checksum += 1;
  return checksum;
}

uint16_t lchksum(const uint16_t len) {
  uint16_t lchecksum = 0x0000;

  if (len == 0)
    return 0x0000;

  lchecksum = (len & 0xf) + ((len >> 4) & 0xf) + ((len >> 8) & 0xf);
  lchecksum = ~(lchecksum % 16) + 1;

  return (lchecksum << 12) + len;  // 4 byte checksum + 12 bytes length
}

uint8_t ascii_hex_to_byte(char a, char b) {
  a = (a <= '9') ? a - '0' : (a & 0x7) + 9;
  b = (b <= '9') ? b - '0' : (b & 0x7) + 9;

  return (a << 4) + b;
}

static char byte_to_ascii_hex(uint8_t v) { return v >= 10 ? 'A' + (v - 10) : '0' + v; }
std::string byte_to_ascii_hex(const uint8_t *data, size_t length) {
  if (length == 0)
    return "";
  std::string ret;
  ret.resize(2 * length);
  for (size_t i = 0; i < length; i++) {
    ret[2 * i] = byte_to_ascii_hex((data[i] & 0xF0) >> 4);
    ret[2 * i + 1] = byte_to_ascii_hex(data[i] & 0x0F);
  }
  return ret;
}

bool PaceModbus::parse_pace_modbus_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];

  // Start of frame
  if (at == 0) {
    if (raw[0] != 0x7E) {
      ESP_LOGW(TAG, "Invalid header: 0x%02X", raw[0]);

      // return false to reset buffer
      return false;
    }

    return true;
  }

  // End of frame '\r'
  if (raw[at] != 0x0D)
    return true;

  if (at > MAX_RESPONSE_SIZE) {
    ESP_LOGW(TAG, "Maximum response size exceeded. Flushing RX buffer...");
    return false;
  }

  uint16_t data_len = at - 4 - 1;//remove checksum and end of frame
  uint16_t computed_crc = chksum(raw + 1, data_len);
  uint16_t remote_crc = uint16_t(ascii_hex_to_byte(raw[at - 4], raw[at - 3])) << 8 |
                        (uint16_t(ascii_hex_to_byte(raw[at - 2], raw[at - 1])) << 0);
  if (computed_crc != remote_crc) {
    ESP_LOGW(TAG, "CRC check failed! 0x%04X != 0x%04X", computed_crc, remote_crc);
    return false;
  }

  std::vector<uint8_t> data;
  for (uint16_t i = 1; i < data_len; i = i + 2) {
    data.push_back(ascii_hex_to_byte(raw[i], raw[i + 1]));
  }

  if (data_len < 10) {
    ESP_LOGW(TAG, "Response size is less than minimum allowed. Flushing RX buffer...");
    return false;
  }

  if (data[3] != 0) {
    ESP_LOGW(TAG, "Data received with error code 0x%02X: %s", data[3], format_hex_pretty(&data.front(), data.size()).c_str());
    return false;
  }

  if (request_all_packs_) {
    std::vector<uint8_t> bdata;
    uint8_t last_addr = data[7];
    if (last_addr == 0) {
      last_addr = 16;
    }
    uint16_t real_dlen = ((((((uint16_t(data[4]) << 8) | (uint16_t(data[5]) << 0)) & 0x0FFF) >> 1) - 2)) / last_addr;
    uint16_t idx = 8;
    uint8_t addr = 1;
    for (uint16_t i = 0; i < real_dlen + 8; ++i) {
      bdata.push_back(data[i]);
    }

    bdata[4] = (real_dlen + 2) * 2 >> 8;
    bdata[5] = (real_dlen + 2) * 2;
    for (uint8_t addr = 1; addr <= last_addr; ++addr) {
      bdata[7] = addr & 0x0F;
      for (uint16_t i = 8; idx < data_len && i < real_dlen + 8; ++idx, ++i) {
        bdata[i] = data[idx];
      }
      auto *device = this->get_device(addr);
      if (device != nullptr) {
        device->on_pace_modbus_data(bdata);
      } else {
        ESP_LOGW(TAG, "Got PaceModbus frame from not configured address 0x%02X! ", bdata[7]);
      }
    }
  } else {
    uint8_t address = data[7];

    auto *device = this->get_device(address);
    if (device != nullptr) {
      device->on_pace_modbus_data(data);
    } else {
      ESP_LOGW(TAG, "Got PaceModbus frame from unknown address 0x%02X! ", address);
    }
  }

  // return false to reset buffer
  return false;
}

void PaceModbus::dump_config() {
  ESP_LOGCONFIG(TAG, "PaceModbus:");
  LOG_PIN("  Flow Control Pin: ", this->flow_control_pin_);
  ESP_LOGCONFIG(TAG, "  RX timeout: %d ms", this->rx_timeout_);
}
float PaceModbus::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

void PaceModbus::send(uint8_t protocol_version, uint8_t address, uint8_t function, uint8_t value) {
  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(true);

  const uint16_t lenid = lchksum(1 * 2);
  std::vector<uint8_t> data;
  data.push_back(protocol_version);  // VER
  data.push_back(address);           // ADDR
  data.push_back(0x46);              // CID1
  data.push_back(function);          // CID2 (0x42)
  data.push_back(lenid >> 8);        // LCHKSUM (0xE0)
  data.push_back(lenid >> 0);        // LENGTH (0x02)
  data.push_back(value);             // PACK for who we are requesting

  const uint16_t frame_len = data.size();
  std::string payload = "~";  // SOF (0x7E)
  payload.append(byte_to_ascii_hex(data.data(), frame_len));

  uint16_t crc = chksum((const uint8_t *) payload.data() + 1, payload.size() - 1);
  data.push_back(crc >> 8);  // CHKSUM (0xFD)
  data.push_back(crc >> 0);  // CHKSUM (0x37)

  payload.append(byte_to_ascii_hex(data.data() + frame_len, data.size() - frame_len));  // Append checksum
  payload.append("\r");                                                                 // EOF (0x0D)

  ESP_LOGD(TAG, "Send frame: %s", payload.c_str());

  this->write_str(payload.c_str());
  this->flush();

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(false);
}

PaceModbusDevice* PaceModbus::get_device(const uint8_t address) const {
  for (auto *device : this->devices_) {
    if (device->address_ == address) {
      return device;
    }
  }

  return nullptr;
}

}  // namespace pace_modbus
}  // namespace esphome
