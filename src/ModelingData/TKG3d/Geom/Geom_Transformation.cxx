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

#include <Geom_Transformation.hxx>

#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Transformation, Standard_Transient)

Geom_Transformation::Geom_Transformation() {}

Geom_Transformation::Geom_Transformation(const gp_Trsf& T)
    : gpTrsf(T)
{
}

occ::handle<Geom_Transformation> Geom_Transformation::Copy() const
{

  occ::handle<Geom_Transformation> T;
  T = new Geom_Transformation(gpTrsf);
  return T;
}

occ::handle<Geom_Transformation> Geom_Transformation::Inverted() const
{

  return new Geom_Transformation(gpTrsf.Inverted());
}

occ::handle<Geom_Transformation> Geom_Transformation::Multiplied(
  const occ::handle<Geom_Transformation>& Other) const
{

  return new Geom_Transformation(gpTrsf.Multiplied(Other->Trsf()));
}

occ::handle<Geom_Transformation> Geom_Transformation::Powered(const int N) const
{

  gp_Trsf T = gpTrsf;
  T.Power(N);
  return new Geom_Transformation(T);
}

void Geom_Transformation::PreMultiply(const occ::handle<Geom_Transformation>& Other)
{

  gpTrsf.PreMultiply(Other->Trsf());
}

void Geom_Transformation::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &gpTrsf)
}
