// Created on: 1993-06-17
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


#include <GeomToStep_MakeVector.ixx>
#include <StdFail_NotDone.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec.hxx>
#include <Geom_Vector.hxx>
#include <StepGeom_Vector.hxx>
#include <GeomToStep_MakeDirection.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un vector de prostep a partir d' un Vec de gp
//=============================================================================

GeomToStep_MakeVector::GeomToStep_MakeVector( const gp_Vec& V)
{
  gp_Dir D = gp_Dir(V);
  Standard_Real lFactor = UnitsMethods::LengthFactor();
#include <GeomToStep_MakeVector_gen.pxx>
}
//=============================================================================
// Creation d' un vector de prostep a partir d' un Vec2d de gp
//=============================================================================

GeomToStep_MakeVector::GeomToStep_MakeVector( const gp_Vec2d& V)
{
  gp_Dir2d D = gp_Dir2d(V);
  Standard_Real lFactor = 1.;
#include <GeomToStep_MakeVector_gen.pxx>
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
#include <GeomToStep_MakeVector_gen.pxx>
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
#include <GeomToStep_MakeVector_gen.pxx>
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Vector) &
      GeomToStep_MakeVector::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theVector;
}
