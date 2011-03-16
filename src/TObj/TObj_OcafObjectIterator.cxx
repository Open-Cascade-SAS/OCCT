// File:        TObj_OcafObjectIterator.cxx
// Created:     Tue Nov  23 12:17:26 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
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
