# Mandelbrot

A classic approach for generating mandelbrot set in c++.
It uses SFML library for graphics.

## Dependencies
### SFML
To install SFML on ubuntu (or a debian based linux distro), copy and paste the following to the terminal:

```sudo apt-get install libsfml-dev```

## Build and Run

To build and run (on linux):

``` g++ -c mandelbrot.cpp && g++ mandelbrot.o -o mandelbrot -lsfml-graphics -lsfml-window -lsfml-system && ./mandelbrot ```

You can replace g++ by any other c++ compiler.
