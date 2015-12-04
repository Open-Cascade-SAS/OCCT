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


#include <GccInt_BCirc.hxx>
#include <gp_Circ2d.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GccInt_BCirc,GccInt_Bisec)

GccInt_BCirc::
   GccInt_BCirc(const gp_Circ2d& Circ) {
   cir = gp_Circ2d(Circ);
 }

GccInt_IType GccInt_BCirc::
   ArcType() const {
   return GccInt_Cir;
 }

gp_Circ2d GccInt_BCirc::
  Circle() const { return cir; }

