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

#include <Geom2dGcc.hxx>

#include <Geom2dGcc_QualifiedCurve.hxx>

Geom2dGcc_QualifiedCurve Geom2dGcc::Unqualified(const Geom2dAdaptor_Curve& Curve)
{
  return Geom2dGcc_QualifiedCurve(Curve, GccEnt_unqualified);
}

Geom2dGcc_QualifiedCurve Geom2dGcc::Enclosing(const Geom2dAdaptor_Curve& Curve)
{
  return Geom2dGcc_QualifiedCurve(Curve, GccEnt_enclosing);
}

Geom2dGcc_QualifiedCurve Geom2dGcc::Enclosed(const Geom2dAdaptor_Curve& Curve)
{
  return Geom2dGcc_QualifiedCurve(Curve, GccEnt_enclosed);
}

Geom2dGcc_QualifiedCurve Geom2dGcc::Outside(const Geom2dAdaptor_Curve& Curve)
{
  return Geom2dGcc_QualifiedCurve(Curve, GccEnt_outside);
}
