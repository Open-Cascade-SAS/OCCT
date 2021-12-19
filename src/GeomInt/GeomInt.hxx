// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
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

#ifndef _GeomInt_HeaderFile
#define _GeomInt_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>


//! Provides intersections on between two surfaces of Geom.
//! The result are curves from Geom.
class GeomInt 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adjusts the parameter <thePar> to the range [theParMin,  theParMax]
  Standard_EXPORT static Standard_Boolean AdjustPeriodic (const Standard_Real thePar, const Standard_Real theParMin, const Standard_Real theParMax, const Standard_Real thePeriod, Standard_Real& theNewPar, Standard_Real& theOffset, const Standard_Real theEps = 0.0);

};

#endif // _GeomInt_HeaderFile
