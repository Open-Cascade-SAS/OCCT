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


#include <IntSurf_Transition.hxx>
#include <Standard_DomainError.hxx>

IntSurf_Transition::IntSurf_Transition (const Standard_Boolean Tangent,
					const IntSurf_TypeTrans Type):
       tangent(Tangent),
       typetra(Type),
       situat(IntSurf_Unknown),
       oppos(Standard_False)

{}


IntSurf_Transition::IntSurf_Transition (const Standard_Boolean Tangent,
					const IntSurf_Situation Situ,
					const Standard_Boolean Oppos):
       tangent(Tangent),
       typetra(IntSurf_Touch),
       situat(Situ),
       oppos(Oppos)
{}


IntSurf_Transition::IntSurf_Transition ():
       tangent(Standard_False),
       typetra(IntSurf_Undecided),
       situat(IntSurf_Unknown),
       oppos(Standard_False)
{}
