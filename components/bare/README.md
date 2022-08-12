# Bare sensor component

This sensor is an empty variable that will push the changes made to the internal value to home-assistant. It will default to `0` but the state can be changed true the `sensor.bare.set_value` lambda action.

## Example

This is an example use of the sersor, pressing the button will add `1` to the sersor state.

```yaml
sensor:
  - platform: bare
    id: bare_sensor
    name: Bare Sensor

button:
  - platform: template
    name: "+1"
    on_press:
    then:
      - sensor.bare.set_value:
        id: bare_sensor
        value: !lambda "return id(bare_sensor).state + 1;"
```
