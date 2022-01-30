# Bare sensor component

This component can be used to store an integer that can be read by home-assistant or other esphome nodes.

Example:
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
