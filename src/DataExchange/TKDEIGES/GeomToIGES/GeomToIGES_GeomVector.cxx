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

// modif du 14/09/95 mjm
// taking into account the unit chosen by the user
// for writing the IGES file.

#include <Geom_Direction.hxx>
#include <Geom_Vector.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <GeomToIGES_GeomVector.hxx>
#include <gp_XYZ.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_Direction.hxx>
#include <MoniTool_Macros.hxx>

//=============================================================================
// GeomToIGES_GeomVector
//=============================================================================
GeomToIGES_GeomVector::GeomToIGES_GeomVector()
    : GeomToIGES_GeomEntity()
{
}

//=============================================================================
// GeomToIGES_GeomVector
//=============================================================================

GeomToIGES_GeomVector::GeomToIGES_GeomVector(const GeomToIGES_GeomEntity& GE)
    : GeomToIGES_GeomEntity(GE)
{
}

//=============================================================================
// Transfer of Vector Entities from Geom to IGES
// TransferVector
//=============================================================================

occ::handle<IGESGeom_Direction> GeomToIGES_GeomVector::TransferVector(const occ::handle<Geom_Vector>& start)
{
  occ::handle<IGESGeom_Direction> res;
  if (start.IsNull())
  {
    return res;
  }

  if (start->IsKind(STANDARD_TYPE(Geom_VectorWithMagnitude)))
  {
    DeclareAndCast(Geom_VectorWithMagnitude, VMagn, start);
    res = TransferVector(VMagn);
  }
  else if (start->IsKind(STANDARD_TYPE(Geom_Direction)))
  {
    DeclareAndCast(Geom_Direction, Direction, start);
    res = TransferVector(Direction);
  }

  return res;
}

//=============================================================================
// Transfer of VectorWithMagnitude Entities from Geom to IGES
// TransferVector
//=============================================================================

occ::handle<IGESGeom_Direction> GeomToIGES_GeomVector::TransferVector(
  const occ::handle<Geom_VectorWithMagnitude>& start)
{
  occ::handle<IGESGeom_Direction> Dir = new IGESGeom_Direction;
  if (start.IsNull())
  {
    return Dir;
  }

  double X, Y, Z;
  start->Coord(X, Y, Z);
  double M = start->Magnitude();
  Dir->Init(gp_XYZ(X / (M * GetUnit()), Y / (M * GetUnit()), Z / (M * GetUnit())));
  return Dir;
}

//=============================================================================
// Transfer of Direction Entities from Geom to IGES
// TransferVector
//=============================================================================

occ::handle<IGESGeom_Direction> GeomToIGES_GeomVector::TransferVector(
  const occ::handle<Geom_Direction>& start)
{
  occ::handle<IGESGeom_Direction> Dir = new IGESGeom_Direction;
  if (start.IsNull())
  {
    return Dir;
  }

  double X, Y, Z;
  start->Coord(X, Y, Z);
  Dir->Init(gp_XYZ(X / GetUnit(), Y / GetUnit(), Z / GetUnit()));
  return Dir;
}
