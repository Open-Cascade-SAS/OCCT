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


#include <GccEnt_QualifiedLin.hxx>
#include <gp_Lin2d.hxx>

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


