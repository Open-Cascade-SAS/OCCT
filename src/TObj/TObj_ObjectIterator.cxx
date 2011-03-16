// File:        TObj_ObjectIterator.cxx
// Created:     Tue Nov  23 12:17:26 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_ObjectIterator.hxx>
#include <TObj_Object.hxx>


IMPLEMENT_STANDARD_HANDLE(TObj_ObjectIterator,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(TObj_ObjectIterator,MMgt_TShared)

//=======================================================================
//function : More
//purpose  : 
//=======================================================================
Standard_Boolean TObj_ObjectIterator::More () const
{ return Standard_False; }

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================
void TObj_ObjectIterator::Next ()
{}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Handle(TObj_Object) TObj_ObjectIterator::Value () const
{ return 0; }
