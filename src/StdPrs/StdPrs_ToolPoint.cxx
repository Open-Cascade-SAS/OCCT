// Created on: 1993-08-24
// Created by: Jean-Louis FRENKEL
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


#include <Geom_Point.hxx>
#include <StdPrs_ToolPoint.hxx>

void StdPrs_ToolPoint::Coord (const Handle(Geom_Point)& aPoint,
			       Standard_Real& X,
			       Standard_Real& Y,
			       Standard_Real& Z) {
  aPoint->Coord(X,Y,Z);
}
