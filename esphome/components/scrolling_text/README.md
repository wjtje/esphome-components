# Scrolling text component

The component can be used to display scrolling text on any display with individual pixels. The value can be changed on the fly by using the `.set_value()` function available on the object.

## Example

```yaml
font:
  - file: "fonts/arial.ttf"
    size: 12
    id: arial_12
    glyphs: " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+[]{}\\|;:'\",.<>/?`~"

scrolling_text:
  - id: text1
    value: "Hello World! How are you doing?"
    font: arial_12

display:
  - platform: vga
    lambda: !lambda |
      it.rectangle(80, 100, 160, 16, Color(0, 255, 0));
      // id(text1).set_value("Something else");
      id(text1).draw(&it, 82, 101, 156, Color(255,255,255));
```
