/**
* This is an implementation of the improved Perlin Noise algorithm
* given by Ken Perlin in 2002. This is the 3D implementation.
*/

#include "PerlinNoise.h"

#include <map>
#include <string>
#include <iostream>
#include <cmath>

PerlinNoise::PerlinNoise()
{
    p = new int[512];
    for (int x = 0; x < 512; ++x)
        p[x] = permutation[x % 256];
}


PerlinNoise::~PerlinNoise()
{
    delete p;
}


double PerlinNoise::fade(double t)
{
    return t * t * t * (t * (t * 6 - 15) + 10); // 6t^5 - 15t^4 + 10t^3
}


// Source: http://riven8192.blogspot.com/2010/08/calculate-perlinnoise-twice-as-fast.html
double PerlinNoise::grad(int hash, double x, double y, double z)
{
    switch (hash & 0xF)
    {
    case 0x0: return  x + y;
    case 0x1: return -x + y;
    case 0x2: return  x - y;
    case 0x3: return -x - y;
    case 0x4: return  x + z;
    case 0x5: return -x + z;
    case 0x6: return  x - z;
    case 0x7: return -x - z;
    case 0x8: return  y + z;
    case 0x9: return -y + z;
    case 0xA: return  y - z;
    case 0xB: return -y - z;
    case 0xC: return  y + x;
    case 0xD: return -y + z;
    case 0xE: return  y - x;
    case 0xF: return -y - z;
    default: return 0; // never happens
    }
}


double PerlinNoise::lerp(double t, double a, double b)
{
    return a + t * (b - a);
}


double PerlinNoise::noise(double x, double y, double z)
{
    /* Noise */
    // Bind coordinates to the range [0, 255] : 8 bit color range
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    // Convert x, y, and z coordinates into a unit cube (e.i. all sides are 1 unit long).
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    /* Fade */
    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    /* Hash */
    mapOfCube["A"] = p[X] + Y;
    mapOfCube["AA"] = p[mapOfCube["A"]] + Z;
    mapOfCube["AB"] = p[mapOfCube["A"] + 1] + Z;
    mapOfCube["B"] = p[X + 1] + Y;
    mapOfCube["BA"] = p[mapOfCube["B"]] + Z;
    mapOfCube["BB"] = p[mapOfCube["B"] + 1] + Z;


    /* Interpolate */
    double res = lerp(w, lerp(v, lerp(u, grad(p[mapOfCube["AA"]], x, y, z),
        grad(p[mapOfCube["BA"]], x - 1, y, z)),
        lerp(u, grad(p[mapOfCube["AB"]], x, y - 1, z),
            grad(p[mapOfCube["BB"]], x - 1, y - 1, z))),
        lerp(v, lerp(u, grad(p[mapOfCube["AA"] + 1], x, y, z - 1),
            grad(p[mapOfCube["BA"] + 1], x - 1, y, z - 1)),
            lerp(u, grad(p[mapOfCube["AB"] + 1], x, y - 1, z - 1),
                grad(p[mapOfCube["BB"] + 1], x - 1, y - 1, z - 1))));
    return (res + 1.0) / 2.0;
}

int main()
{
    int height = 300;
    int width = 300;

    FILE* fis;
    auto error = fopen_s(&fis, "texture.ppm", "w");
    fprintf(fis, "P6\n %d %d 255\n", height, width);

    PerlinNoise* Perlin = new PerlinNoise();

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            double x = (double)j / ((double)width);
            double y = (double)i / ((double)height);
            //double d = Perlin->noise(10 * x, 10 * y, 0.8);
            double d = Perlin->noise(100 * x, 100 * y, 0.8);
            d = d - floor(d);
            fprintf(fis, "%c%c%c", (int)floor(255 * d), (int)floor(255 * d), (int)floor(255 * d));
        }
        std::cout << i << " ";
    }

    return 0;
}