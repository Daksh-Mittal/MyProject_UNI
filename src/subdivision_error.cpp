#include "subdivision_error.h"

subdivision_error::subdivision_error(const std::string& text, bool couldRandomise) : value(text), couldRandomise(couldRandomise) {}

bool subdivision_error::CouldRandomise() const {
  return couldRandomise;
}
