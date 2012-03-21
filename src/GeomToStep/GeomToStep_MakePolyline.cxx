// Created on: 1993-07-12
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


#include <GeomToStep_MakePolyline.ixx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <StepGeom_Polyline.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une polyline Step a partir d' une Array1OfPnt 
//=============================================================================

GeomToStep_MakePolyline::GeomToStep_MakePolyline( const TColgp_Array1OfPnt& P)
{
  gp_Pnt P1;
#include <GeomToStep_MakePolyline_gen.pxx>
}

//=============================================================================
// Creation d' une polyline Step a partir d' une Array1OfPnt2d
//=============================================================================

GeomToStep_MakePolyline::GeomToStep_MakePolyline( const TColgp_Array1OfPnt2d& P)
{
  gp_Pnt2d P1;
#include <GeomToStep_MakePolyline_gen.pxx>
}
//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Polyline) &
      GeomToStep_MakePolyline::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return thePolyline;
}
