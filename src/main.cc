
#include <cassert>
#include <iostream>
#include <memory>
#include <random>
#include <string>
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

void drawFrameChilds(::Surface &gr, std::weak_ptr<Frame> _frame,
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
    gr.drawRect(x, y, _w, _h, frame->color);
  } else {
    for (auto &subframe : frame->subframes)
      drawFrameChilds(gr, subframe, x, y, width, height);
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

  root->subframes.push_back(subframe_l);

  drawFrameChilds(gr, root, 0, 0, 640, 480);

  gr.present();

  //gr.sleep(5000);
  gr.pollLoop();

  return 0;
}

