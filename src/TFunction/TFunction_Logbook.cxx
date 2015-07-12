// Created on: 1999-07-20
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Standard_OStream.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_Tool.hxx>
#include <TFunction_Logbook.hxx>

//=======================================================================
//function : TFunction_Logbook
//purpose  : A Logbook creation
//=======================================================================
TFunction_Logbook::TFunction_Logbook():isDone(Standard_False)
{}

//=======================================================================
//function : Clear
//purpose  : Clears the valid and modified labels
//=======================================================================

void TFunction_Logbook::Clear()
{
  myTouched.Clear();
  myImpacted.Clear();
  myValid.Clear();
}

//=======================================================================
//function : IsEmpty
//purpose  : Returns Standard_True if the nothing is reccorded in the logbook
//=======================================================================

Standard_Boolean TFunction_Logbook::IsEmpty () const
{
  return (myTouched.IsEmpty() && myImpacted.IsEmpty() && myValid.IsEmpty());
}

//=======================================================================
//function : IsModified
//purpose  : Returns Standard_True if the label is modified
//=======================================================================

Standard_Boolean TFunction_Logbook::IsModified(const TDF_Label& L,
					       const Standard_Boolean WithChildren) const
{
  if (myTouched.Contains(L)) return Standard_True;
  if (myImpacted.Contains(L)) return Standard_True;
  if (WithChildren) {
    TDF_ChildIterator itr(L);
    for (; itr.More(); itr.Next())
      if (IsModified(itr.Value(), Standard_True))
	return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : SetValid
//purpose  : 
//=======================================================================

void TFunction_Logbook::SetValid(const TDF_Label& L,
				 const Standard_Boolean WithChildren)
{
  myValid.Add(L);
  if (WithChildren) {
    TDF_ChildIterator itr(L, Standard_True);
    for (; itr.More(); itr.Next()) {
      myValid.Add(itr.Value());
    }
  }
}

//=======================================================================
//function : SetImpacted
//purpose  : 
//=======================================================================

void TFunction_Logbook::SetImpacted(const TDF_Label& L,
				    const Standard_Boolean WithChildren)
{
  myImpacted.Add(L);
  if (WithChildren) {
    TDF_ChildIterator itr(L, Standard_True);
    for (; itr.More(); itr.Next()) {
      myImpacted.Add(itr.Value());
    }
  }  
}

//=======================================================================
//function : Dump
//purpose  : Dump of modifications
//=======================================================================

Standard_OStream& TFunction_Logbook::Dump(Standard_OStream& stream) const
{
  TDF_MapIteratorOfLabelMap itr;
  TCollection_AsciiString as;
  
  stream<<"Done = "<<isDone<<endl;
  stream<<"Touched labels: "<<endl;
  for (itr.Initialize(myTouched); itr.More(); itr.Next()) {
    TDF_Tool::Entry(itr.Key(), as);
    stream<<as<<endl;
  }
  stream<<"Impacted labels: "<<endl;
  for (itr.Initialize(myImpacted); itr.More(); itr.Next()) {
    TDF_Tool::Entry(itr.Key(), as);
    stream<<as<<endl;
  }  
  stream<<"Valid labels: "<<endl;
  for (itr.Initialize(myValid); itr.More(); itr.Next()) {
    TDF_Tool::Entry(itr.Key(), as);
    stream<<as<<endl;
  }  

  return stream;
}
