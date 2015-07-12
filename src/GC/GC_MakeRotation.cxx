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


#include <GC_MakeRotation.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a une     +
//   droite Line.                                                         +
//=========================================================================
GC_MakeRotation::GC_MakeRotation(const gp_Lin&  Line  ,
				   const Standard_Real     Angle ) {
  TheRotation = new Geom_Transformation();
  TheRotation->SetRotation(gp_Ax1(Line.Location(),Line.Direction()),Angle);
}

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a un      +
//   axe Axis.                                                            +
//=========================================================================

GC_MakeRotation::GC_MakeRotation(const gp_Ax1&  Axis  ,
				   const Standard_Real     Angle ) {
  TheRotation = new Geom_Transformation();
  TheRotation->SetRotation(Axis,Angle);
}

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a une     +
//   droite issue du point Point et de direction Direc.                   +
//=========================================================================

GC_MakeRotation::GC_MakeRotation(const gp_Pnt&  Point ,
				   const gp_Dir&  Direc ,
				   const Standard_Real     Angle ) {
  TheRotation = new Geom_Transformation();
  TheRotation->SetRotation(gp_Ax1(Point,Direc),Angle);
}

const Handle(Geom_Transformation)& GC_MakeRotation::Value() const
{ 
  return TheRotation;
}
