// Created on: 1993-03-04
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <Contap_Point.hxx>
#include <gp_Pnt.hxx>
#include <IntSurf_Transition.hxx>
#include <Standard_DomainError.hxx>

Contap_Point::Contap_Point ():
  onarc(Standard_False),isvtx(Standard_False),ismult(Standard_False),
  myInternal(Standard_False)
{}

Contap_Point::Contap_Point (const gp_Pnt& Pt,
                            const Standard_Real U,
                            const Standard_Real V):
pt(Pt),uparam(U),vparam(V),onarc(Standard_False),isvtx(Standard_False),
ismult(Standard_False),myInternal(Standard_False)
{}
