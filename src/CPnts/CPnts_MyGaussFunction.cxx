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


#include <CPnts_MyGaussFunction.hxx>

void CPnts_MyGaussFunction::Init(const CPnts_RealFunction& F,
				 const Standard_Address D) 
{
  myFunction = F;
  myData = D;
}

Standard_Boolean CPnts_MyGaussFunction::Value(const Standard_Real X,
					      Standard_Real& F)
{
  F = myFunction(X,myData);
  return Standard_True;
}










