#ifndef _COREGRAPHICS_CXX_H_
#define _COREGRAPHICS_CXX_H_ 1

typedef double CGFloat;

typedef struct 
{
    CGFloat width;
    CGFloat height;
} CGSize;

typedef struct 
{
    CGFloat x;
    CGFloat y;
} CGPoint;

typedef struct
{
    CGPoint origin;
    CGSize size;
} CGRect;

#endif