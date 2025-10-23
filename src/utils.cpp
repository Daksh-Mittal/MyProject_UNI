#include "utils.h"

void printStudentDetails() {
  std::cout << "Student 1 - Task A: Daksh Mittal - S4075055 - @Daksh-Mittal " << std::endl ;
  std::cout << "Student 2 - Task B: Brandon Tan - S4167619 - @bt-uni " << std::endl ;
  std::cout << "Student 3 - Task C: Jacob Dong - S4172023 - @JDONNn " << std::endl ;
}

bool isTesting(std::string testingComponent) {
  return testingComponent == "" || (Config::GetInstance().IsTestMode() && testingComponent == Config::GetInstance().GetTestedComponentName());
} 

std::vector<Plot*> pointerisePlotVector(std::vector<Plot> plots) {
  std::vector<Plot*> plotPointers;

  for (auto& plot : plots) {
    plotPointers.push_back(new Plot(plot));
  }

  return plotPointers;
}
