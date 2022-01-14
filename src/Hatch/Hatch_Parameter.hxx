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

#ifndef _Hatch_Parameter_HeaderFile
#define _Hatch_Parameter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>


//! Stores an intersection on a line represented by :
//!
//! * A Real parameter.
//!
//! * A flag True when the parameter starts an interval.
class Hatch_Parameter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Hatch_Parameter();
  
  Standard_EXPORT Hatch_Parameter(const Standard_Real Par1, const Standard_Boolean Start, const Standard_Integer Index = 0, const Standard_Real Par2 = 0);


friend class Hatch_Line;
friend class Hatch_Hatcher;


protected:





private:



  Standard_Real myPar1;
  Standard_Boolean myStart;
  Standard_Integer myIndex;
  Standard_Real myPar2;


};







#endif // _Hatch_Parameter_HeaderFile
