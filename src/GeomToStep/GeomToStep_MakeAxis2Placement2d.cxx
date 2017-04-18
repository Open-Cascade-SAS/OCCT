// Created on: 1994-08-26
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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


#include <GeomToStep_MakeAxis2Placement2d.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <GeomToStep_MakeDirection.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax22d.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' un axis2_placement_2d de prostep a partir d' un Ax2 de gp
//=============================================================================
GeomToStep_MakeAxis2Placement2d::GeomToStep_MakeAxis2Placement2d
( const gp_Ax2& A)
{
  Handle(StepGeom_Axis2Placement2d) Axe;
  Handle(StepGeom_CartesianPoint) P;
  Handle(StepGeom_Direction) D;

  GeomToStep_MakeCartesianPoint MkPoint(A.Location());
  GeomToStep_MakeDirection      MkDir(A.Direction());

  P = MkPoint.Value();
  D = MkDir.Value();

  Axe = new StepGeom_Axis2Placement2d;
  Axe->SetLocation(P);
  Axe->SetRefDirection(D);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Axe->SetName(name);
  theAxis2Placement2d = Axe;
  done = Standard_True;
}

//=============================================================================
// Creation d' un axis2_placement_2d de prostep a partir d' un Ax22d de gp
//=============================================================================

GeomToStep_MakeAxis2Placement2d::GeomToStep_MakeAxis2Placement2d
( const gp_Ax22d& A)
{
  Handle(StepGeom_Axis2Placement2d) Axe;
  Handle(StepGeom_CartesianPoint) P;
  Handle(StepGeom_Direction) D1;

  GeomToStep_MakeCartesianPoint MkPoint(A.Location());
  GeomToStep_MakeDirection      MkDir(A.XDirection());

  P  = MkPoint.Value();
  D1 = MkDir.Value();

  Axe = new StepGeom_Axis2Placement2d;
  Axe->SetLocation(P);
  Axe->SetRefDirection(D1);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Axe->SetName(name);
  theAxis2Placement2d = Axe;
  done = Standard_True;
}

//=============================================================================
// Creation d' un axis2_placement_2d de prostep a partir d' un Axis2Placement
// de Geom
//=============================================================================

//GeomToStep_MakeAxis2Placement2d::GeomToStep_MakeAxis2Placement2d
//  ( const Handle(Geom_Axis2Placement)& Axis2)
//{
//  Handle(StepGeom_Axis2Placement2d) Axe;
//  Handle(StepGeom_CartesianPoint) P;
//  Handle(StepGeom_Direction) D1, D2;
//  gp_Ax2 A;
  
//  A = Axis2->Ax2();
//  P = GeomToStep_MakeCartesianPoint(A.Location());
//  D1 = GeomToStep_MakeDirection(A.Direction());
//  D2 = GeomToStep_MakeDirection(A.XDirection());
//  Axe = new StepGeom_Axis2Placement2d;
//  Axe->SetLocation(P);
//  Axe->SetAxis(D1);  
//  Axe->SetRefDirection(D2);
//  theAxis2Placement2d = Axe;
//  done = Standard_True;
//}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Axis2Placement2d) &
      GeomToStep_MakeAxis2Placement2d::Value() const
{
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeAxis2Placement2d::Value() - no result");
  return theAxis2Placement2d;
}
