// Created on: 1999-06-11
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


#include <Standard_Type.hxx>
#include <TDF_Label.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TFunction_Driver,Standard_Transient)

//=======================================================================
//function : TFunction_Driver
//purpose  : Constructor
//=======================================================================
TFunction_Driver::TFunction_Driver()
{

}


//=======================================================================
//function : Init
//purpose  : Initialization
//=======================================================================

void TFunction_Driver::Init(const TDF_Label& L)
{
  myLabel = L;
}


//=======================================================================
//function : Validate
//purpose  : Validates labels of a function
//=======================================================================

void TFunction_Driver::Validate(Handle(TFunction_Logbook)& log) const
{
  TDF_LabelList res;
  Results(res);

  TDF_ListIteratorOfLabelList itr(res);
  for (; itr.More(); itr.Next())
  {
    log->SetValid(itr.Value(), Standard_True);
  }
}


//=======================================================================
//function : MustExecute
//purpose  : Analyzes the labels in the logbook
//=======================================================================

Standard_Boolean TFunction_Driver::MustExecute(const Handle(TFunction_Logbook)& log) const
{
  // Check modification of arguments.
  TDF_LabelList args;
  Arguments(args);

  TDF_ListIteratorOfLabelList itr(args);
  for (; itr.More(); itr.Next())
  {
    if (log->IsModified(itr.Value()))
      return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : Arguments
//purpose  : The method fills-in the list by labels, 
//           where the arguments of the function are located.
//=======================================================================

void TFunction_Driver::Arguments(TDF_LabelList& ) const
{

}


//=======================================================================
//function : Results
//purpose  : The method fills-in the list by labels,
//           where the results of the function are located.
//=======================================================================

void TFunction_Driver::Results(TDF_LabelList& ) const
{

}
