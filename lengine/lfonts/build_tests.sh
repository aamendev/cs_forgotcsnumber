gcc -g -c laughic/math/vector3d.c laughic/*.c
g++ -c -g -I. -std=c++23 tables/cff/*.cc tables/cmap/*.cc ./font.cc ./tests.cc ./run_tests.cc;
g++ *.o -o run_tests;
rm *.o;
