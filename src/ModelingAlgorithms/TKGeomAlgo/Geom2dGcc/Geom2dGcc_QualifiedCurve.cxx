// Copyright (c) 1995-1999 Matra Datavision
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

#include <Geom2dGcc_QualifiedCurve.hxx>

Geom2dGcc_QualifiedCurve::Geom2dGcc_QualifiedCurve(const Geom2dAdaptor_Curve& Curve,
                                                   const GccEnt_Position      Qualifier)
{
  TheQualified = Curve;
  TheQualifier = Qualifier;
}

Geom2dAdaptor_Curve Geom2dGcc_QualifiedCurve::Qualified() const
{
  return TheQualified;
}

GccEnt_Position Geom2dGcc_QualifiedCurve::Qualifier() const
{
  return TheQualifier;
}

bool Geom2dGcc_QualifiedCurve::IsUnqualified() const
{
  return TheQualifier == GccEnt_unqualified;
}

bool Geom2dGcc_QualifiedCurve::IsEnclosing() const
{
  return TheQualifier == GccEnt_enclosing;
}

bool Geom2dGcc_QualifiedCurve::IsEnclosed() const
{
  return TheQualifier == GccEnt_enclosed;
}

bool Geom2dGcc_QualifiedCurve::IsOutside() const
{
  return TheQualifier == GccEnt_outside;
}
