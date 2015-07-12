// Created on: 1999-04-08
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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

//    gka 13.04.99 S4136: add myPrecision and myMaxTol fields

#include <Precision.hxx>
#include <StepToTopoDS_Root.hxx>

//=======================================================================
//function : StepToTopoDS_Root
//purpose  : 
//=======================================================================
StepToTopoDS_Root::StepToTopoDS_Root () : done(Standard_False) 
{
  myPrecision = myMaxTol = Precision::Confusion();
}

