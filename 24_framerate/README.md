# Notice

The program offers 2 ways to calculate frame rate.

1. With independent time use `-DCALCULATE_WITH_DELTATIME=1 make` or just `make`. It's set by default.
2. Without independent time, use timer (`LTimer`) instead use `-DCALCULATE_WITH_DELTATIME=0 make`

Also make sure to execute `make clean` first when you want to build with different setting.

# Changes

* Added independent time for game loop
