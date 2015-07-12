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


#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>

Geom2dGcc_QualifiedCurve::
   Geom2dGcc_QualifiedCurve (const Geom2dAdaptor_Curve& Curve    ,
			     const GccEnt_Position      Qualifier) {
   TheQualified = Curve;
   TheQualifier = Qualifier;
 }

Geom2dAdaptor_Curve Geom2dGcc_QualifiedCurve::
   Qualified () const { return TheQualified; }

GccEnt_Position Geom2dGcc_QualifiedCurve::
   Qualifier () const { return TheQualifier; }

Standard_Boolean Geom2dGcc_QualifiedCurve::
   IsUnqualified () const {
     if (TheQualifier == GccEnt_unqualified ) { return Standard_True; }
     else { return Standard_False; }
   }

Standard_Boolean Geom2dGcc_QualifiedCurve::
   IsEnclosing () const {
     if (TheQualifier == GccEnt_enclosing) { return Standard_True; }
     else { return Standard_False; }
   }

Standard_Boolean Geom2dGcc_QualifiedCurve::
   IsEnclosed () const {
     if (TheQualifier == GccEnt_enclosed) { return Standard_True; }
     else { return Standard_False; }
   }

Standard_Boolean Geom2dGcc_QualifiedCurve::
   IsOutside () const {
     if (TheQualifier == GccEnt_outside) { return Standard_True; }
     else { return Standard_False; }
   }


