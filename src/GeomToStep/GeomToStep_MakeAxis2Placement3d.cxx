// Created on: 1993-06-16
// Created by: Martine LANGLOIS
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


#include <Geom_Axis2Placement.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <GeomToStep_MakeDirection.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <TCollection_HAsciiString.hxx>

static Handle(StepGeom_Axis2Placement3d)  MakeAxis2Placement3d
  (const gp_Pnt& O, const gp_Dir& D, const gp_Dir& X, const Standard_CString nom)
{
  Handle(StepGeom_Axis2Placement3d) Axe;
  Handle(StepGeom_CartesianPoint) P;
  Handle(StepGeom_Direction) D1, D2;

  GeomToStep_MakeCartesianPoint MkPoint(O);
  GeomToStep_MakeDirection      MkDir1(D);
  GeomToStep_MakeDirection      MkDir2(X);
  
  P  = MkPoint.Value();
  D1 = MkDir1.Value();
  D2 = MkDir2.Value();

  Axe = new StepGeom_Axis2Placement3d;
  Axe->SetLocation(P);
  Axe->SetAxis(D1);
  Axe->SetRefDirection(D2);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString(nom);
  Axe->SetName(name);
  return Axe;
}

//=============================================================================
// Creation d' un axis2_placement_3d a l origine
//=============================================================================

GeomToStep_MakeAxis2Placement3d::GeomToStep_MakeAxis2Placement3d ( )
{
  gp_Ax2 A (gp_Pnt(0.,0.,0.), gp_Dir (0.,0.,1.), gp_Dir (1.,0.,0.));
//   le reste inchange

  Handle(StepGeom_Axis2Placement3d) Axe = MakeAxis2Placement3d
    (A.Location(), A.Direction(), A.XDirection(), "");
  theAxis2Placement3d = Axe;
  done = Standard_True;
}

//=============================================================================
// Creation d' un axis2_placement_3d de prostep a partir d' un Ax2 de gp
//=============================================================================

GeomToStep_MakeAxis2Placement3d::GeomToStep_MakeAxis2Placement3d( const gp_Ax2&
							       A)
{
  Handle(StepGeom_Axis2Placement3d) Axe = MakeAxis2Placement3d
    (A.Location(), A.Direction(), A.XDirection(), "");
  theAxis2Placement3d = Axe;
  done = Standard_True;
}

//=============================================================================
// Creation d' un axis2_placement_3d de prostep a partir d' un Ax3 de gp
//=============================================================================

GeomToStep_MakeAxis2Placement3d::GeomToStep_MakeAxis2Placement3d
( const gp_Ax3&	 A)
{
  Handle(StepGeom_Axis2Placement3d) Axe = MakeAxis2Placement3d
    (A.Location(), A.Direction(), A.XDirection(), "");
  theAxis2Placement3d = Axe;
  done = Standard_True;
}

//=============================================================================
// Creation d' un axis2_placement_3d de prostep a partir d' un Trsf de gp
//=============================================================================

GeomToStep_MakeAxis2Placement3d::GeomToStep_MakeAxis2Placement3d
  ( const gp_Trsf& T)
{
  gp_Ax2 A (gp_Pnt(0.,0.,0.), gp_Dir (0.,0.,1.), gp_Dir (1.,0.,0.));
  A.Transform (T);
//   le reste inchange

  Handle(StepGeom_Axis2Placement3d) Axe = MakeAxis2Placement3d
    (A.Location(), A.Direction(), A.XDirection(), "");
  theAxis2Placement3d = Axe;
  done = Standard_True;
}

//=============================================================================
// Creation d' un axis2_placement_3d de prostep a partir d' un Axis2Placement
// de Geom
//=============================================================================

GeomToStep_MakeAxis2Placement3d::GeomToStep_MakeAxis2Placement3d
  ( const Handle(Geom_Axis2Placement)& Axis2)
{
  gp_Ax2 A;
  A = Axis2->Ax2();

  Handle(StepGeom_Axis2Placement3d) Axe = MakeAxis2Placement3d
    (A.Location(), A.Direction(), A.XDirection(), "");
  theAxis2Placement3d = Axe;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Axis2Placement3d) &
      GeomToStep_MakeAxis2Placement3d::Value() const
{
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeAxis2Placement3d::Value() - no result");
  return theAxis2Placement3d;
}
