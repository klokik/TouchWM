#pragma once

#include <functional>
#include <string>

#include <SDL2/SDL.h>

#include "Primitives.hh"


class Surface {
  public: Surface(std::string_view _name, const int _width, const int _height) :
      width(_width), height(_height) {
    this->window = SDL_CreateWindow(
        _name.data(), SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        _width, _height, SDL_WINDOW_SHOWN);

    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_SOFTWARE);
  }

  public: void clear() {
    SDL_SetRenderDrawColor(this->renderer,
      this->clear_color.r,
      this->clear_color.g,
      this->clear_color.b, 255);

    SDL_RenderClear(this->renderer);
  }

  public: void drawRect(const int _x, const int _y, const int _w, const int _h, Color const &_color, const bool _fill = true) {
    SDL_SetRenderDrawColor(this->renderer,
      _color.r, _color.g, _color.b, 255);

    SDL_Rect r { .x = _x, .y = _y, .w = _w, .h = _h };
    if (_fill)
      SDL_RenderFillRect(this->renderer, &r);
    else
      SDL_RenderDrawRect(this->renderer, &r);
  }

  public: void drawLine(const int _x1, const int _y1, const int _x2, const int _y2, Color const &_color) {
    SDL_SetRenderDrawColor(this->renderer,
      _color.r, _color.g, _color.b, 255);

    SDL_RenderDrawLine(this->renderer, _x1, _y1, _x2, _y2);
  }

  public: void drawMark(const int _x, const int _y, Color const &_color) {
    this->drawLine(_x-5, _y-5, _x+5, _y+5, _color);
    this->drawLine(_x-5, _y+5, _x+5, _y-5, _color);
  }

  public: void drawCircle(const int _x, const int _y, const int _diam, Color const &_color, const bool _fill = true) {
    /*SDL_SetRenderDrawColor(this->renderer,
      _color.r, _color.g, _color.b, 255);
 
    if (_fill)
    SDL_RenderDrawLine(this->renderer, _x1, _y1, _x2, _y2);*/
    this->drawRect(_x-_diam/2, _y-_diam/2, _diam, _diam, _color, _fill);
  }

  public: void present() {
    SDL_RenderPresent(this->renderer);
  }

  public: void sleep(const int ms) {
    SDL_Delay(ms);
  }

  public: bool pollOnce() {
    SDL_Event ev;

    if (SDL_PollEvent(&ev)) {
      switch (ev.type) {
      case SDL_QUIT:
        this->done = true;
        break;

      case SDL_KEYUP:
        if (ev.key.keysym.sym == SDLK_ESCAPE) {
          std::cout << "Escape" << std::endl;
          this->done = true;
        }
        break;

      case SDL_FINGERDOWN:
        if (this->onTouchDown)
          this->onTouchDown(ev.tfinger.x, ev.tfinger.y, ev.tfinger.fingerId, 0);
        break;

      case SDL_FINGERUP:
        if (this->onTouchUp)
          this->onTouchUp(ev.tfinger.x, ev.tfinger.y, ev.tfinger.fingerId, 0);
        break;

      case SDL_FINGERMOTION:
        if (this->onTouchMotion)
          this->onTouchMotion(ev.tfinger.x, ev.tfinger.y,
                              ev.tfinger.dx, ev.tfinger.dy,
                              ev.tfinger.fingerId, 0);
        break;

      case SDL_MOUSEBUTTONDOWN:
        if (ev.button.which == SDL_TOUCH_MOUSEID)
          break;

        if (this->onTouchDown)
          this->onTouchDown(ev.button.x, ev.button.y, ev.button.which, ev.button.button);
        break;

      case SDL_MOUSEBUTTONUP:
        if (ev.button.which == SDL_TOUCH_MOUSEID)
          break;

        if (this->onTouchUp)
          this->onTouchUp(ev.button.x, ev.button.y, ev.button.which, ev.button.button);
        break;

      case SDL_MOUSEMOTION:
        if (ev.motion.which == SDL_TOUCH_MOUSEID)
          break;

        if (this->onTouchMotion)
          this->onTouchMotion(ev.motion.x, ev.motion.y,
                              ev.motion.xrel, ev.motion.yrel,
                              ev.motion.which, ev.motion.state);
        break;

      default:
        //std::cout << "Some event" << std::endl;
        break;
      }
    }
    else
      return false;

    return true;
  }

  public: void pollLoop() {
    this->done = false;

    while (!this->done) {
      if (!this->pollOnce())
        this->sleep(30);
    }
  }

  public: ~Surface() {
    SDL_DestroyWindow(this->window);
    SDL_Quit();
  }

  public: Color clear_color = {100, 100, 100};

  public: std::function<void(int, int, int, int)> onTouchDown;
  public: std::function<void(int, int, int, int)> onTouchUp;
  public: std::function<void(int, int, int, int, int, int)> onTouchMotion;

  private: SDL_Window *window = nullptr;
  private: SDL_Renderer *renderer = nullptr;

  private: int width;
  private: int height;

  private: bool done;
};

