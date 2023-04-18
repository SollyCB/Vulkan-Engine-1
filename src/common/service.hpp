#pragma once

struct Service {

  virtual void init(void __attribute__((unused)) * configuration) {}
  virtual void shutdown() {}

}; // struct Service
