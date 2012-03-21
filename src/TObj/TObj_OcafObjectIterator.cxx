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

#include <TObj_OcafObjectIterator.hxx>
#include <TObj_Object.hxx>


IMPLEMENT_STANDARD_HANDLE(TObj_OcafObjectIterator,TObj_LabelIterator)
IMPLEMENT_STANDARD_RTTIEXT(TObj_OcafObjectIterator,TObj_LabelIterator)

//=======================================================================
//function : TObj_OcafObjectIterator
//purpose  :
//=======================================================================

TObj_OcafObjectIterator::TObj_OcafObjectIterator
                         (const TDF_Label&             theLabel,
                          const Handle(Standard_Type)& theType,
                          const Standard_Boolean       theRecursive)
     : TObj_LabelIterator( theLabel, theRecursive ), myType( theType )
{
  MakeStep();
}

//=======================================================================
//function : MakeStep
//purpose  :
//=======================================================================

void TObj_OcafObjectIterator::MakeStep()
{
  for(;myIterator.More() && myNode.IsNull(); )
  {
    TDF_Label L = myIterator.Value();
    Handle(TObj_Object) anObject;
    if(TObj_Object::GetObj(L,anObject))
    {
      if (myType.IsNull() || anObject->IsKind( myType ))
      {
        myObject = anObject;
        myNode = L;
      }
      myIterator.NextBrother();
    }
    else
      myIterator.Next();
  }
}
