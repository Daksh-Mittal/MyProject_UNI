#include "utils.h"

void print_student_details() {
  std::cout << "Student 1 - Task A: Daksh Mittal - S4075055 - @Daksh-Mittal " << std::endl ;
  std::cout << "Student 2 - Task B: Brandon Tan - S4167619 - @bt-uni " << std::endl ;
  std::cout << "Student 3 - Task C: Jacob Dong - S4172023 - @JDONNn " << std::endl ;
}

void print_command_line_arguments(int loc_x, int loc_z, int village_size, int plot_border, int seed, bool testmode) {
  std::cout << "Command Line Arguments: " << std::endl ;
  std::cout << "\t Location (x,z) : (" << loc_x << ", " << loc_z << ")" << std::endl ;
  std::cout << "\t Village Size : " << village_size << std::endl ;
  std::cout << "\t Plot Border : " << plot_border << std::endl ;
  std::cout << "\t Seed : " << seed << std::endl ;
  std::cout << "\t Testmode : " << testmode << std::endl ;
}
