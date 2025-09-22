#include "pace_bms.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace pace_bms {

static const char *const TAG = "pace_bms";

static const uint8_t MAX_NO_RESPONSE_COUNT = 12;//roughly 1 minute at 5 seconds update time

void PaceBms::on_pace_modbus_data(const std::vector<uint8_t> &data) {
  this->reset_online_status_tracker_();

  if (data.size() > 32 && data[8] >= 8 && data[8] <= 16)
  { //estimate data arriving, since UART bus is async and data could arrive even later than what we can expect
    uint8_t guess = data.size() - (data[8] * 2 + 8);  // stimate guess, 8 cells + 4 temp sensor minimum
    if (guess >= 24) {  // we have telemetry data - this is merely a guess but status is way more short
      this->on_telemetry_data_(data);
    } else { // we have status data
      this->on_status_data_(data);
    }

    return;
  }

  ESP_LOGW(TAG, "Unhandled data received (data_len: 0x%02X): %s", data[5],
           format_hex_pretty(&data.front(), data.size()).c_str());
}

void PaceBms::on_telemetry_data_(const std::vector<uint8_t> &data) {
  auto pace_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  ESP_LOGI(TAG, "Telemetry frame (%d bytes) received", data.size());
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // ->
  // 0x25014600F07A0001100CC70CC80CC70CC70CC70CC50CC60CC70CC70CC60CC70CC60CC60CC70CC60CC7060B9B0B990B990B990BB30BBCFF1FCCCD12D303286A008C2710E1E4
  //
  // *Data*
  //
  // Byte   Address Content: Description                      Decoded content               Coeff./Unit
  //   0    0x25             Protocol version      VER        2.0
  //   1    0x01             Device address        ADR        Address of the battery that sends the data (usually CAN/485A or RS232)
  //   2    0x46             Device type           CID1       Lithium iron phosphate battery BMS
  //   3    0x00             Function code         CID2       0x00: Normal, 0x01 VER error, 0x02 Chksum error, ...
  //   4    0xF0             Data length checksum  LCHKSUM
  //   5    0x7A             Data length           LENID      122 / 2 = 61
  //   6    0x00             Data flag
  //   7    0x01             Responding Address               address of the responding battery (485B/A - RS232 *ONLY*)
  ESP_LOGV(TAG, "Responding Address: %d", data[7]);
  //   8    0x10             Number of cells                  16
  uint8_t cells = (this->override_cell_count_) ? this->override_cell_count_ : data[8];

  ESP_LOGV(TAG, "Number of cells: %d", cells);
  //   9      0x0C 0xC7      Cell voltage 1                   3287 * 0.001f = 3.271         V
  //   11     0x0C 0xC8      Cell voltage 2                   3305 * 0.001f = 3.272         V
  //   ...    ...            ...
  //   39     0x0C 0xC7      Cell voltage 16                                                V
  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  float average_cell_voltage = 0.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  for (uint8_t i = 0; i < std::min((uint8_t) 16, cells); i++) {
    float cell_voltage = (float) pace_get_16bit(9 + (i * 2)) * 0.001f;
    average_cell_voltage = average_cell_voltage + cell_voltage;
    if (cell_voltage < min_cell_voltage) {
      min_cell_voltage = cell_voltage;
      min_voltage_cell = i + 1;
    }
    if (cell_voltage > max_cell_voltage) {
      max_cell_voltage = cell_voltage;
      max_voltage_cell = i + 1;
    }
    this->publish_state_(this->cells_[i].cell_voltage_sensor_, cell_voltage);
  }
  average_cell_voltage = average_cell_voltage / cells;

  this->publish_state_(this->min_cell_voltage_sensor_, min_cell_voltage);
  this->publish_state_(this->max_cell_voltage_sensor_, max_cell_voltage);
  this->publish_state_(this->max_voltage_cell_sensor_, max_voltage_cell);
  this->publish_state_(this->min_voltage_cell_sensor_, min_voltage_cell);
  this->publish_state_(this->delta_cell_voltage_sensor_, max_cell_voltage - min_cell_voltage);
  this->publish_state_(this->average_cell_voltage_sensor_, average_cell_voltage);

  uint8_t offset = 9 + (cells * 2);

  //   41     0x06           Number of temperatures           6                             V
  uint8_t temperature_sensors = data[offset];
  ESP_LOGV(TAG, "Number of temperature sensors: %d", temperature_sensors);

  //   42     0x0B 0x9B      Temperature sensor 1             (2971 - 2731) * 0.1f = 24.0          °C
  //   44     0x0B 0x99      Temperature sensor 2             (2969 - 2731) * 0.1f = 23.8          °C
  //   46     0x0B 0x99      Temperature sensor 3             (2969 - 2731) * 0.1f = 23.8          °C
  //   48     0x0B 0x99      Temperature sensor 4             (2969 - 2731) * 0.1f = 23.8          °C
  //   50     0x0B 0xB3      Environment temperature          (2995 - 2731) * 0.1f = 26.4          °C
  //   52     0x0B 0xBC      Mosfet temperature               (3004 - 2731) * 0.1f = 27.3          °C
  for (uint8_t i = 0; i < std::min((uint8_t) 6, temperature_sensors); i++) {
    float raw_temperature = (float) pace_get_16bit(offset + 1 + (i * 2));
    this->publish_state_(this->temperatures_[i].temperature_sensor_, (raw_temperature - 2731.0f) * 0.1f);
  }
  offset = offset + 1 + (temperature_sensors * 2);

  //   54     0xC7 0x0C      Charge/discharge current         signed int?                   A
  float current = (float) ((int16_t) pace_get_16bit(offset)) * 0.01f;
  this->publish_state_(this->current_sensor_, current);

  //   56     0xC6 0x0C      Total battery voltage            50700 * 0.001f = 50.70          V
  float total_voltage = (float) pace_get_16bit(offset + 2) * 0.001f;
  this->publish_state_(this->total_voltage_sensor_, total_voltage);

  float power = total_voltage * current;
  this->publish_state_(this->power_sensor_, power);
  this->publish_state_(this->charging_power_sensor_, std::max(0.0f, power));               // 500W vs 0W -> 500W
  this->publish_state_(this->discharging_power_sensor_, std::abs(std::min(0.0f, power)));  // -500W vs 0W -> 500W

  //   58     0x12 0xD3      Residual capacity                4819 * 0.01f = 48.19        Ah
  float remaining_capacity = (float) pace_get_16bit(offset + 4) * 0.01f;
  this->publish_state_(this->residual_capacity_sensor_, remaining_capacity);

  //   60     0x03           Custom number                    03
  //   61     0x28 0x6A      Battery capacity                 10346 * 0.01f = 103.46        Ah
  float full_capacity = (float) pace_get_16bit(offset + 7) * 0.01f;
  this->publish_state_(this->battery_capacity_sensor_, full_capacity);

  //   63     0x00 0x8C      Number of cycles                 140
  this->publish_state_(this->charging_cycles_sensor_, (float) pace_get_16bit(offset + 9));
  
  //   65     0x27 0x10      Rated capacity                   10000 * 0.01f = 100.00        Ah
  float rated_capacity = (float) pace_get_16bit(offset + 11) * 0.01f;
  this->publish_state_(this->rated_capacity_sensor_, rated_capacity);

  // calculate some "extras"
  this->publish_state_(this->state_of_charge_sensor_, (remaining_capacity / full_capacity) * 100.0f);
  this->publish_state_(this->state_of_health_sensor_, std::min(100.0f, (full_capacity / rated_capacity) * 100.0f));

}

static const char *const WARNING_MESSAGES[4] = {
    "OK",                 // 0
    "Below Lower Limit",  //  1
    "Above Upper Limit",  //  2
    "Other Fault"         //  Other values
};

/*static const char *const PROTECT_1_MESSAGES[8] = {
    "High Cell Voltage\n",       // bit 1
    "Low Cell Voltage\n",        // bit 2
    "High Total Voltage\n",      // bit 3
    "Low Total Voltage\n",       // bit 4
    "Charge Current\n",          // bit 5
    "Discharge Current\n",       // bit 6
    "Short Circuit\n",           // bit 7
    "Charger High Voltage IN\n", // bit 8
};

static const char *const PROTECT_2_MESSAGES[8] = {
    "High Charge Temperature\n",        // bit 1
    "High Discharge Temperature\n",     // bit 2
    "Low Charge Temperature\n",         // bit 3
    "Low Discharge Temperature\n",      // bit 4
    "High MOSFET Temperature\n",        // bit 5
    "High Environmental Temperature\n", // bit 6
    "Low Environmental Temperature\n",  // bit 7
    "Fully\n",                          // bit 8
};*/

static const char *const PROTECT_MESSAGES[16] = {
    "High Cell Voltage\n",               // bit 1
    "Low Cell Voltage\n",                // bit 2
    "High Total Voltage\n",              // bit 3
    "Low Total Voltage\n",               // bit 4
    "Charge Current\n",                  // bit 5
    "Discharge Current\n",               // bit 6
    "Short Circuit\n",                   // bit 7
    "Charger High Voltage IN\n",         // bit 8
    "High Charge Temperature\n",         // bit 9
    "High Discharge Temperature\n",      // bit 10
    "Low Charge Temperature\n",          // bit 11
    "Low Discharge Temperature\n",       // bit 12
    "High MOSFET Temperature\n",         // bit 13
    "High Environmental Temperature\n",  // bit 14
    "Low Environmental Temperature\n",   // bit 15
    "Fully\n",                           // bit 16
};

static const char *const STATUS_MESSAGES[8] = {
    "Charge Current Limiter Disabled\n",       // bit 1
    "Charge MOSFET On\n",                      // bit 2
    "Discharge MOSFET On\n",                   // bit 3
    "Discharging\n",                           // bit 4
    "Positive/Negative Terminals Inverted\n",  // bit 5
    "Charging\n",                              // bit 6
    "AC IN\n",                                 // bit 7
    "Heater Enabled\n",                        // bit 8
};

static const char *const FAULT_MESSAGES[8] = {
    "Charge MOSFET fault\n",     // bit 1
    "Discharge MOSFET fault\n",  // bit 2
    "NTC fault\n",               // bit 3
    "Comm Fault\n",              // bit 4
    "Cell fault\n",              // bit 5
    "Sampling Fault\n",          // bit 6
    "CCB Fault\n",               // bit 7
    "Heater Fault\n",            // bit 8
};

static const char *const CONFIG_MESSAGES[8] = {
    "Warning Buzzer Enabled\n",         // bit 1  
    "Charge MOSFET Turned Off\n",       // bit 2
    "Discharge MOSFET Turned Off\n",    // bit 3
    "",                                 // bit 4
    "Charge Current Limiter Enabled\n", // bit 5
    "Warning LED Enabled\n",            // bit 6
    "Static Balance\n",                 // bit 7
    "Undefined bit 8\n",                // bit 8
};

// ==== Status Information
// 0 Responding Bus Id
// 1 Cell Count (this example has 16 cells)
// 2 Cell Warning (repeated Cell Count times) see: DecodeWarningValue / enum WarningValue
// 3 Temperature Count (this example has 6 temperatures)
// 4 Temperature Warning (repeated Temperature Count times) see: DecodeWarningValue / enum WarningValue
// 5 Charge Current Warning see: DecodeWarningValue / enum WarningValue
// 6 Total Voltage Warning see: DecodeWarningValue / enum WarningValue
// 7 Discharge Current Warning see: DecodeWarningValue / enum WarningValue
// 8 Protection Status 1 see: DecodeProtectionStatus1Value / enum ProtectionStatus1Flags
// 9 Protection Status 2 see: DecodeProtectionStatus2Value / enum ProtectionStatus2Flags
// 0 System Status see: DecodeSystemStatusValue
// 1 Configuration Status see: DecodeConfigurationStatusValue
// 2 Fault Status see: DecodeFaultStatusValue
// 3 Balance Status (high byte) set bits indicate those cells are balancing
// 4 Balance Status (low byte) set bits indicate those cells are balancing
// 5 Warning Status 1 see: DecodeWarningStatus1Value
// 6 Warning Status 2 see: DecodeWarningStatus2Value
// req:   ~25014644E00201FD2E.
// resp:  ~25014600004C000110000000000000000000000000000000000600000000000000000000000E000000000000EF3A.
//                       00112222222222222222222222222222222233444444444444556677889900112233445566

void PaceBms::on_status_data_(const std::vector<uint8_t>& data) {
  auto pace_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  ESP_LOGI(TAG, "Status frame (%d bytes) received", data.size());
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  ESP_LOGV(TAG, "Responding Address: %d", data[7]);

  uint8_t warn_limit = 3; //for future mods

  uint8_t offset = 8;
  uint8_t cells = (this->override_cell_count_) ? this->override_cell_count_ : data[offset]; // cell count
  ESP_LOGV(TAG, "Number of cells: %d", cells);

  for (uint8_t i = 0; i < std::min((uint8_t) 16, cells); ++i) {
    uint8_t warn = data[offset + 1 + i]; // cell warning (0 if no warning is emitted)
    this->publish_state_(this->cells_[i].cell_warning_sensor_, warn);
    this->publish_state_(this->cells_[i].cell_text_warning_sensor_, WARNING_MESSAGES[std::min(warn_limit, warn)]);
    if (warn != 0) {
      ESP_LOGV(TAG, "Cell %d warning: 0x{:x} - %s", i + 1, warn, WARNING_MESSAGES[std::min(warn_limit, warn)]);
    }
  }

  uint8_t temperatures = data[offset + 1 + cells]; // temperature count
  ESP_LOGV(TAG, "Number of temperatures: %d", temperatures);

  for (uint8_t i = 0; i < temperatures; ++i) {
    uint8_t warn = data[offset + 1 + cells + 1 + i]; // temperature warning (0 if no warning is emitted)
    this->publish_state_(this->temperatures_[i].temperature_warning_sensor_, warn);
    this->publish_state_(this->temperatures_[i].temperature_text_warning_sensor_, WARNING_MESSAGES[std::min(warn_limit, warn)]);
    if (warn != 0) {
      ESP_LOGV(TAG, "Temperature %d warning: 0x{:x} - %s", i + 1, warn, WARNING_MESSAGES[std::min(warn_limit, warn)]);
    }
  }

  uint8_t charge_warn = data[offset + 1 + cells + 1 + temperatures];  // charge current warning (0 if no warning is emitted)
  this->publish_state_(this->charge_current_warning_sensor_, charge_warn);
  this->publish_state_(this->charge_current_text_warning_sensor_, WARNING_MESSAGES[std::min(warn_limit, charge_warn)]);
  if (charge_warn != 0) {
    ESP_LOGV(TAG, "Charge warning: 0x{:x} - %s", charge_warn, WARNING_MESSAGES[std::min(warn_limit, charge_warn)]);
  }

  uint8_t voltage_warn = data[offset + 1 + cells + 1 + temperatures + 1]; // total voltage warning (0 if no warning)
  this->publish_state_(this->total_voltage_warning_sensor_, voltage_warn);
  this->publish_state_(this->total_voltage_text_warning_sensor_, WARNING_MESSAGES[std::min(warn_limit, voltage_warn)]);
  if (voltage_warn != 0) {
    ESP_LOGV(TAG, "Total Voltage warning: 0x{:x} - %s", voltage_warn, WARNING_MESSAGES[std::min(warn_limit, voltage_warn)]);
  }

  uint8_t discharge_warn = data[offset + 1 + cells + 1 + temperatures + 2]; // discharge current warning (0 if no warning is emitted)
  this->publish_state_(this->discharge_current_warning_sensor_, discharge_warn);
  this->publish_state_(this->discharge_current_text_warning_sensor_, WARNING_MESSAGES[std::min(warn_limit, discharge_warn)]);
  if (discharge_warn != 0) {
    ESP_LOGV(TAG, "Discharge warning: 0x{:x} - %s", discharge_warn, WARNING_MESSAGES[std::min(warn_limit, discharge_warn)]);
  }

  //uint8_t protect1 = data[offset + 1 + cells + 1 + temperatures + 3]; // protection 1 value
  //uint8_t protect2 = data[offset + 1 + cells + 1 + temperatures + 4];  // protection 2 value
  uint16_t protect = pace_get_16bit(offset + 1 + cells + 1 + temperatures + 3);
  std::string str;
  for (uint8_t i = 0; i < 16; ++i) {
    if ((protect & (1 << i)) != 0) {
      str.append(PROTECT_MESSAGES[i]);
    }
  }
  if (str.length() > 1) {
    str.pop_back();
    ESP_LOGV(TAG, "Protect Messages: %s", str.c_str());
  }
  str.clear();
  
  uint8_t status = data[offset + 1 + cells + 1 + temperatures + 5];  // system status value
  for (uint8_t i = 0; i < 8; ++i) {
    if ((status & (1 << i)) != 0) {
      str.append(STATUS_MESSAGES[i]);
    }
  }
  if (str.length() > 1) {
    str.pop_back();
    ESP_LOGV(TAG, "Status Messages: %s", str.c_str());
  }
  str.clear();

  uint8_t conf_status = data[offset + 1 + cells + 1 + temperatures + 6];  // config status value
  for (uint8_t i = 0; i < 8; ++i) {
    if ((conf_status & (1 << i)) != 0) {
      str.append(CONFIG_MESSAGES[i]);
    }
  }
  if (str.length() > 1) {
    str.pop_back();
    ESP_LOGV(TAG, "Config Messages: %s", str.c_str());
  }
  str.clear();

  uint8_t fault_status = data[offset + 1 + cells + 1 + temperatures + 7];  // fault status value
  for (uint8_t i = 0; i < 8; ++i) {
    if ((fault_status & (1 << i)) != 0) {
      str.append(FAULT_MESSAGES[i]);
    }
  }
  if (str.length() > 1) {
    str.pop_back();
    ESP_LOGV(TAG, "Fault Messages: %s", str.c_str());
  }
  str.clear();

  uint16_t balancing_status = pace_get_16bit(offset + 1 + cells + 1 + temperatures + 8);  // balancing state per cell
  for (uint8_t i = 0; i < cells; ++i) {
    bool balancing = (balancing_status & (1 << i)) != 0;
    this->publish_state_(this->cells_[i].cell_balancing_sensor_, balancing);
    if (balancing) {
      ESP_LOGV(TAG, "Cell %d is balancing", i + 1);
    }
  }
}

void PaceBms::dump_config() {
  ESP_LOGCONFIG(TAG, "PaceBms:");
  LOG_SENSOR("", "Minimum Cell Voltage", this->min_cell_voltage_sensor_);
  LOG_SENSOR("", "Maximum Cell Voltage", this->max_cell_voltage_sensor_);
  LOG_SENSOR("", "Minimum Voltage Cell", this->min_voltage_cell_sensor_);
  LOG_SENSOR("", "Maximum Voltage Cell", this->max_voltage_cell_sensor_);
  LOG_SENSOR("", "Delta Cell Voltage", this->delta_cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 1", this->cells_[0].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 2", this->cells_[1].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 3", this->cells_[2].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 4", this->cells_[3].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 5", this->cells_[4].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 6", this->cells_[5].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 7", this->cells_[6].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 8", this->cells_[7].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 9", this->cells_[8].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 10", this->cells_[9].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 11", this->cells_[10].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 12", this->cells_[11].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 13", this->cells_[12].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 14", this->cells_[13].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 15", this->cells_[14].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 16", this->cells_[15].cell_voltage_sensor_);
  LOG_SENSOR("", "Temperature 1", this->temperatures_[0].temperature_sensor_);
  LOG_SENSOR("", "Temperature 2", this->temperatures_[1].temperature_sensor_);
  LOG_SENSOR("", "Temperature 3", this->temperatures_[2].temperature_sensor_);
  LOG_SENSOR("", "Temperature 4", this->temperatures_[3].temperature_sensor_);
  LOG_SENSOR("", "Temperature 5", this->temperatures_[4].temperature_sensor_);
  LOG_SENSOR("", "Temperature 6", this->temperatures_[5].temperature_sensor_);
  LOG_SENSOR("", "Total Voltage", this->total_voltage_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Power", this->power_sensor_);
  LOG_SENSOR("", "Charging Power", this->charging_power_sensor_);
  LOG_SENSOR("", "Discharging Power", this->discharging_power_sensor_);
  LOG_SENSOR("", "Charging cycles", this->charging_cycles_sensor_);
  LOG_SENSOR("", "State of charge", this->state_of_charge_sensor_);
  LOG_SENSOR("", "Residual capacity", this->residual_capacity_sensor_);
  LOG_SENSOR("", "Battery capacity", this->battery_capacity_sensor_);
  LOG_SENSOR("", "Rated capacity", this->rated_capacity_sensor_);
  LOG_SENSOR("", "Charging cycles", this->charging_cycles_sensor_);
  LOG_SENSOR("", "Average Cell Voltage", this->average_cell_voltage_sensor_);
  LOG_SENSOR("", "State of health", this->state_of_health_sensor_);
  }

float PaceBms::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

void PaceBms::update() {
  this->track_online_status_();
  if (status_send_)
  {
    this->send(0x44, this->pack_); // status info
    status_send_ = false;
  } else {
    this->send(0x42, this->pack_);  // telemetry info
    status_send_ = true;
  }
}

void PaceBms::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void PaceBms::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void PaceBms::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

void PaceBms::track_online_status_() {
  if (this->no_response_count_ < MAX_NO_RESPONSE_COUNT) {
    this->no_response_count_++;
  }
  if (this->no_response_count_ == MAX_NO_RESPONSE_COUNT) {
    this->publish_device_unavailable_();
    this->no_response_count_++;
  }
}

void PaceBms::reset_online_status_tracker_() {
  this->no_response_count_ = 0;
  this->publish_state_(this->online_status_binary_sensor_, true);
}

void PaceBms::publish_device_unavailable_() {
  this->publish_state_(this->online_status_binary_sensor_, false);
  this->publish_state_(this->errors_text_sensor_, "Offline");

  this->publish_state_(this->min_cell_voltage_sensor_, NAN);
  this->publish_state_(this->max_cell_voltage_sensor_, NAN);
  this->publish_state_(this->min_voltage_cell_sensor_, NAN);
  this->publish_state_(this->max_voltage_cell_sensor_, NAN);
  this->publish_state_(this->delta_cell_voltage_sensor_, NAN);
  this->publish_state_(this->average_cell_voltage_sensor_, NAN);
  this->publish_state_(this->total_voltage_sensor_, NAN);
  this->publish_state_(this->current_sensor_, NAN);
  this->publish_state_(this->power_sensor_, NAN);
  this->publish_state_(this->charging_power_sensor_, NAN);
  this->publish_state_(this->discharging_power_sensor_, NAN);
  this->publish_state_(this->state_of_charge_sensor_, NAN);
  this->publish_state_(this->residual_capacity_sensor_, NAN);
  this->publish_state_(this->battery_capacity_sensor_, NAN);
  this->publish_state_(this->rated_capacity_sensor_, NAN);
  this->publish_state_(this->charging_cycles_sensor_, NAN);
  this->publish_state_(this->state_of_health_sensor_, NAN);
  
  for (auto &temperature : this->temperatures_) {
    this->publish_state_(temperature.temperature_sensor_, NAN);
  }

  for (auto &cell : this->cells_) {
    this->publish_state_(cell.cell_voltage_sensor_, NAN);
  }
}

void PaceBms::setup() {
  this->stop_poller();
  // update interval is for all the sends, so we halve that value, since we have to do telemetry and the status
  // with one shot, RS232 is an async operation and is pretty slow at 9600bps
  this->set_retry(
      "", this->get_rx_timeout() * (this->get_address() + 1), 1,
      [this](const uint8_t remaining_attempts) {
        this->set_update_interval(this->get_update_interval() >> 1);
        this->start_poller();
        return RetryResult::DONE;
      },
      1);
}

}  // namespace pace_bms
}  // namespace esphome
