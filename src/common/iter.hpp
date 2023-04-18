// clang-format off
#pragma once

namespace Sol {
template <typename T> class Iterator { 
  virtual T next(); 
};

template <typename T> class Iter : public Iterator {
  T* next() const override;
};

template <typename T> class MutIter : public Iterator {
  T* next() override;
};

template <typename T> class IntoIter : public Iterator {
  T next() override;
}
} // namespace Sol
