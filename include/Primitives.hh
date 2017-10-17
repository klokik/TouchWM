#pragma once


struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

Color adjColor(Color const &_col) {
  uint8_t r = (255 - _col.r);
  uint8_t g = (255 - _col.g);
  uint8_t b = (255 - _col.b);

  return Color{r, g, b};
}

