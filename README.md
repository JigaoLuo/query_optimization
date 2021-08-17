# ***This the hand-in for sheet7-11***

# QO Homework

## Compilation

### CMake

We use CMake for compilation. CMake doesn't do the compilation itself, but acts as a intermediary between us and a native build system. We can use a single CMake configuration to generate either a Makefile (to be used with GNU Make), a Microsoft Visual Studio Solution, or Ninja configuration files. We then use the specific build system to run the actual build. Which build system you prefer is up to you (and what your system supports). Almost all systems support CMake.

Make sure to install a recent version of CMake. Our configurations are written for CMake 3.8 and up, and were tested with CMake 3.10.2.

#### Example build with CMake and GNU Make on Linux (or WSL)

We need to create a folder in which CMake will generate the necessary build configuration for the build system.

`mkdir build`

`cd build`

We tell cmake to generate Unix Makefiles, and that the compilation should be done in debug mode.

`cmake -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Debug  ..`

Now that the Makefiles are generated we can run `make`.

`make`

(The linking will fail if not all the necessary functions are implemented. If not, you we see the executables in the current directory.)

Let's also generate the dataset that will be used in the homework.

`make uni`

When you make changes to the source files, you can simple run `make` without re-running `cmake`.

After you complete your implementation, run our tests to see if you have any errors.

`make check`

#### Example build with CLion 2018.2

* Import Project From Sources (enter the directory where you cloned your fork).
* Open Existing Project.
* Wait for CLion to configure itself.
* Choose target from the top-right. Use the target `uni` to build the dataset. Choose the target `checker` to run the tests.  For the targets `uni` and `checker` you use the build button on the left to execute them. For actual target executables, you need to use the run button on the right to execute them, the build button will only generate the executables.

_Debugging_

* Open `examples/ChiExample.cpp` and put a breakpoint on the first line under `main()` (`Database db;`).
* Choose `ChiExample` as the target from the top-right.
* Press the debug button (the button that looks like a bug) to build & run the executable.
* The execution will stop at the first line, where you placed your breakpoint.
* Enjoy!

#### Example build with Microsoft Visual Studio 2017

* Make sure you have installed basic C++ support and C++ tools for CMake.
* File -> Open -> CMake
* Choose the CMakeLists.txt under the directory where you cloned your fork
* Wait for Visual Studio to configure itself.
* Use "CMake -> Build Only -> uni" to build the dataset.
* Use "CMake -> Build All" to build all the executables.
* Use "Test -> Windows -> Test Explorer" to open the testing window. Use "Run All" to run the test. Double click to view the details. You can also use "CMake -> Build Only -> checker" to run the tests.

_Debugging_

* Open `examples/ChiExample.cpp` and put a breakpoint on the first line under `main()` (`Database db;`).
* Choose ChiExample.exe from the dropdown menu next to the run button.
* The execution will stop at the first line, where you placed your breakpoint.
* Enjoy!

### src/

The `src/` directory contains the include and source files. There are two lists under `src/local.cmake` that are relevant to compilation. The files in the list `INCLUDE_H` are includes and the files in `SRC_CC` are compiled sources. These are then linked together to form the library `qolib`. You can append your own includes to `INCLUDE_H` and your own source files to `SRC_CC`. Make sure they are also tracked by git, otherwise they won't be pushed with your solution.

### examples/

The source files under `examples/` are all compiled to seperate target executables. They all have their own main functions and are not linked together. You can append your own source file to be compiled into an executable to the list `EXAMPLES_SRC` under `examples/local.cmake`. All these examples are linked with `qolib` and can include from the files listed in `INCLUDE_H`.

### tests/

The tests are implemented under `tests/tester.cpp`. You are allowed (and encouraged) to read and understand the tests.
