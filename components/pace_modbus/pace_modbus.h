#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace pace_modbus {

class PaceModbusDevice;

class PaceModbus : public uart::UARTDevice, public Component {
 public:
  PaceModbus() = default;

  void setup() override;

  void loop() override;

  void dump_config() override;

  void register_device(PaceModbusDevice *device) { this->devices_.push_back(device); }

  float get_setup_priority() const override;

  void send(uint8_t protocol_version, uint8_t address, uint8_t function, uint8_t value);
  void set_rx_timeout(uint16_t rx_timeout) { rx_timeout_ = rx_timeout; }
  void set_flow_control_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
  void set_update_interval(uint16_t interval) { this->update_interval_ = interval; }
  void set_wait_network(bool wait_network) { this->wait_network_ = wait_network; }
  void set_request_all_packs(bool request_all_packs) { this->request_all_packs_ = request_all_packs; }

 protected:
  PaceModbusDevice* get_device(const uint8_t address) const;

  uint16_t rx_timeout_{500};
  GPIOPin *flow_control_pin_{nullptr};
  uint16_t update_interval_{10000};
  bool wait_network_{false};

  bool parse_pace_modbus_byte_(uint8_t byte);
  bool status_send_{false};
  bool request_all_packs_{false};
  std::vector<uint8_t> rx_buffer_;
  uint32_t last_pace_modbus_byte_{0};
  uint32_t last_send_{0};
  uint32_t next_send_{0};
  uint8_t first_device_{0};
  uint8_t send_device_{0xFF};
  uint8_t last_device_{0};
  std::vector<PaceModbusDevice *> devices_;
};

class PaceModbusDevice {
 public:
  void set_parent(PaceModbus *parent) { parent_ = parent; }
  void set_address(uint8_t address) { address_ = address; }
  void set_pack(uint8_t pack) { pack_ = pack; }
  void set_protocol_version(uint8_t protocol_version) { protocol_version_ = protocol_version; }
  virtual void on_pace_modbus_data(const std::vector<uint8_t> &data) = 0;
  void send(uint8_t function, uint8_t value) {
    this->parent_->send(this->protocol_version_, this->address_, function, value);
  }
  uint8_t get_protocol_version() { return this->protocol_version_; }

 protected:
  friend PaceModbus;

  PaceModbus *parent_;
  uint8_t address_;
  uint8_t pack_;
  uint8_t protocol_version_;
};

}  // namespace pace_modbus
}  // namespace esphome
