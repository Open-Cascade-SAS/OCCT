// File:	Draw_PInterp.hxx
// Created:	Thu Feb 23 17:31:31 1995
// Author:	Remi LEQUETTE
//		<rle@bravox>


#ifndef _Draw_PInterp_HeaderFile
#define _Draw_PInterp_HeaderFile

struct Tcl_Interp;

typedef Tcl_Interp *Draw_PInterp;


#include <Standard_Type.hxx>
inline const Handle(Standard_Type)& STANDARD_TYPE(Draw_PInterp) {return *((Handle(Standard_Type)*)0);}

#endif
