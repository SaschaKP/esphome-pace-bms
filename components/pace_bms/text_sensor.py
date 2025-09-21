import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID, ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_PACE_BMS_ID, PACE_BMS_COMPONENT_SCHEMA

DEPENDENCIES = ["pace_bms"]

CODEOWNERS = ["@SaschaKP"]

CONF_ERRORS = "errors"

CONF_CELL_VOLTAGE_TEXT_WARNING_1 = "cell_voltage_text_warning_1"
CONF_CELL_VOLTAGE_TEXT_WARNING_2 = "cell_voltage_text_warning_2"
CONF_CELL_VOLTAGE_TEXT_WARNING_3 = "cell_voltage_text_warning_3"
CONF_CELL_VOLTAGE_TEXT_WARNING_4 = "cell_voltage_text_warning_4"
CONF_CELL_VOLTAGE_TEXT_WARNING_5 = "cell_voltage_text_warning_5"
CONF_CELL_VOLTAGE_TEXT_WARNING_6 = "cell_voltage_text_warning_6"
CONF_CELL_VOLTAGE_TEXT_WARNING_7 = "cell_voltage_text_warning_7"
CONF_CELL_VOLTAGE_TEXT_WARNING_8 = "cell_voltage_text_warning_8"
CONF_CELL_VOLTAGE_TEXT_WARNING_9 = "cell_voltage_text_warning_9"
CONF_CELL_VOLTAGE_TEXT_WARNING_10 = "cell_voltage_text_warning_10"
CONF_CELL_VOLTAGE_TEXT_WARNING_11 = "cell_voltage_text_warning_11"
CONF_CELL_VOLTAGE_TEXT_WARNING_12 = "cell_voltage_text_warning_12"
CONF_CELL_VOLTAGE_TEXT_WARNING_13 = "cell_voltage_text_warning_13"
CONF_CELL_VOLTAGE_TEXT_WARNING_14 = "cell_voltage_text_warning_14"
CONF_CELL_VOLTAGE_TEXT_WARNING_15 = "cell_voltage_text_warning_15"
CONF_CELL_VOLTAGE_TEXT_WARNING_16 = "cell_voltage_text_warning_16"

CONF_TEMPERATURE_TEXT_WARNING_1 = "temperature_text_warning_1"
CONF_TEMPERATURE_TEXT_WARNING_2 = "temperature_text_warning_2"
CONF_TEMPERATURE_TEXT_WARNING_3 = "temperature_text_warning_3"
CONF_TEMPERATURE_TEXT_WARNING_4 = "temperature_text_warning_4"
CONF_TEMPERATURE_TEXT_WARNING_5 = "temperature_text_warning_5"
CONF_TEMPERATURE_TEXT_WARNING_6 = "temperature_text_warning_6"

CONF_PACK_CHARGE_TEXT_WARNING = "pack_charge_text_warning"
CONF_PACK_VOLTAGE_TEXT_WARNING = "pack_voltage_text_warning"
CONF_PACK_DISCHARGE_TEXT_WARNING = "pack_discharge_text_warning"

ICON_ERRORS = "mdi:alert-circle-outline"

TEXT_SENSORS = [
    CONF_ERRORS,
    CONF_PACK_CHARGE_TEXT_WARNING,
    CONF_PACK_VOLTAGE_TEXT_WARNING,
    CONF_PACK_DISCHARGE_TEXT_WARNING,
]

CELLS_TEXT_WARNING = [
    CONF_CELL_VOLTAGE_TEXT_WARNING_1,
    CONF_CELL_VOLTAGE_TEXT_WARNING_2,
    CONF_CELL_VOLTAGE_TEXT_WARNING_3,
    CONF_CELL_VOLTAGE_TEXT_WARNING_4,
    CONF_CELL_VOLTAGE_TEXT_WARNING_5,
    CONF_CELL_VOLTAGE_TEXT_WARNING_6,
    CONF_CELL_VOLTAGE_TEXT_WARNING_7,
    CONF_CELL_VOLTAGE_TEXT_WARNING_8,
    CONF_CELL_VOLTAGE_TEXT_WARNING_9,
    CONF_CELL_VOLTAGE_TEXT_WARNING_10,
    CONF_CELL_VOLTAGE_TEXT_WARNING_11,
    CONF_CELL_VOLTAGE_TEXT_WARNING_12,
    CONF_CELL_VOLTAGE_TEXT_WARNING_13,
    CONF_CELL_VOLTAGE_TEXT_WARNING_14,
    CONF_CELL_VOLTAGE_TEXT_WARNING_15,
    CONF_CELL_VOLTAGE_TEXT_WARNING_16,
]

TEMPERATURES_TEXT_WARNING = [
    CONF_TEMPERATURE_TEXT_WARNING_1,
    CONF_TEMPERATURE_TEXT_WARNING_2,
    CONF_TEMPERATURE_TEXT_WARNING_3,
    CONF_TEMPERATURE_TEXT_WARNING_4,
    CONF_TEMPERATURE_TEXT_WARNING_5,
    CONF_TEMPERATURE_TEXT_WARNING_6,
]

CONFIG_SCHEMA = PACE_BMS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_ERRORS): text_sensor.text_sensor_schema(
            text_sensor.TextSensor,
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_1): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_2): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_3): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_4): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_5): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_6): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_7): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_8): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_9): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_10): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_11): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_12): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_13): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_14): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_15): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_TEXT_WARNING_16): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_TEMPERATURE_TEXT_WARNING_1): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_TEMPERATURE_TEXT_WARNING_2): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_TEMPERATURE_TEXT_WARNING_3): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_TEMPERATURE_TEXT_WARNING_4): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_TEMPERATURE_TEXT_WARNING_5): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_TEMPERATURE_TEXT_WARNING_6): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_PACK_CHARGE_TEXT_WARNING): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_PACK_VOLTAGE_TEXT_WARNING): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_PACK_DISCHARGE_TEXT_WARNING): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_PACE_BMS_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
    for i, key in enumerate(CELLS_TEXT_WARNING):
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(hub.set_cell_voltage_text_warning_sensor(i, sens))
    for i, key in enumerate(TEMPERATURES_TEXT_WARNING):
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(hub.set_temperature_text_warning_sensor(i, sens))
