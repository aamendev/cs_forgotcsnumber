#include "./newton_fractals.h"
void func_z3_n1(float x0, float y0, float* zx, float* zy)
{
    *zx = x0 * x0 * x0 - 3 * x0 * y0 * y0 - 1;
    *zy = 3 * x0 * x0 * y0 - y0 * y0 * y0;
}
void deri_z3_n1(float x0, float y0, float* derix, float* deriy)
{
    *derix = 3 * (x0 * x0 - y0 * y0);
    *deriy = 3 * 2 * x0 * y0;
}
void func_z4_n1(float x0, float y0, float * zx, float *zy)
{
 //x ^4 + x^3 yi -x^2 y^2 - xy^3i + y^4
 *zx = x0 * x0 * x0 * x0 - 6 * x0 * x0 * y0 * y0 + y0 * y0 * y0 *y0 - 1;
 *zy = 4 * (x0 * x0 * x0 * y0 - x0 * y0 * y0 * y0);
}
void deri_z4_n1(float x0, float y0, float *derix, float *deriy)
{
    func_z3_n1(x0, y0,  derix,  deriy); 
    *derix += 1;
    *derix *= 4;
    *deriy *= 4;
}
