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


#include <GeomToStep_MakeLine.ixx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Vector.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <GeomToStep_MakeLine.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <GeomToStep_MakeVector.hxx>
#include <TCollection_HAsciiString.hxx>


//=============================================================================
// Creation d' une line de prostep a partir d' une Lin de gp
//=============================================================================

GeomToStep_MakeLine::GeomToStep_MakeLine( const gp_Lin& L)
{
#define Vec_gen gp_Vec
#include <GeomToStep_MakeLine_gen.pxx>
#undef Vec_gen
}

//=============================================================================
// Creation d' une line de prostep a partir d' une Lin2d de gp
//=============================================================================

GeomToStep_MakeLine::GeomToStep_MakeLine( const gp_Lin2d& L)
{
#define Vec_gen gp_Vec2d
#include <GeomToStep_MakeLine_gen.pxx>
#undef Vec_gen
}

//=============================================================================
// Creation d' une line de prostep a partir d' une Line de Geom
//=============================================================================

GeomToStep_MakeLine::GeomToStep_MakeLine ( const Handle(Geom_Line)& Gline)
{
  gp_Lin L;
  L = Gline->Lin();
#define Vec_gen gp_Vec
#include <GeomToStep_MakeLine_gen.pxx>
#undef Vec_gen
}

//=============================================================================
// Creation d' une line de prostep a partir d' une Line de Geom2d
//=============================================================================

GeomToStep_MakeLine::GeomToStep_MakeLine ( const Handle(Geom2d_Line)& Gline)
{
  gp_Lin2d L;
  L = Gline->Lin2d();
#define Vec_gen gp_Vec2d
#include <GeomToStep_MakeLine_gen.pxx>
#undef Vec_gen
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Line) &
      GeomToStep_MakeLine::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theLine;
}

