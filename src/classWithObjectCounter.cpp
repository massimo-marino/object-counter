//
// classWithObjectCounter.cpp
//
//
#include "classWithObjectCounter.h"

classWithObjectCounter::~classWithObjectCounter() {
  std::cout << "~classWithObjectCounter: DTOR CALLED for "
            << name_
            << " @0x"
            << this
            << std::endl;
}
