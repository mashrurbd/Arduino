
[![Arduino CI](https://github.com/RobTillaart/SparseArray/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/SparseArray/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/SparseArray/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/SparseArray/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/SparseArray/actions/workflows/jsoncheck.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/SparseArray/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/SparseArray.svg?maxAge=3600)](https://github.com/RobTillaart/SparseArray/releases)


# SparseArray

Arduino library for sparse arrays of floats.


TODO REDO DOCUMENTATION.


## Description

SparseArray is an **experimental** library to implement a one
dimensional sparse array of floats (a.k.a. vector) on an Arduino.
A sparse array is a mn array with mostly zeros and a low percentage 
non-zero values.
The purpose of this library is efficient storage in memory. 

The maximum array that can be represented is 65535 elements 
with a theoretical maximum of 65535 non-zero elements. 
(although that does not make sense due to overhead)
In practice the library limits this to 1000 non-zero elements.
Note: 255 elements would still fit in an UNO's 2K memory.

Relates to: 
- https://github.com/RobTillaart/SparseMatrix
- https://github.com/RobTillaart/distanceTable

Note: this library is derived from SparseMatrix.


#### Implementation

The implementation is based on 2 arrays holding ``` x, value``` 
where value is float, and x is an uint16_t.
That are 6 bytes per element. 
The number of elements that the sparse array object can hold are 
given as parameter to the constructor. 
If the space cannot be allocated the size is set to zero.

In the future other data types should be possible.


#### Performance

The elements are not kept sorted or indexed so optimizations might be 
possible but are not investigated yet.
There is however a test sketch to monitor the performance of
the most important functions.

Accessing elements internally is done with a linear search, 
which becomes (much) slower if the number of elements is increasing. 
This means that although in theory there can be 65535 elements, 
in practice a few 100 can already become annoyingly slow.
To keep performance a bit the library has a limit build in.
Check the .h file for **SPARSEARRAY_MAX_SIZE 1000**


## Interface

```cpp
#include "SparseArray.h"
```

### Constructor + meta

- **SparseArray(uint16_t size)** constructor. 
Parameter is the maximum number of elements in the sparse array.
Note this number is limited to **SPARSEARRAY_MAX_SIZE 1000**.
If the space requested cannot be allocated size will be set to 0.
- **uint16_t size()** maximum number of elements.
If this is zero, a problem occurred with allocation happened.
- **uint16_t count()** current number of elements in the array.
Should be between 0 and size.
- **float sum()** sum of all elements ( != 0 ) in the array.
- **void clear()** resets the array to all zero's again.


### Access

- **bool set(uint16_t x, float value)** gives an element in the array a value.
If the value is set to zero, it is removed from the internal store.
Returns false if the internal store is full, true otherwise.
- **float get(uint16_t x)** returns a value from the array. 
- **bool add(uint16_t x, float value)** adds value to an element in the array.
If needed a new internal element is created. 
If the sum is zero, the element is removed from the internal store.
Returns false if the internal store is full, true otherwise.
- **void  boundingSegment(uint16_t &minX, uint16_t &maxX)** 
Returns the bounding box in which all values != 0 are located.
This can be useful for printing or processing the non zero elements.


## Future

- documentation
- test
- keep in sync with SparseMatrix where possible
- merge into one class hierarchy?
- dump should be in the class?
  - or as static function...
  - stream as param  dump(Stream str, ...
  
#### ideas

- array { uint32_t, float }; for logging  millis/micros + measurement
  delta coding of time stamp? if it fit in 16 bit?
  => sounds like a class on its own.


