#pragma once

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

  public: void pollLoop() {
    this->done = false;
    SDL_Event ev;

    while (!this->done) {
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

        case SDL_FINGERMOTION:
          this->drawMark(ev.tfinger.x, ev.tfinger.y, {255, 255, 255});
          this->present();
          std::cout << "Touch event: id" << ev.tfinger.fingerId << std::endl;
          break;

        case SDL_MOUSEMOTION:
          if (!(ev.motion.state & SDL_BUTTON_LMASK))
            break;
          this->drawMark(ev.motion.x, ev.motion.y, {200, 200, 200});
          this->present();
          std::cout << "Mouse event" << std::endl;
          break;

        default:
          //std::cout << "Some event" << std::endl;
          break; 
        }
      }
      else
        this->sleep(100);
    }
  }

  public: ~Surface() {
    SDL_DestroyWindow(this->window);
    SDL_Quit();
  }

  public: Color clear_color = {100, 100, 100};

  private: SDL_Window *window = nullptr;
  private: SDL_Renderer *renderer = nullptr;

  private: int width;
  private: int height;

  private: bool done;
};

