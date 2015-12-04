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


#include <GccInt_BHyper.hxx>
#include <gp_Hypr2d.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GccInt_BHyper,GccInt_Bisec)

GccInt_BHyper::
   GccInt_BHyper(const gp_Hypr2d& Hyper) {
   hyp = gp_Hypr2d(Hyper);
 }

GccInt_IType GccInt_BHyper::
   ArcType() const {
   return GccInt_Hpr;
 }

gp_Hypr2d GccInt_BHyper::
  Hyperbola() const { return hyp; }


