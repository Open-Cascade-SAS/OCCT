#ifndef CGMTYPES_H
#define CGMTYPES_H

     /*  Include the only file on which this is dependent.  */

#include "cgmmach.h"

/*  CGM Typedefs  */

typedef short Logical;
typedef long Index;
typedef unsigned short Enum;
typedef Int16 Code;
typedef long Prec;
typedef unsigned long Posint;
typedef float Ptype;
typedef double Mtype;

/*  Structures used by CGM user interface */

typedef struct { float x, y; } Point;
typedef struct { float x, y; } Vector;
typedef struct { Posint red, green, blue; } RGBcolour;
struct colour {
   Index index;
   Posint red, green, blue;
};
typedef struct colour Colour;

typedef Index Indexcolour;

/*  Useful Structures for CGM routines */

struct vdc {
   long intr;
   float real;
};
typedef struct vdc Vdc;

struct colourentry {
   float red, green, blue;
};
typedef struct colourentry Colourentry;

typedef double Tmatrix[3][2];

struct point {
 struct vdc x, y;
};
typedef struct point Cpoint;

struct rect {
  struct point a, b;
};

typedef struct { float left, right, top, bot; } Rect;


#ifndef TRUE
#define TRUE    ((Logical) 1)
#endif

#ifndef FALSE
#define FALSE   ((Logical) 0)
#endif

#endif  /*  end of cgmtypes.h  */
