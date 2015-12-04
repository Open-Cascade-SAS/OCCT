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


#include <Standard_Type.hxx>
#include <StepData_SelectArrReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_SelectArrReal,StepData_SelectNamed)

//  Definitions : cf Field
#define myKindArrReal 8


//=======================================================================
//function : StepData_SelectSeqReal
//purpose  : 
//=======================================================================

StepData_SelectArrReal::StepData_SelectArrReal ()
{
}




//=======================================================================
//function : Kind
//purpose  : 
//=======================================================================

Standard_Integer StepData_SelectArrReal::Kind () const
{
  return myKindArrReal;  
}


//=======================================================================
//function : ArrReal
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepData_SelectArrReal::ArrReal () const
{
 return theArr;  
}


//=======================================================================
//function : SetArrReal
//purpose  : 
//=======================================================================

void StepData_SelectArrReal::SetArrReal (const Handle(TColStd_HArray1OfReal)& arr)
{
  theArr = arr;  
}
