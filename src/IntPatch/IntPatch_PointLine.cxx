// Created on: 2015-02-18
// Created by: Nikolai BUKHALOV
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2015 OPEN CASCADE SAS
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


#include <IntPatch_PointLine.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IntPatch_PointLine::IntPatch_PointLine (const Standard_Boolean Tang,
                                        const IntSurf_TypeTrans Trans1,
                                        const IntSurf_TypeTrans Trans2) : 
  IntPatch_Line(Tang, Trans1, Trans2)
{}

IntPatch_PointLine::IntPatch_PointLine (const Standard_Boolean Tang,
                                        const IntSurf_Situation Situ1,
                                        const IntSurf_Situation Situ2) : 
  IntPatch_Line(Tang, Situ1, Situ2)
{}

IntPatch_PointLine::IntPatch_PointLine (const Standard_Boolean Tang) : 
  IntPatch_Line(Tang)
{}


