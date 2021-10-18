// Created on: 1992-08-18
// Created by: Modelistation
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Hatch_Line_HeaderFile
#define _Hatch_Line_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Lin2d.hxx>
#include <Hatch_LineForm.hxx>
#include <Hatch_SequenceOfParameter.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>


//! Stores a Line in the Hatcher. Represented by :
//!
//! * A Lin2d from gp, the geometry of the line.
//!
//! * Bounding parameters for the line.
//!
//! * A sorted List  of Parameters, the  intersections
//! on the line.
class Hatch_Line 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Hatch_Line();
  
  Standard_EXPORT Hatch_Line(const gp_Lin2d& L, const Hatch_LineForm T);
  
  //! Insert a new intersection in the sorted list.
  Standard_EXPORT void AddIntersection (const Standard_Real Par1, const Standard_Boolean Start, const Standard_Integer Index, const Standard_Real Par2, const Standard_Real theToler);


friend class Hatch_Hatcher;


protected:





private:



  gp_Lin2d myLin;
  Hatch_LineForm myForm;
  Hatch_SequenceOfParameter myInters;


};







#endif // _Hatch_Line_HeaderFile
