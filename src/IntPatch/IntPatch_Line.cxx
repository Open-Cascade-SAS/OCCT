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


#include <IntPatch_Line.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntPatch_Line,Standard_Transient)

IntPatch_Line::IntPatch_Line (const Standard_Boolean Tang,
			      const IntSurf_TypeTrans Trans1,
			      const IntSurf_TypeTrans Trans2):
       tg(Tang),
       tS1(Trans1),tS2(Trans2),
       sit1(IntSurf_Unknown),sit2(IntSurf_Unknown),
       uS1(Standard_False),vS1(Standard_False),
       uS2(Standard_False),vS2(Standard_False)
{}

IntPatch_Line::IntPatch_Line (const Standard_Boolean Tang,
			      const IntSurf_Situation Situ1,
                              const IntSurf_Situation Situ2):
       tg(Tang),
       tS1(IntSurf_Touch),tS2(IntSurf_Touch),
       sit1(Situ1),sit2(Situ2),
       uS1(Standard_False),vS1(Standard_False),
       uS2(Standard_False),vS2(Standard_False)
{}

IntPatch_Line::IntPatch_Line (const Standard_Boolean Tang):
       tg(Tang),
       tS1(IntSurf_Undecided),tS2(IntSurf_Undecided),
       sit1(IntSurf_Unknown),sit2(IntSurf_Unknown),
       uS1(Standard_False),vS1(Standard_False),
       uS2(Standard_False),vS2(Standard_False)
{}


