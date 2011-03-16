// File:        TObj_LabelIterator.cxx
// Created:     Tue Nov 23 11:51:13 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
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
