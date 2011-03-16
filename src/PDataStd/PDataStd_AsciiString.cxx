// File:	PDataStd_AsciiString.cxx
// Created:	Wed Aug 22 16:30:32 2007
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2007

#include <PDataStd_AsciiString.ixx>
#include <PCollection_HAsciiString.hxx>
//=======================================================================
//function : PDataStd_AsciiString
//purpose  : Constructor
//=======================================================================
PDataStd_AsciiString::PDataStd_AsciiString() {}

//=======================================================================
//function : PDataStd_AsciiString
//purpose  : Constructor
//=======================================================================
PDataStd_AsciiString::PDataStd_AsciiString(const Handle(PCollection_HAsciiString)& V)
: myValue (V) {}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================
Handle(PCollection_HAsciiString) PDataStd_AsciiString::Get() const
{ return myValue; }

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void PDataStd_AsciiString::Set(const Handle(PCollection_HAsciiString)& V) 
{ myValue = V; }

