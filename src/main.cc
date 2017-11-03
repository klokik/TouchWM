
#include <cassert>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#include "Primitives.hh"
#include "Surface.hh"


class Frame
{
  public: enum class SplitType {Vertical, Horizontal};
  public: enum class SizeType {Absolute, Ratio};

  public: float preferred_width = 1.f;
  public: float preferred_height = 1.f;

  public: Color color = {255, 255, 255};
  public: SplitType split_type = SplitType::Vertical;
  public: SizeType size_type = SizeType::Ratio;

  public: std::list<Frame> subframes;

  public: Frame() = default;

  public: ~Frame() {
    std::cout << "Destroyed" << std::endl;
  }

  public: int getWidth() const noexcept {
    return this->actual_width;
  }

  public: int getHeight() const noexcept {
    return this->actual_height;
  }

  public: int getLeft() const noexcept {
    return this->actual_left;
  }

  public: int getTop() const noexcept {
    return this->actual_top;
  }

  protected: void updateSubframes() {
    int left = this->getLeft();
    int top = this->getTop();

    float width_ratio_sum = 0;
    float height_ratio_sum = 0;

    for (auto &item : this->subframes) {
      width_ratio_sum += item.preferred_width;
      height_ratio_sum += item.preferred_height;
    }

    for (auto &item : this->subframes) {
      item.actual_left = left;
      item.actual_top = top;

      if (this->split_type == SplitType::Horizontal) {
        item.actual_width = this->getWidth();
        item.actual_height = this->getHeight()*item.preferred_height/height_ratio_sum;

        top += item.actual_height;
      } else {
        item.actual_width = this->getWidth()*item.preferred_width/width_ratio_sum;
        item.actual_height = this->getHeight();

        left += item.actual_width;
      }
    }

    for (auto &item : this->subframes)
      item.updateSubframes();
  }

  public: void setFixedSize(int _width, int _height) {
    this->size_type = SizeType::Absolute;

    this->preferred_width = this->actual_width = _width;
    this->preferred_height = this->actual_height = _height;

    this->invalidate();
  }

  public: Frame &newFrame() {
    return this->subframes.emplace_back();
  }

  public: void invalidate() {
    this->updateSubframes();
  }

  public: Frame& dispatchPosition(int _x, int _y) {
    if (this->subframes.empty())
      return *this;
    else
      for (auto &item : this->subframes) {
        if (item.getLeft() <= _x && item.getLeft() + item.getWidth() > _x &&
            item.getTop() <= _y && item.getTop() + item.getHeight() > _y)
          return item.dispatchPosition(_x, _y);
      }

    throw std::runtime_error("Wut??, Attempt to dispatch position which is not on frame");
  }

  private: int actual_width;
  private: int actual_height;
  private: int actual_left = 0;
  private: int actual_top = 0;
};

struct FrameEdgeHandle {
  ::Frame &first;
  ::Frame &second;

  int x;
  int y;
};

class Compositor {
  public: Compositor(int _width, int _height)
    : gr("TouchMeComp", _width, _height)
  {
    gr.clear();
    root.setFixedSize(_width, _height);

    this->initContent();
  }

  public: void present() {
    this->drawFrameR(this->root);

    this->edge_handles.clear();
    this->populateEdgeHandles(this->root);
    this->drawEdgeHandles();

    gr.present();
  }

  protected: void initContent() {
    using namespace std::placeholders;
    this->gr.onTouchDown = std::bind(&Compositor::onTouchDown, this, _1, _2, _3, _4);

    this->root.color = {128, 0, 0};

    auto &subframe_l = this->root.newFrame();
    subframe_l.preferred_width = .4f;
    subframe_l.color = {0, 128, 0};

    auto &subframe_r = this->root.newFrame();
    subframe_r.preferred_width = .6f;
    subframe_r.color = {0, 0, 255};
    subframe_r.split_type = ::Frame::SplitType::Horizontal;

    auto &subframe_rt = subframe_r.newFrame();
    subframe_rt.preferred_height = .3f;
    subframe_rt.color = {0, 100, 100};

    auto &subframe_rb = subframe_r.newFrame();
    subframe_rb.preferred_height = .7f;
    subframe_rb.color = {100, 100, 0};

    root.invalidate();
  }

  private: void drawFrameR(Frame &_frame,
    const int _x = 0, const int _y = 0,
    const int _w = 640, const int _h = 480) {

    int x = _frame.getLeft();
    int y = _frame.getTop();;
    int width = _frame.getWidth();
    int height = _frame.getHeight();

    if (_frame.subframes.empty()) {
      this->gr.drawRect(x, y, width, height, _frame.color);
      auto bcolor = adjColor(_frame.color);
      this->gr.drawRect(x, y, width, height, bcolor, false);
      this->gr.drawLine(x, y, x+width, y+height, bcolor);
      this->gr.drawLine(x, y+height, x+width, y, bcolor);
    } else {
      for (auto &subframe : _frame.subframes)
        drawFrameR(subframe);
    }
  }

  private: void drawEdgeHandles() {
    for (auto [first, second, x, y] : this->edge_handles)
      this->gr.drawCircle(x, y, 40, {200, 0, 0});
  }

  private: void onTouchDown(int _x, int _y, int _id, int _key) {
    std::cout << "touchDown " << std::endl;

    try {
      auto &frame = this->root.dispatchPosition(_x, _y);

      if (_key == 1)
        frame.split_type = ::Frame::SplitType::Horizontal;
      else if (_key == 3)
        frame.split_type = ::Frame::SplitType::Vertical;

      if (_key == 1 || _key == 3) {
        auto &fr1 = frame.newFrame();
        auto &fr2 = frame.newFrame();

        ::Color newcolor1 {rand() % 255, rand() % 255, rand() % 255};
        ::Color newcolor2 {rand() % 255, rand() % 255, rand() % 255};

        fr1.color = newcolor1;
        fr2.color = newcolor2;

        frame.invalidate();
      }

      this->present();
    }
    catch (std::exception &ex) {
      // lol :)
    }
  }

  private: void populateEdgeHandles(::Frame &_node) {
    if (_node.subframes.size() <= 1)
      return;

    this->populateEdgeHandles(_node.subframes.front());

    for (auto it = std::next(_node.subframes.begin()); it != _node.subframes.end(); ++it) {
      if (_node.split_type == ::Frame::SplitType::Horizontal)
        this->edge_handles.push_back(::FrameEdgeHandle{*std::prev(it), *it, _node.getLeft() + _node.getWidth()/2, it->getTop()});
      else
        this->edge_handles.push_back(::FrameEdgeHandle{*std::prev(it), *it, it->getLeft(), _node.getTop() + _node.getHeight()/2});

      this->populateEdgeHandles(*it);
    }
  }

  private: ::Frame root;
  public: ::Surface gr;

  private: std::vector<::FrameEdgeHandle> edge_handles;
};


int main() {
  std::cout << "Hello" << std::endl;

  Compositor compositor(640, 480);
  compositor.present();

  compositor.gr.pollLoop();

  return 0;
}

