Prereq:
- Install QT on your system. Instructions to do so on QT website

To compile:
- Navigate to source directory
- call the following...
- cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="PATH_TO_QT/6.10.2/NAME_OF_FOLDER"
- cmake --build build

To run:
- ./build/debug/CMusic
