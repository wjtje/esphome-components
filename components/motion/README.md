# Motion cover

Custom cover component for two outputs with a pot meter.

## Options (All are required)

- `open_action` (**Template**): This action is performed when the remote request the cover to be opened.
- `almost_open_action` (**Template**): This action is performed when the cover is almost open, can be used to slow down the cover.
- `close_action` (**Template**): This action is performed when the remote requirest the cover to be closed.
- `almost_closed_action` (**Template**): This action is performed when the cover is almost closed, can be used to slow down the cover.
- `stop_action` (**Template**): This action is performed when the cover needs to stop.
- `force_stop_action` (**Template**): This action is performed when the cover needs to stop immediately.
- `position` (**Lambda**): This returns the current position of the pod meter, 0 closed, 1 opened.
- `almost_closed` (**float**): The zone that needs to be seen as almost closed, or almost opened.
- `can_open` (**Lambda**): This returns wheater the cover can be opened.
- `can_close` (**Lambda**): This return wheater the cover can be closed.
- `extra_close_duration` (**number**): How many miliseconds the door should keep closing the door after the position went to 0, this is helpful if your door need be really closed tight.

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
  - platform: motion
    name: Cover
    open_action:
      - output.turn_on: output_pwm_out
    almost_open_action: 
      - output.set_level:
          id: output_pwm_out
          level: 20%
    close_action:
      - output.turn_on: output_pwm_in
    almost_closed_action:
      - output.set_level:
          id: output_pwm_in
          level: 20%
    stop_action:
      - output.turn_off: output_pwm_out
      - output.turn_off: output_pwm_in
    force_stop_action:
      - lambda: |-
          id(output_pwm_out).force_write(0.0f);
          id(output_pwm_in).force_write(0.0f);
    position: !lambda |-
      return id(rotation).get_state();
    almost_closed: 0.2
    can_open: !lambda |-
      return id(output_pwm_in).get_state() != 0.0f;
    can_close: !lambda |-
      return id(output_pwm_out).get_state() != 0.0f;
    extra_close_duration: 500
```
