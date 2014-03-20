// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _BRepExtrema_Poly_HeaderFile
#define _BRepExtrema_Poly_HeaderFile

#include <Standard.hxx>

class TopoDS_Shape;
class gp_Pnt;

class BRepExtrema_Poly
{
 public:

  //! returns Standard_True if OK.
  Standard_EXPORT static Standard_Boolean Distance(const TopoDS_Shape& S1,const TopoDS_Shape& S2,gp_Pnt& P1,gp_Pnt& P2,Standard_Real& dist);
};

#endif
