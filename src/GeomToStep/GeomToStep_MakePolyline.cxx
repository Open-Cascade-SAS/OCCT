// Created on: 1993-07-12
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


#include <GeomToStep_MakeCartesianPoint.hxx>
#include <GeomToStep_MakePolyline.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepGeom_Polyline.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une polyline Step a partir d' une Array1OfPnt 
//=============================================================================
GeomToStep_MakePolyline::GeomToStep_MakePolyline( const TColgp_Array1OfPnt& P)
{
  gp_Pnt P1;
#include "GeomToStep_MakePolyline_gen.pxx"
}

//=============================================================================
// Creation d' une polyline Step a partir d' une Array1OfPnt2d
//=============================================================================

GeomToStep_MakePolyline::GeomToStep_MakePolyline( const TColgp_Array1OfPnt2d& P)
{
  gp_Pnt2d P1;
#include "GeomToStep_MakePolyline_gen.pxx"
}
//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Polyline) &
      GeomToStep_MakePolyline::Value() const
{
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakePolyline::Value() - no result");
  return thePolyline;
}
