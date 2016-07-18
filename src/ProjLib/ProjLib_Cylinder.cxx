// Created on: 1993-08-24
// Created by: Bruno DUMORTIER
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


#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pln.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <ProjLib_Cylinder.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>

//=======================================================================
//function : ProjLib_Cylinder
//purpose  : 
//=======================================================================
ProjLib_Cylinder::ProjLib_Cylinder()
{
}


//=======================================================================
//function : ProjLib_Cylinder
//purpose  : 
//=======================================================================

ProjLib_Cylinder::ProjLib_Cylinder(const gp_Cylinder& Cyl)
{
  Init(Cyl);
}


//=======================================================================
//function : ProjLib_Cylinder
//purpose  : 
//=======================================================================

ProjLib_Cylinder::ProjLib_Cylinder(const gp_Cylinder& Cyl, const gp_Lin& L)
{
  Init(Cyl);
  Project(L);
}


//=======================================================================
//function : ProjLib_Cylinder
//purpose  : 
//=======================================================================

ProjLib_Cylinder::ProjLib_Cylinder(const gp_Cylinder& Cyl, const gp_Circ& C)
{
  Init(Cyl);
  Project(C);
}


//=======================================================================
//function : ProjLib_Cylinder
//purpose  : 
//=======================================================================

ProjLib_Cylinder::ProjLib_Cylinder(const gp_Cylinder& Cyl, const gp_Elips& E)
{
  Init(Cyl);
  Project(E);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  ProjLib_Cylinder::Init(const gp_Cylinder& Cyl)
{
  myType = GeomAbs_OtherCurve;
  myCylinder = Cyl;
  myIsPeriodic = Standard_False;
  isDone = Standard_False;
}


//=======================================================================
//function : EvalPnt2d / EvalDir2d
//purpose  : returns the Projected Pnt / Dir in the parametrization range
//           of myPlane.
//=======================================================================

static gp_Pnt2d EvalPnt2d( const gp_Pnt& P, const gp_Cylinder& Cy )
{
  gp_Vec OP( Cy.Location(),P);
  Standard_Real X = OP.Dot(gp_Vec(Cy.Position().XDirection()));
  Standard_Real Y = OP.Dot(gp_Vec(Cy.Position().YDirection()));
  Standard_Real Z = OP.Dot(gp_Vec(Cy.Position().Direction()));
  Standard_Real U ;

  if ( Abs(X) > Precision::PConfusion() ||
       Abs(Y) > Precision::PConfusion() ) {
    U = ATan2(Y,X);
  }
  else {
    U = 0.;
  }
  return gp_Pnt2d( U, Z);
}



//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void  ProjLib_Cylinder::Project(const gp_Lin& L)
{
  // Check the line is parallel to the axis of cylinder.
  // In other cases, the projection is wrong.
  if (L.Direction().XYZ().CrossSquareMagnitude(myCylinder.Position().Direction().XYZ()) >
      Precision::Angular() * Precision::Angular())
    return;

  myType = GeomAbs_Line;

  gp_Pnt2d P2d = EvalPnt2d(L.Location(),myCylinder);
  if ( P2d.X() < 0.) {
    P2d.SetX(P2d.X()+2*M_PI);
  }
  Standard_Real Signe 
    = L.Direction().Dot(myCylinder.Position().Direction());
  Signe = (Signe > 0.) ? 1. : -1.;
  gp_Dir2d D2d(0., Signe);
  
  myLin = gp_Lin2d( P2d, D2d);
  isDone = Standard_True;
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void  ProjLib_Cylinder::Project(const gp_Circ& C)
{
  // Check the circle's normal is parallel to the axis of cylinder.
  // In other cases, the projection is wrong.
  const gp_Ax3& aCylPos = myCylinder.Position();
  const gp_Ax2& aCircPos = C.Position();
  if (aCylPos.Direction().XYZ().CrossSquareMagnitude(aCircPos.Direction().XYZ()) >
      Precision::Angular() * Precision::Angular())
    return;

  myType = GeomAbs_Line;

  gp_Dir ZCyl = aCylPos.XDirection().Crossed(aCylPos.YDirection());

  Standard_Real U = aCylPos.XDirection().AngleWithRef(aCircPos.XDirection(), ZCyl);

  gp_Vec OP( myCylinder.Location(),C.Location());
  Standard_Real V = OP.Dot(gp_Vec(aCylPos.Direction()));

  gp_Pnt2d P2d1 (U, V);
  gp_Dir2d D2d;
  if ( ZCyl.Dot(aCircPos.Direction()) > 0.) 
    D2d.SetCoord(1., 0.);
  else
    D2d.SetCoord(-1., 0.);

  myLin = gp_Lin2d(P2d1, D2d);
  isDone = Standard_True;
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

//void  ProjLib_Cylinder::Project(const gp_Elips& E)
void  ProjLib_Cylinder::Project(const gp_Elips& )
{
  // Pour de vastes raisons de periodicite mal gerees,
  // la projection d`une ellipse sur un cylindre sera passee aux approx.
  
  
}

void  ProjLib_Cylinder::Project(const gp_Parab& P)
{
 ProjLib_Projector::Project(P);
}

void  ProjLib_Cylinder::Project(const gp_Hypr& H)
{
 ProjLib_Projector::Project(H);
}

