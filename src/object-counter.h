//
// object-counter.h
//
//
#pragma once

#include <iostream>
#include <tuple>
#include <mutex>
////////////////////////////////////////////////////////////////////////////////
namespace object_counter
{
using countersMutex = std::recursive_mutex;

//
// See:
// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
//
// Each time an object of class T is created, the constructor of objectCounter<T>
// is called, incrementing both the created and alive count.
// Each time an object of class T is destroyed, the alive count is decremented.
// It is important to note that objectCounter<T> and objectCounter<U> are two
// separate classes and this is why they will keep separate counts of T's and U's.
//
// counterType, the type of the counters, MUST be unsigned
// counterType's type is unsigned long by default
template <typename T, typename counterType = unsigned long>
class objectCounter
{
public:
  using objectCounters = std::tuple<counterType, counterType, counterType, bool>;
  using copyMoveCounters = std::tuple<counterType, counterType, counterType, counterType>;

  // default ctor
  objectCounter() noexcept(false)
  {
    std::lock_guard<countersMutex> lg(mtx_);
    ++objectsCreated_;
    ++objectsAlive_;
    if ( checkCounterOverflow() )
    {
      throw std::overflow_error("Object Counters in OVERFLOW");
    }
  }

  // copy ctor
  objectCounter([[maybe_unused]] const objectCounter& rhs) noexcept(false)
  {
    std::lock_guard<countersMutex> lg(mtx_);
    ++copyConstructions_;
    ++objectsCreated_;
    ++objectsAlive_;
    if ( checkCounterOverflow() )
    {
      throw std::overflow_error("Object Counters in OVERFLOW");
    }
  }

  // copy assignment operator=
  objectCounter& operator=([[maybe_unused]] const objectCounter& rhs) noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    ++copyAssignments_;
    return *this;
  }

  // move ctor
  objectCounter([[maybe_unused]] objectCounter&& rhs) noexcept(false)
  {
    std::lock_guard<countersMutex> lg(mtx_);
    ++moveConstructions_;
    ++objectsCreated_;
    ++objectsAlive_;
    if ( checkCounterOverflow() )
    {
      throw std::overflow_error("Object Counters in OVERFLOW");
    }
  }

  // move assignment operator=
  objectCounter& operator=([[maybe_unused]] objectCounter&& rhs) noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    ++moveAssignments_;
    return *this;
  }

  static
  counterType
  getObjectsCreatedCounter() noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    return objectsCreated_;
  }

  static
  counterType
  getObjectsAliveCounter() noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    return objectsAlive_;
  }

  static
  counterType
  getObjectsDestroyedCounter() noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    return objectsDestroyed_;
  }

  static
  bool
  getTooManyDestructionsFlag() noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    return tooManyDestructions_;
  }

  static
  counterType
  getCopyConstructionsCounter() noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    return copyConstructions_;
  }

  static
  counterType
  getCopyAssignmentsCounter() noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    return copyAssignments_;
  }

  static
  counterType
  getMoveConstructionsCounter() noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    return moveConstructions_;
  }

  static
  counterType
  getMoveAssignmentsCounter() noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    return moveAssignments_;
  }

  static
  auto
  getObjectCounters() noexcept -> objectCounters
  {
    std::lock_guard<countersMutex> lg(mtx_);
    return std::make_tuple(objectsCreated_, objectsAlive_, objectsDestroyed_, tooManyDestructions_);
  }

  static
  auto
  getCopyMoveCounters() noexcept -> copyMoveCounters
  {
    std::lock_guard<countersMutex> lg(mtx_);
    return std::make_tuple(copyConstructions_, copyAssignments_, moveConstructions_, moveAssignments_);
  }

  static
  void
  resetCounters() noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    objectsCreated_ = 0;
    objectsAlive_ = 0;
    objectsDestroyed_ = 0;
    copyConstructions_ = 0;
    copyAssignments_ = 0;
    moveConstructions_ = 0;
    moveAssignments_ = 0;
    tooManyDestructions_ = false;
  }

  static
  bool
  isLeakPossible () noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    return (objectsAlive_ > 0);
  }

  static
  void
  reportCounters(const std::string& className = "class counters") noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    std::cout << "\n" << className << " object counters:"
              << "\n--> objects created:       "
              << objectsCreated_
              << "\n--> objects alive:         "
              << objectsAlive_
              << "\n--> objects destroyed:     "
              << objectsDestroyed_
              << "\n--> copy constructions:    "
              << copyConstructions_
              << "\n--> copy assignments:      "
              << copyAssignments_
              << "\n--> move constructions:    "
              << moveConstructions_
              << "\n--> move assignments:      "
              << moveAssignments_
              << "\n--> too many destructions: "
              << tooManyDestructions_
              << "\n--> memory leak possible:  "
              << ((isLeakPossibleInternal()) ? "true" : "false")
              << std::endl << std::endl;
  }

protected:
  // objects should never be removed through pointers of this type
  ~objectCounter() noexcept
  {
    std::lock_guard<countersMutex> lg(mtx_);
    if (   (0 == objectsAlive_) // must be non-zero since we destroy an object
           || (objectsCreated_ != (objectsAlive_ + objectsDestroyed_)) ) {
      tooManyDestructions_ = true;
      return;
    }

    --objectsAlive_;
    ++objectsDestroyed_;
  }

private:
  // in a multithreaded process threads can allocate objects of the same class,
  // so static data must be protected with a mutex
  static inline countersMutex mtx_ {};
  static inline counterType objectsCreated_ {0};
  static inline counterType objectsAlive_ {0};
  static inline counterType objectsDestroyed_ {0};
  static inline counterType copyConstructions_ {0};
  static inline counterType copyAssignments_ {0};
  static inline counterType moveConstructions_ {0};
  static inline counterType moveAssignments_ {0};
  static inline bool tooManyDestructions_ {false};

  static constexpr
  bool
  isLeakPossibleInternal () noexcept
  {
    return (objectsAlive_ > 0);
  }

  static constexpr
  bool
  checkCounterOverflow() noexcept
  {
    return ( (0 == objectsAlive_) ||
             (objectsCreated_ != (objectsAlive_ + objectsDestroyed_)) );
  }
};  // class objectCounter

}  // namespace object_counter
