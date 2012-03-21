// Created on: 1993-06-16
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <GeomToStep_MakeAxis2Placement3d.ixx>
#include <StepGeom_Direction.hxx>
#include <GeomToStep_MakeDirection.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <Geom_Axis2Placement.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
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
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theAxis2Placement3d;
}
