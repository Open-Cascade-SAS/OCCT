// Created on: 1993-03-10
// Created by: JCV
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

#include <Geom_Conic.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Conic,Geom_Curve)

//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

void Geom_Conic::Reverse ()
{
  gp_Dir Vz = pos.Direction ();
  Vz.Reverse();
  pos.SetDirection (Vz);
}

//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_Conic::Continuity () const
{
  return GeomAbs_CN;
}

//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

gp_Ax1 Geom_Conic::XAxis () const
{
  return gp_Ax1 (pos.Location(), pos.XDirection());
}

//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

gp_Ax1 Geom_Conic::YAxis () const
{
  return gp_Ax1 (pos.Location(), pos.YDirection());
}

//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

Standard_Boolean Geom_Conic::IsCN (const Standard_Integer ) const
{
  return Standard_True;
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Geom_Conic::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, Geom_Curve)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &pos)
}
