// Copyright (c) 1995-1999 Matra Datavision
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


#include <Geom2dGcc_QualifiedCurve.ixx>

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


