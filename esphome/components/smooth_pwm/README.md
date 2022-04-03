# Smooth PWM component

This component can be used to create a smooth PWM output.

Example:
```yaml
output:
  - platform: smooth_pwm
    id: my_pwm
    pin: 12
    duration: 5000
    deadzone: 63
```
