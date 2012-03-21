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

#include <TObj_Object.hxx>
#include <TObj_ReferenceIterator.hxx>
#include <TObj_TReference.hxx>

IMPLEMENT_STANDARD_HANDLE(TObj_ReferenceIterator,TObj_LabelIterator)
IMPLEMENT_STANDARD_RTTIEXT(TObj_ReferenceIterator,TObj_LabelIterator)

//=======================================================================
//function : TObj_ObjectIterator
//purpose  :
//=======================================================================

TObj_ReferenceIterator::TObj_ReferenceIterator
                         (const TDF_Label&             theLabel,
                          const Handle(Standard_Type)& theType,
                          const Standard_Boolean       theRecursive)
  : TObj_LabelIterator (theLabel, theRecursive), myType (theType)
{
  MakeStep();
}

//=======================================================================
//function : MakeStep
//purpose  :
//=======================================================================

void TObj_ReferenceIterator::MakeStep()
{
  for(;myIterator.More() && myNode.IsNull(); myIterator.Next())
  {
    TDF_Label L = myIterator.Value();

    Handle(TObj_TReference) A;
    if ( L.FindAttribute(TObj_TReference::GetID(), A) )
    {
      myObject = A->Get();
      if (! myType.IsNull() && !myObject.IsNull() && !myObject->IsKind( myType ))
        continue;

      myNode = L;
    }
  }
}
