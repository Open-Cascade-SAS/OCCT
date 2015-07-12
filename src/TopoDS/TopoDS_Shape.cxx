// Created on: 1991-03-20
// Created by: Remi Lequette
// Copyright (c) 1991-1999 Matra Datavision
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


#include <Standard_DomainError.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_TShape.hxx>

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================
Standard_Integer TopoDS_Shape::HashCode(const Standard_Integer Upper) const
{
  //PKV
  const Standard_Integer aI = (Standard_Integer) ptrdiff_t(myTShape.operator->());
  const Standard_Integer aHS = ::HashCode(aI,Upper);
  const Standard_Integer aHL = myLocation.HashCode(Upper);
  return (aHS^aHL)%Upper;
} 
