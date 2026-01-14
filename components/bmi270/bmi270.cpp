#include "bmi270.h"
#include "bmi270_config.h"
#include "esphome/core/log.h"

namespace esphome {
namespace bmi270 {

static const char *const TAG = "bmi270";

// BMI270 Register addresses
#define BMI2_CHIP_ID_ADDR 0x00
#define BMI2_PWR_CONF_ADDR 0x7C
#define BMI2_PWR_CTRL_ADDR 0x7D
#define BMI2_INIT_CTRL_ADDR 0x59
#define BMI2_INIT_DATA_ADDR 0x5E
#define BMI2_ACC_CONF_ADDR 0x40
#define BMI2_ACC_RANGE_ADDR 0x41
#define BMI2_GYR_CONF_ADDR 0x42
#define BMI2_GYR_RANGE_ADDR 0x43
#define BMI2_ACC_DATA_ADDR 0x0C
#define BMI2_GYR_DATA_ADDR 0x12
#define BMI2_INTERNAL_STATUS_ADDR 0x21

#define BMI2_CHIP_ID 0x24
#define BMI2_INIT_DATA_SIZE sizeof(bmi270_config_file)

// BMI270 API function implementations
int8_t bmi270_init(bmi2_dev *dev) {
  uint8_t chip_id;
  int8_t rslt = dev->read(BMI2_CHIP_ID_ADDR, &chip_id, 1, dev->intf_ptr);
  if (rslt != BMI2_OK) return rslt;

  if (chip_id != BMI2_CHIP_ID) return BMI2_E_COM_FAIL;

  dev->chip_id = chip_id;

  // Disable advanced power save
  uint8_t pwr_conf = 0x00;
  rslt = dev->write(BMI2_PWR_CONF_ADDR, &pwr_conf, 1, dev->intf_ptr);
  if (rslt != BMI2_OK) return rslt;
  dev->delay_us(450, dev->intf_ptr);

  // Prepare for config file upload
  uint8_t init_ctrl = 0x00;
  rslt = dev->write(BMI2_INIT_CTRL_ADDR, &init_ctrl, 1, dev->intf_ptr);
  if (rslt != BMI2_OK) return rslt;

  // Upload config file
  for (uint16_t i = 0; i < BMI2_INIT_DATA_SIZE; i += 32) {
    uint16_t len = (BMI2_INIT_DATA_SIZE - i) > 32 ? 32 : (BMI2_INIT_DATA_SIZE - i);
    rslt = dev->write(BMI2_INIT_DATA_ADDR, &bmi270_config_file[i], len, dev->intf_ptr);
    if (rslt != BMI2_OK) return rslt;
    dev->delay_us(500, dev->intf_ptr);
  }

  // Complete config upload
  init_ctrl = 0x01;
  rslt = dev->write(BMI2_INIT_CTRL_ADDR, &init_ctrl, 1, dev->intf_ptr);
  if (rslt != BMI2_OK) return rslt;
  dev->delay_us(20000, dev->intf_ptr);

  return BMI2_OK;
}

int8_t bmi270_sensor_enable(const uint8_t *sens_list, uint8_t n_sens, bmi2_dev *dev) {
  // Enable accelerometer (0x04) and gyroscope (0x02)
  uint8_t pwr_ctrl = 0x06; // ACC (0x04) + GYR (0x02) = 0x06
  return dev->write(BMI2_PWR_CTRL_ADDR, &pwr_ctrl, 1, dev->intf_ptr);
}

int8_t bmi270_set_sensor_config(bmi2_sens_config *sens_cfg, uint8_t n_sens, bmi2_dev *dev) {
  if (sens_cfg->type == BMI2_ACCEL) {
    uint8_t acc_conf = sens_cfg->cfg.acc.odr | (sens_cfg->cfg.acc.bw << 4);
    uint8_t acc_range = sens_cfg->cfg.acc.range;
    int8_t rslt = dev->write(BMI2_ACC_CONF_ADDR, &acc_conf, 1, dev->intf_ptr);
    if (rslt != BMI2_OK) return rslt;
    return dev->write(BMI2_ACC_RANGE_ADDR, &acc_range, 1, dev->intf_ptr);
  } else if (sens_cfg->type == BMI2_GYRO) {
    uint8_t gyr_conf = sens_cfg->cfg.gyr.odr | (sens_cfg->cfg.gyr.bw << 4);
    uint8_t gyr_range = sens_cfg->cfg.gyr.range;
    int8_t rslt = dev->write(BMI2_GYR_CONF_ADDR, &gyr_conf, 1, dev->intf_ptr);
    if (rslt != BMI2_OK) return rslt;
    return dev->write(BMI2_GYR_RANGE_ADDR, &gyr_range, 1, dev->intf_ptr);
  }
  return BMI2_OK;
}

int8_t bmi2_get_sensor_data(bmi2_sensor_data *sensor_data, uint8_t n_sens, bmi2_dev *dev) {
  uint8_t data[12];

  // Read accelerometer data
  int8_t rslt = dev->read(BMI2_ACC_DATA_ADDR, data, 6, dev->intf_ptr);
  if (rslt != BMI2_OK) return rslt;

  sensor_data[0].sens_data.acc.x = (int16_t)(data[0] | (data[1] << 8));
  sensor_data[0].sens_data.acc.y = (int16_t)(data[2] | (data[3] << 8));
  sensor_data[0].sens_data.acc.z = (int16_t)(data[4] | (data[5] << 8));

  // Read gyroscope data
  rslt = dev->read(BMI2_GYR_DATA_ADDR, data, 6, dev->intf_ptr);
  if (rslt != BMI2_OK) return rslt;

  sensor_data[1].sens_data.gyr.x = (int16_t)(data[0] | (data[1] << 8));
  sensor_data[1].sens_data.gyr.y = (int16_t)(data[2] | (data[3] << 8));
  sensor_data[1].sens_data.gyr.z = (int16_t)(data[4] | (data[5] << 8));

  return BMI2_OK;
}

void BMI270Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up BMI270...");

  this->sensor_.intf_ptr = this;
  this->sensor_.intf = BMI2_I2C_INTF;
  this->sensor_.read = read_bytes;
  this->sensor_.write = write_bytes;
  this->sensor_.delay_us = delay_usec;

  int8_t rslt;

  // Initialize the BMI270
  rslt = bmi270_init(&this->sensor_);
  if (rslt != BMI2_OK) {
    ESP_LOGE(TAG, "BMI270 initialization failed: %d", rslt);
    this->failure_reason_ = "Initialization failed with error code " + std::to_string(rslt);
    this->mark_failed();
    return;
  }

  ESP_LOGI(TAG, "BMI270 initialization succeeded");

  // Enable accelerometer and gyroscope
  uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_GYRO };
  rslt = bmi270_sensor_enable(sens_list, 2, &this->sensor_);
  if (rslt != BMI2_OK) {
    ESP_LOGE(TAG, "Failed to enable accelerometer and gyroscope: %d", rslt);
    this->failure_reason_ = "Sensor enable failed with error code " + std::to_string(rslt);
    this->mark_failed();
    return;
  }
  ESP_LOGI(TAG, "Sensors enabled successfully");

  // Configure accelerometer
  this->accel_cfg_.type = BMI2_ACCEL;
  this->accel_cfg_.cfg.acc.odr = BMI2_ACC_ODR_100HZ;
  this->accel_cfg_.cfg.acc.range = BMI2_ACC_RANGE_2G;
  this->accel_cfg_.cfg.acc.bw = BMI2_ACC_NORMAL_AVG4;
  this->accel_cfg_.cfg.acc.perf_mode = BMI2_PERF_OPT_MODE;
  rslt = bmi270_set_sensor_config(&this->accel_cfg_, 1, &this->sensor_);
  if (rslt != BMI2_OK) {
    ESP_LOGE(TAG, "Failed to configure accelerometer: %d", rslt);
    this->failure_reason_ = "Accelerometer config failed with error code " + std::to_string(rslt);
    this->mark_failed();
    return;
  }

  // Configure gyroscope
  this->gyro_cfg_.type = BMI2_GYRO;
  this->gyro_cfg_.cfg.gyr.odr = BMI2_GYR_ODR_100HZ;
  this->gyro_cfg_.cfg.gyr.range = BMI2_GYR_RANGE_2000;
  this->gyro_cfg_.cfg.gyr.bw = BMI2_GYR_NORMAL_MODE;
  this->gyro_cfg_.cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;
  this->gyro_cfg_.cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;
  rslt = bmi270_set_sensor_config(&this->gyro_cfg_, 1, &this->sensor_);
  if (rslt != BMI2_OK) {
    ESP_LOGE(TAG, "Failed to configure gyroscope: %d", rslt);
    this->failure_reason_ = "Gyroscope config failed with error code " + std::to_string(rslt);
    this->mark_failed();
    return;
  }

  this->is_initialized_ = true;
}

void BMI270Component::update() {
  if (!this->is_initialized_)
    return;

  struct bmi2_sensor_data sensor_data[2] = {};
  sensor_data[0].type = BMI2_ACCEL;
  sensor_data[1].type = BMI2_GYRO;

  int8_t rslt = bmi2_get_sensor_data(sensor_data, 2, &this->sensor_);
  if (rslt != BMI2_OK) {
    ESP_LOGW(TAG, "Failed to read sensor data: %d", rslt);
    return;
  }

  if (this->accel_x_sensor_ != nullptr)
    this->accel_x_sensor_->publish_state(sensor_data[0].sens_data.acc.x / 1000.0f);
  if (this->accel_y_sensor_ != nullptr)
    this->accel_y_sensor_->publish_state(sensor_data[0].sens_data.acc.y / 1000.0f);
  if (this->accel_z_sensor_ != nullptr)
    this->accel_z_sensor_->publish_state(sensor_data[0].sens_data.acc.z / 1000.0f);

  if (this->gyro_x_sensor_ != nullptr)
    this->gyro_x_sensor_->publish_state(sensor_data[1].sens_data.gyr.x / 16.4f);
  if (this->gyro_y_sensor_ != nullptr)
    this->gyro_y_sensor_->publish_state(sensor_data[1].sens_data.gyr.y / 16.4f);
  if (this->gyro_z_sensor_ != nullptr)
    this->gyro_z_sensor_->publish_state(sensor_data[1].sens_data.gyr.z / 16.4f);
}

void BMI270Component::dump_config() {
  ESP_LOGCONFIG(TAG, "BMI270:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "  Initialization failed: %s", this->failure_reason_.c_str());
  }
}

float BMI270Component::get_setup_priority() const {
  return setup_priority::DATA;
}

int8_t BMI270Component::read_bytes(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr) {
  auto *component = reinterpret_cast<BMI270Component *>(intf_ptr);
  if (component->read_register(reg_addr, data, len) != i2c::ERROR_OK) {
    return BMI2_E_COM_FAIL;
  }
  return BMI2_OK;
}

int8_t BMI270Component::write_bytes(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr) {
  auto *component = reinterpret_cast<BMI270Component *>(intf_ptr);
  if (component->write_register(reg_addr, data, len) != i2c::ERROR_OK) {
    return BMI2_E_COM_FAIL;
  }
  return BMI2_OK;
}

void BMI270Component::delay_usec(uint32_t period, void *) {
  delay_microseconds_safe(period);
}

}  // namespace bmi270
}  // namespace esphome
