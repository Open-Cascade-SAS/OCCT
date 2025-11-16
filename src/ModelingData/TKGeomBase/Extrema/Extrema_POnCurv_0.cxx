// Created on: 1991-02-26
// Created by: Isabelle GRIGNON
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

#include <Extrema_POnCurv.hxx>

#include <gp_Pnt.hxx>

Extrema_POnCurv::Extrema_POnCurv()
    : myU(0.0)
{
}

Extrema_POnCurv::Extrema_POnCurv(const Standard_Real U, const gp_Pnt& P)
{
  myU = U;
  myP = P;
}

void Extrema_POnCurv::SetValues(const Standard_Real U, const gp_Pnt& P)
{
  myU = U;
  myP = P;
}
