//
//    FILE: ellipse.cpp
//  AUTHOR: Rob Tillaart
//    DATE: 2021-10-31
// VERSION: 0.1.1
// PURPOSE: Arduino library for ellipse maths
//     URL: https://github.com/RobTillaart/ellipse
// TRIGGER: https://www.youtube.com/watch?v=5nW3nJhBHL0
//
// HISTORY:
//  0.1.0  2021-10-31  initial version
//  0.1.1  2022-07-    add angle() + example
//                     add isCircle(), isFlat()
//                     update readme.md


#include "ellipse.h"


ellipse::ellipse(float a, float b)
{
  _a = abs(a);
  _b = abs(b);
}


//////////////////////////////////////////////////////////////////
// 
// unknown - see youtube link.
// p = 2 * PI * sqrt((a*a + b*b)/2);
//
float ellipse::circumference()
{
  return perimeter_Ramanujan1();
}


float ellipse::perimeter_ref()
{
  return perimeter_Ramanujan2();
}


float ellipse::perimeter_Keppler()
{
  // Keppler 
  float p = 2 * PI * (_a + _b) / 2;   // very fast for a ~ b
  return p;
}


float ellipse::perimeter_Ramanujan1()
{
  // Srinivasa Ramanujan I - very good
  // float p = PI * (3 * (_a + _b) - sqrt((3 * _a +_b)*(_a + 3 * _b)));
  float a3 = 3 * _a;
  float b3 = 3 * _b;
  float p = PI * (a3 + b3 - sqrt( (a3 + _b)*(_a + b3))); 
  // one float operation less (~7% faster)
  return p;
}


float ellipse::perimeter_Ramanujan2()
{
  // Srinivasa Ramanujan II - extremely good
  float x = _a - _b;
  float y = _a + _b;
  float h3 = 3 * (x * x) / (y * y);
  float p = PI * y * ( 1 + h3 / (10 + sqrt(4 - h3)));
  return p;
}


//////////////////////////////////////////////////////////

float ellipse::area()
{
  return PI * _a * _b;
}


float ellipse::eccentricity()
{
  if (_a == _b) return 0;  //  quick circle check.
  float x = _a * _a - _b * _b;
  if (x < 0) x = -1 * x;
  return sqrt(x)/ _a;
}


bool ellipse::isCircle(float epsilon)
{
  if (epsilon == 0) return (_a == _b);
  float delta = abs(_a - _b);
  return (delta < epsilon);
}


bool ellipse::isFlat()
{
  if (_a > _b) return (_a > (4 * _b));
  return (_b > (4 * _a));
}


float ellipse::getC()
{
  float e = eccentricity();
  if (_a > _b) return e * _a;
  return e * _b;
}


float ellipse::angle()
{
  float c = (_b < _a) ? _b/_a : _a/_b;
  return acos(c) * (180 / PI);
}

// -- END OF FILE --

