// Created on: 1992-10-02
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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


#include <GCE2d_MakeRotation.hxx>
#include <Geom2d_Transformation.hxx>
#include <gp_Pnt2d.hxx>

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a une     +
//   droite Line.                                                         +
//=========================================================================
GCE2d_MakeRotation::GCE2d_MakeRotation(const gp_Pnt2d&     Point  ,
				       const Standard_Real Angle ) {
  TheRotation = new Geom2d_Transformation();
  TheRotation->SetRotation(Point,Angle);
}

const Handle(Geom2d_Transformation)& GCE2d_MakeRotation::Value() const
{ 
  return TheRotation;
}
