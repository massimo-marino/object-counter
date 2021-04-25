//
// main.cpp
//
//
#include <iostream>

#include "classWithObjectCounter.h"

int main() {
  {
    classWithObjectCounter obj1("obj1");
  }  // obj1 destroyed

  classWithObjectCounter obj2("obj2");
  classWithObjectCounter* pobj3 = new classWithObjectCounter("obj3");
  delete pobj3;  // *pobj3 destroyed

  classWithObjectCounter obj4(obj2);

  auto [objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions] = classWithObjectCounter::getObjectCounters();
  auto [copyConstructions, copyAssignments, moveConstructions, moveAssignments] = classWithObjectCounter::getCopyMoveCounters();

  classWithObjectCounter::reportCounters("classWithObjectCounter");

  return 0;
}  // obj2, obj4 destroyed
