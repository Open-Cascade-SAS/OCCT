/*This file is used to define all attrib structures of the library used to
generate CGM files*/

#ifndef CGMATT_H
#define CGMATT_H
#ifdef CGMATT_C
#define Extern
#else
#define Extern extern
#endif

 
#include "cgmtypes.h"

struct sharedatt {
     Index       auxindex;
     RGBcolour   aux;
     Enum        colmode,
                 transparency;
     float       xgrain,
                 ygrain;
};

typedef struct sharedatt Sharedatt;

struct lineatt {
     Index       type;
     float       width;
     Enum        widthmode;
     Index       colindex;
     RGBcolour   col;
     Sharedatt  *shared;
};

typedef struct lineatt Lineatt;

struct markeratt {
     Index       type;
     float       size;
     Enum        sizemode;
     Index       colindex;
     RGBcolour   col;
     Sharedatt  *shared;
};

typedef struct markeratt Markeratt;

struct textatt {
     int         fontcount;             /*  environmental variables  */
     char      **fontlist;
     int         csetcount;
     char      **csetlist;
     Enum        csetannouncer;

     Index       fontindex;             /*  bundleable attributes  */
     Enum        precision;
     float       expansion;
     float       spacing;
     Index       colindex;
     RGBcolour   col;

     float       height;                /*  device-independent variables  */
     float       xup,
                 yup,
                 xbase,
                 ybase;
     Enum        path;
     Enum        halign,
                 valign;
     float       hcont,
                 vcont;
     Index       csindex,
                 acsindex;
     Sharedatt  *shared;
};

typedef struct textatt Textatt;

struct fillatt {
     Index       style;
     Index       colindex;
     RGBcolour   col;
     Index       hatch,
                 pattern;
     Sharedatt  *shared;
};

typedef struct fillatt Fillatt;

struct edgeatt {
     Index       type;
     float       width;
     Enum        widthmode;
     Index       colindex;
     RGBcolour   col;
     Sharedatt  *shared;
};

typedef struct edgeatt Edgeatt;

struct linebundle {
     Index   type;
     float   width;
     Index   colindex;
     Colourentry col;
};

typedef struct linebundle Linebundle;

struct markerbundle {
     Index   type;
     float   size;
     Index   colindex;
     Colourentry col;
};

typedef struct markerbundle Markerbundle;

struct textbundle {
     Index   font;
     Enum    precision;
     float   expansion;
     float   spacing;
     Index   colindex;
     Colourentry col;
};

typedef struct textbundle Textbundle;

struct fillbundle {
     Index   style;
     Index   pattern;
     Index   hatch;
     Index   colindex;
     Colourentry col;
};

typedef struct fillbundle Fillbundle;

struct edgebundle {
     Index   type;
     float   width;
     Index   colindex;
     Colourentry col;
};

typedef struct edgebundle Edgebundle;

Extern void ATTline();
Extern void ATTmarker();
Extern void ATTtext();
Extern void ATTfill();
Extern void ATTedge();

#undef Extern

#endif   /*  end of cgmatt.h */
