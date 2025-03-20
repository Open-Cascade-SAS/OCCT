// Created on: 1993-03-24
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

#include <Geom2d_Conic.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_Conic, Geom2d_Curve)

//=================================================================================================

gp_Ax2d Geom2d_Conic::XAxis() const
{
  return gp_Ax2d(pos.Location(), pos.XDirection());
}

//=================================================================================================

gp_Ax2d Geom2d_Conic::YAxis() const
{
  return gp_Ax2d(pos.Location(), pos.YDirection());
}

//=================================================================================================

void Geom2d_Conic::Reverse()
{
  gp_Dir2d Temp = pos.YDirection();
  Temp.Reverse();
  pos.SetAxis(gp_Ax22d(pos.Location(), pos.XDirection(), Temp));
}

//=================================================================================================

GeomAbs_Shape Geom2d_Conic::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

Standard_Boolean Geom2d_Conic::IsCN(const Standard_Integer) const
{
  return Standard_True;
}

//=================================================================================================

void Geom2d_Conic::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Curve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &pos)
}
