// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepData_SelectArrReal.ixx>

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
