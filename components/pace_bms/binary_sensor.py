import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID, DEVICE_CLASS_CONNECTIVITY, ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_PACE_BMS_ID, PACE_BMS_COMPONENT_SCHEMA

DEPENDENCIES = ["pace_bms"]

CODEOWNERS = ["@SaschaKP"]

ICON_BATTERY_HEART="mdi:battery-heart-variant"

CONF_ONLINE_STATUS = "online_status"

CONF_CELL_BALANCING_1 = "cell_balancing_1"
CONF_CELL_BALANCING_2 = "cell_balancing_2"
CONF_CELL_BALANCING_3 = "cell_balancing_3"
CONF_CELL_BALANCING_4 = "cell_balancing_4"
CONF_CELL_BALANCING_5 = "cell_balancing_5"
CONF_CELL_BALANCING_6 = "cell_balancing_6"
CONF_CELL_BALANCING_7 = "cell_balancing_7"
CONF_CELL_BALANCING_8 = "cell_balancing_8"
CONF_CELL_BALANCING_9 = "cell_balancing_9"
CONF_CELL_BALANCING_10 = "cell_balancing_10"
CONF_CELL_BALANCING_11 = "cell_balancing_11"
CONF_CELL_BALANCING_12 = "cell_balancing_12"
CONF_CELL_BALANCING_13 = "cell_balancing_13"
CONF_CELL_BALANCING_14 = "cell_balancing_14"
CONF_CELL_BALANCING_15 = "cell_balancing_15"
CONF_CELL_BALANCING_16 = "cell_balancing_16"

BINARY_SENSORS = [
    CONF_ONLINE_STATUS,
]

CELLS_BALANCING = [
    CONF_CELL_BALANCING_1,
    CONF_CELL_BALANCING_2,
    CONF_CELL_BALANCING_3,
    CONF_CELL_BALANCING_4,
    CONF_CELL_BALANCING_5,
    CONF_CELL_BALANCING_6,
    CONF_CELL_BALANCING_7,
    CONF_CELL_BALANCING_8,
    CONF_CELL_BALANCING_9,
    CONF_CELL_BALANCING_10,
    CONF_CELL_BALANCING_11,
    CONF_CELL_BALANCING_12,
    CONF_CELL_BALANCING_13,
    CONF_CELL_BALANCING_14,
    CONF_CELL_BALANCING_15,
    CONF_CELL_BALANCING_16,
]

CONFIG_SCHEMA = PACE_BMS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_ONLINE_STATUS): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CELL_BALANCING_1): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_2): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_3): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_4): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_5): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_6): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_7): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_8): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_9): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_10): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_11): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_12): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_13): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_14): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_15): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
        cv.Optional(CONF_CELL_BALANCING_16): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon=ICON_BATTERY_HEART,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_PACE_BMS_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await binary_sensor.register_binary_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
    for i, key in enumerate(CELLS_BALANCING):
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await binary_sensor.register_binary_sensor(sens, conf)
            cg.add(hub.set_cell_balancing_sensor(i, sens))
