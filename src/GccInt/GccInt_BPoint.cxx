// Created on: 1991-10-07
// Created by: Remi GILET
// Copyright (c) 1991-1999 Matra Datavision
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


#include <GccInt_BPoint.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GccInt_BPoint,GccInt_Bisec)

GccInt_BPoint::
   GccInt_BPoint(const gp_Pnt2d& Point) {
   pnt = gp_Pnt2d(Point);
 }

GccInt_IType GccInt_BPoint::
   ArcType() const {
   return GccInt_Pnt;
 }

gp_Pnt2d GccInt_BPoint::
   Point() const { return pnt; }


