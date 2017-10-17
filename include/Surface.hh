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

  public: void drawRect(const int _x, const int _y, const int _w, const int _h, Color const &_color) {
    SDL_SetRenderDrawColor(this->renderer,
      _color.r, _color.g, _color.b, 255);

    SDL_Rect r { .x = _x, .y = _y, .w = _w, .h = _h };
    SDL_RenderFillRect(this->renderer, &r);
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
          std::cout << "Touch event: id" << ev.tfinger.fingerId << std::endl;
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

