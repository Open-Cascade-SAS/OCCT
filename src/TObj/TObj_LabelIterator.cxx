// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_LabelIterator.hxx>

IMPLEMENT_STANDARD_HANDLE(TObj_LabelIterator,TObj_ObjectIterator)
IMPLEMENT_STANDARD_RTTIEXT(TObj_LabelIterator,TObj_ObjectIterator)
     
//=======================================================================
//function : TObj_LabelIterator
//purpose  : 
//=======================================================================

TObj_LabelIterator::TObj_LabelIterator()
{
}
     
//=======================================================================
//function : TObj_LabelIterator
//purpose  : 
//=======================================================================

TObj_LabelIterator::TObj_LabelIterator(const TDF_Label& theLabel,
                                               const Standard_Boolean isRecursive)
{
  Init(theLabel,isRecursive);
}
     
//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TObj_LabelIterator::Next()
{
  myObject.Nullify();
  myNode.Nullify();
  MakeStep();
}
