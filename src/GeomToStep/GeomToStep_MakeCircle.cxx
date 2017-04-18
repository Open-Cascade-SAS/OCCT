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


#include <Geom2d_Circle.hxx>
#include <Geom_Circle.hxx>
#include <GeomToStep_MakeAxis2Placement2d.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <GeomToStep_MakeCircle.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_Circle.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un cercle de prostep a partir d' un cercle 3d de gp
//=============================================================================
GeomToStep_MakeCircle::GeomToStep_MakeCircle( const gp_Circ& C)
{
#include "GeomToStep_MakeCircle_gen.pxx"
}


//=============================================================================
// Creation d' un cercle de prostep a partir d' un cercle de
// Geom
//=============================================================================

GeomToStep_MakeCircle::GeomToStep_MakeCircle( const Handle(Geom_Circle)& Cer)
{
  gp_Circ C;
  C = Cer->Circ();
#include "GeomToStep_MakeCircle_gen.pxx"
}


//=============================================================================
// Creation d' un cercle 2d de prostep a partir d' un cercle de
// Geom2d
//=============================================================================

GeomToStep_MakeCircle::GeomToStep_MakeCircle( const Handle(Geom2d_Circle)& Cer)
{
  gp_Circ2d C2d;
  C2d = Cer->Circ2d();

  Handle(StepGeom_Circle) CStep = new StepGeom_Circle;
  StepGeom_Axis2Placement Ax2;
  Handle(StepGeom_Axis2Placement2d) Ax2Step;
  Standard_Real Rayon;
  
  GeomToStep_MakeAxis2Placement2d MkAxis2(C2d.Position());
  Ax2Step = MkAxis2.Value();
  Rayon = C2d.Radius();
  Ax2.SetValue(Ax2Step);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  CStep->Init(name, Ax2, Rayon);
  theCircle = CStep;
  done = Standard_True;

}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Circle) &
      GeomToStep_MakeCircle::Value() const
{
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeCircle::Value() - no result");
  return theCircle;
}
