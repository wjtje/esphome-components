# KORTLINJE

A very basic component for the IKEA KORTLINJE.

## See also

- https://github.com/VIPnytt/Kortlinje
- https://github.com/kobebeefg/ikea-clock-lab/tree/main

## Example

```yml
display:
  - platform: kortlinje
    id: matrix
    clock_pins: [GPIO0, GPIO1, GPIO2]
    data_pins: [GPIO3, GPIO6, GPIO7]
    brightness: 7
    rotation: 0°
    update_interval: 1s
    lambda: |-
      it.fill(Color(255, 255, 255));
      id(matrix).set_indicator(esphome::kortlinje::LEFT_TOP, true);
```
