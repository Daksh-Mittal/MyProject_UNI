# see https://makefiletutorial.com/ for guidance on how to interpret all this

CXX := g++ # not necessary but just for clarity
CXXFLAGS := -Wall -Werror -std=c++17 -g -O -MMD -MP -I./src

# use obj directory to minimise unnecessary compilation as we dont want to constantly recompile files that havent changed
SRC := $(shell find ./src -name '*.cpp')
OBJ := $(SRC:./src/%.cpp=./obj/%.o) # this is equivalent to calling patsubst, we're just getting the equivalent paths of .o files that will be generated for each .cpp file

.default: all

all: gen-village

clean:
	rm -rf ./bin/gen-village ./bin/gen-village.dSYM $(shell find ./obj -name '*.o') $(shell find ./obj -name '*.d')

gen-village: $(OBJ)
	$(CXX) $(CXXFLAGS) -o ./bin/$@ $^ -lmcpp

./obj/%.o: ./src/%.cpp
	@mkdir -p $(dir $@) #will error if a subdirectory doesnt already exist so we need to create subdirectories if they dont already exist
	$(CXX) $(CXXFLAGS) -c $< -o $@

# dependency tracking, - prefix because .d files will be missing if a file is new/it is the first time compiling
-include $(OBJ:.o=.d)
