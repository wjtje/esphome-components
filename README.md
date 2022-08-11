# External-Components-for-ESPHome

These are some components that I wrote for ESPHome that aren't included in the official ESPHome repository.

## Components

- [bare](esphome/components/bare)
- [scrolling_text](esphome/components/scrolling_text)
- [smooth_pwm](esphome/components/smooth_pwm)
- [vga](esphome/components/vga)

## How to use it

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/wjtje/External-Components-for-ESPHome
      ref: main
```

## License

MIT License

Copyright (c) 2021 Wouter van der Wal
