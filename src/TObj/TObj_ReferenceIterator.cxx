// File:        TObj_ReferenceIterator.cxx
// Created:     Tue Nov 23 12:19:22 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
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
