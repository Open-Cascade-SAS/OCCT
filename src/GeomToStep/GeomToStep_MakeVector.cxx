// Created on: 1993-06-17
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


#include <Geom2d_Vector.hxx>
#include <Geom_Vector.hxx>
#include <GeomToStep_MakeDirection.hxx>
#include <GeomToStep_MakeVector.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Vector.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un vector de prostep a partir d' un Vec de gp
//=============================================================================
GeomToStep_MakeVector::GeomToStep_MakeVector( const gp_Vec& V)
{
  gp_Dir D = gp_Dir(V);
  Standard_Real lFactor = UnitsMethods::LengthFactor();
#include "GeomToStep_MakeVector_gen.pxx"
}
//=============================================================================
// Creation d' un vector de prostep a partir d' un Vec2d de gp
//=============================================================================

GeomToStep_MakeVector::GeomToStep_MakeVector( const gp_Vec2d& V)
{
  gp_Dir2d D = gp_Dir2d(V);
  Standard_Real lFactor = 1.;
#include "GeomToStep_MakeVector_gen.pxx"
}

//=============================================================================
// Creation d' un vector de prostep a partir d' un Vector de Geom
//=============================================================================

GeomToStep_MakeVector::GeomToStep_MakeVector ( const Handle(Geom_Vector)&
					    GVector)
{
  gp_Vec V;
  V = GVector->Vec();
  gp_Dir D = gp_Dir(V);
  Standard_Real lFactor = UnitsMethods::LengthFactor();
#include "GeomToStep_MakeVector_gen.pxx"
}

//=============================================================================
// Creation d' un vector de prostep a partir d' un Vector de Geom2d
//=============================================================================

GeomToStep_MakeVector::GeomToStep_MakeVector ( const Handle(Geom2d_Vector)&
					    GVector)
{
  gp_Vec2d V;
  V = GVector->Vec2d();
  gp_Dir2d D = gp_Dir2d(V);
  Standard_Real lFactor = 1.;
#include "GeomToStep_MakeVector_gen.pxx"
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Vector) &
      GeomToStep_MakeVector::Value() const
{
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeVector::Value() - no result");
  return theVector;
}
