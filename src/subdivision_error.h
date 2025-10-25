#ifndef SUBDIVISION_ERROR
#define SUBDIVISION_ERROR

#include <exception>
#include <string>

class subdivision_error : public std::exception {
  public:
    subdivision_error(const std::string& text, bool couldRandomise);
    
    bool CouldRandomise() const;

  private:
    std::string value;
    bool couldRandomise;
};

#endif
