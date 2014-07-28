#ifndef TYPE_H_
#define TYPE_H_

typedef unsigned char uchar;
typedef unsigned int uint;

     
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long int uint64;

typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;
typedef signed long long int sint64;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int int64;

/*typedef struct point
{
    uint16 x;
    uint16 y;
}point;*/

typedef struct plotpoint
{
    uint16 x;
    int32 y_min;
    int32 y_max;
}plotpoint;

#define max(x, y) ((x > y )? x : y)
#define min(x, y) ((x > y )? y : x)

#endif /*TYPE_H_*/
