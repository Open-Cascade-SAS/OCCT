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


#include <GccEnt_QualifiedLin.ixx>

gp_Lin2d GccEnt_QualifiedLin::
   Qualified () const { return TheQualified; }

GccEnt_Position GccEnt_QualifiedLin::
   Qualifier () const { return TheQualifier; }

Standard_Boolean GccEnt_QualifiedLin::
   IsUnqualified () const {
   if (TheQualifier == GccEnt_unqualified) { return Standard_True; }
   else { return Standard_False; }
 }

Standard_Boolean GccEnt_QualifiedLin::
   IsEnclosed () const {
   return (TheQualifier == GccEnt_enclosed);
 }


Standard_Boolean GccEnt_QualifiedLin::
   IsOutside () const {
   if (TheQualifier == GccEnt_outside) { return Standard_True; }
   else { return Standard_False; }
 }

GccEnt_QualifiedLin::
   GccEnt_QualifiedLin (const gp_Lin2d&       Qualified,
			const GccEnt_Position Qualifier) {
   TheQualified = Qualified;
   TheQualifier = Qualifier;
 }


