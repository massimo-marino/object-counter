//
// classWithObjectCounter.h
//
//
#ifndef OBJECT_COUNTER_CLASSWITHOBJECTCOUNTER_H
#define OBJECT_COUNTER_CLASSWITHOBJECTCOUNTER_H

#include "object-counter.h"

class classWithObjectCounter final : public object_counter::objectCounter<classWithObjectCounter>
{
public:
  size_t x_;
  std::string name_;
  std::string s_;

  classWithObjectCounter(std::string name,
                         const size_t i = 0,
                         std::string s = "") :
          x_(i),
          name_(name),
          s_(std::move(s))
  {
    std::cout << "classWithObjectCounter:  CTOR CALLED for "
              << name_
              << " @0x"
              << this
              << std::endl;
  }

  virtual ~classWithObjectCounter();

  // copy ctor
  classWithObjectCounter([[maybe_unused]] const classWithObjectCounter& rhs) noexcept(false)
  {
    x_ = rhs.x_;
    name_ = rhs.name_ + "_copyCtor";
    s_ = rhs.s_;
  }
};

#endif //OBJECT_COUNTER_CLASSWITHOBJECTCOUNTER_H
