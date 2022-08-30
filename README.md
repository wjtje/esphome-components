# External-Components-for-ESPHome

These are some components that I wrote for ESPHome that aren't included in the official ESPHome repository.

## Components

- [bare](components/bare)
- [motion] (components/motion)
- [scrolling_text](components/scrolling_text)
- [smooth_pwm](components/smooth_pwm)
- [vga](components/vga)

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
