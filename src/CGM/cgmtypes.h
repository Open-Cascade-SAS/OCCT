/*
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

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
