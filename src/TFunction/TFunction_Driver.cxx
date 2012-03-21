// Created on: 1999-06-11
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <TFunction_Driver.ixx>

#include <TDF_Label.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>


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

void TFunction_Driver::Validate(TFunction_Logbook& log) const
{
  TDF_LabelList res;
  Results(res);
  TDF_ListIteratorOfLabelList itr(res);
  for (; itr.More(); itr.Next())
  {
    log.SetValid(itr.Value(), Standard_True);
  }
}


//=======================================================================
//function : MustExecute
//purpose  : Analyzes the labels in the logbook
//=======================================================================

Standard_Boolean TFunction_Driver::MustExecute(const TFunction_Logbook& log) const
{
  // Check modification of arguments.
  TDF_LabelList args;
  Arguments(args);
  TDF_ListIteratorOfLabelList itr(args);
  for (; itr.More(); itr.Next())
  {
    if (log.IsModified(itr.Value()))
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
