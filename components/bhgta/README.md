# BHGTA cover component

This is a custom cover component designed for personal use with a BHGTA linear actuator.

## Example

```yaml
output:
  - platform: smooth_pwm
    id: output_pwm_out
    pin: 14
    duration: 2500
    deadzone: 38
  - platform: smooth_pwm
    id: output_pwm_in
    pin: 12
    duration: 2500
    deadzone: 38

sensor:
  - platform: adc
    pin: A0
    id: rotation
    update_interval: 250ms
    accuracy_decimals: 1
    filters:
      - calibrate_linear:
          - 0.16 -> 0.0
          - 0.91 -> 1.0

button:
  - platform: template
    name: "Force stop"
    on_press:
      - lambda: |-
          id(output_pwm_in).force_write(0.0f);
          id(output_pwm_out).force_write(0.0f);

cover:
  - platform: bhgta
    name: Cover
    pwm_in: output_pwm_in
    pwm_out: output_pwm_out
    pot: rotation
    pot_almost: 0.2
    on_force_stop:
      - logger.log: "Force stop!"
```
