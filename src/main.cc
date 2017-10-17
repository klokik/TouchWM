
#include <cassert>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#include "Primitives.hh"
#include "Surface.hh"


class Frame {
  public: enum class SplitType {Vertical, Horisontal};
  public: enum class SizeType {Absolute, Ratio};
  public: float width = 1.f;
  public: float height = 1.f;
  public: Color color = {255, 255, 255};
  public: SplitType split_type = SplitType::Vertical;
  public: SizeType size_type = SizeType::Ratio;

  public: std::weak_ptr<Frame> ancestor;
  public: std::vector<std::shared_ptr<Frame>> subframes;
};

std::tuple<int, int> drawFrameChilds(::Surface &gr, std::weak_ptr<Frame> _frame,
    const int _x = 0, const int _y = 0,
    const int _w = 640, const int _h = 480) {
  assert(!_frame.expired());
  auto frame = _frame.lock();

  int x = _x;
  int y = _y;
  int width = _w;
  int height = _h;

  switch (frame->size_type) {
  case Frame::SizeType::Absolute:
    width = frame->width;
    height = frame->height;
    break;

  case Frame::SizeType::Ratio:
    width = _w*frame->width;
    height = _h*frame->height;
    break;
  }

  if (frame->subframes.empty()) {
    gr.drawRect(x, y, width, height, frame->color);
    auto bcolor = adjColor(frame->color);
    gr.drawRect(x, y, width, height, bcolor, false);
    gr.drawLine(x, y, x+width, y+height, bcolor);
    gr.drawLine(x, y+height, x+width, y, bcolor);

    return {width, height};
  } else {

    std::vector<std::tuple<int, int>> sep_line_centers;
    for (auto &subframe : frame->subframes) {
      auto [sf_w, sf_h] = drawFrameChilds(gr, subframe, x, y, width, height);

      std::cout << "x: " << x << "; y: " << y << std::endl;
      if (frame->split_type == ::Frame::SplitType::Vertical) {
        x += sf_w;
        sep_line_centers.push_back({x, y+sf_h/2});
      } else {
        y += sf_h;
        sep_line_centers.push_back({x+sf_w/2, y});
      }
    }

    // drop last element
    sep_line_centers.resize(sep_line_centers.size()-1);
    // draw items
    for (const auto &[cx, cy] : sep_line_centers) {
      gr.drawCircle(cx, cy, 40, {200, 0, 0});
    }
  }
}


int main() {
  std::cout << "Hello" << std::endl;

  ::Surface gr("TouchMe", 640, 480);
  gr.clear();

  //gr.drawRect(100, 100, 200, 240, {0, 200, 200});

  auto root = std::make_shared<::Frame>();
  root->width = 640;
  root->height = 480;
  root->size_type = ::Frame::SizeType::Absolute;
  root->color = {128, 0, 0};

  auto subframe_l = std::make_shared<::Frame>();
  subframe_l->width = .5f;
  subframe_l->color = {0, 128, 0};
  auto subframe_r = std::make_shared<::Frame>();
  subframe_r->width = .5f;
  subframe_r->color = {0, 0, 255};

  root->subframes.push_back(subframe_l);
  root->subframes.push_back(subframe_r);

  drawFrameChilds(gr, root, 0, 0, 640, 480);

  gr.present();

  //gr.sleep(5000);
  gr.pollLoop();

  return 0;
}

