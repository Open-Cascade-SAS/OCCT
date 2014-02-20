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

#ifndef _Select3D_Pnt2d_HeaderFile
#define _Select3D_Pnt2d_HeaderFile

#include<gp_Pnt2d.hxx>
#include<Standard_ShortReal.hxx>
#include<Select3D_Macro.hxx>

struct Select3D_Pnt2d{

 Standard_ShortReal x, y;

 inline operator gp_Pnt2d() const
 {
   return gp_Pnt2d(x, y);
 }

 inline operator gp_XY() const
 {
  return gp_XY(x, y); 
 }

 inline gp_Pnt2d operator = (const gp_Pnt2d& thePnt)
 {
   x = DToF(thePnt.X());
   y = DToF(thePnt.Y());
   return *this;
 }


};

#endif

