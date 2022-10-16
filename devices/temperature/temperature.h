#pragma once
#include <ArduinoJson.h>

namespace temperature {

typedef std::map<std::string, std::string> name_map_t;
name_map_t sensor_name_map;

void handle_json(ArduinoJson::JsonObjectConst &o) {
    const char* address;
    const char* name;
    sensor_name_map.clear();
    for (JsonObjectConst::iterator i = o.begin(); i != o.end(); ++i) {
        address = i->key().c_str();
        name = i->value().as<const char*>();
        sensor_name_map[address] = name;
        ESP_LOGW("rename", "Renamed sensor (%s) to %s", address, name);
    }
}

void handle_sensor_value(esphome::dallas::DallasTemperatureSensor *s, esphome::mqtt::MQTTClientComponent *mqtt_client) {
    std::string sensorAddress = s->get_address_name();
    std::string sensorValue = esphome::value_accuracy_to_string(s->state, 1);
    std::string addressTopic = "temperature/sensor/" + sensorAddress + "/state";
    name_map_t::iterator i = sensor_name_map.find(sensorAddress);
    if (i != sensor_name_map.end()) {
        std::string nameTopic = "temperature/sensor/" + sensor_name_map[sensorAddress] + "/state";
        mqtt_client->publish(nameTopic, sensorValue);
    }
    mqtt_client->publish(addressTopic, sensorValue);
}

}