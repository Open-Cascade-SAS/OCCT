// Created on: 1996-01-29
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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



#include <SelectMgr_CompositionFilter.ixx>
#include <SelectMgr_ListIteratorOfListOfFilter.hxx>

void SelectMgr_CompositionFilter::Add(const Handle(SelectMgr_Filter)& afilter)
{
  myFilters.Append(afilter);
}

void SelectMgr_CompositionFilter::Remove(const Handle(SelectMgr_Filter)& afilter)
{
  SelectMgr_ListIteratorOfListOfFilter It(myFilters);
  for(;It.More();It.Next()){
    if (afilter==It.Value()){ 
      myFilters.Remove(It);
      return;
    }
  }
}


Standard_Boolean SelectMgr_CompositionFilter::IsEmpty() const
{
  return myFilters.IsEmpty();
}

Standard_Boolean SelectMgr_CompositionFilter::IsIn(const Handle(SelectMgr_Filter)& afilter) const
{
  SelectMgr_ListIteratorOfListOfFilter It(myFilters);
  for(;It.More();It.Next())
    if (afilter==It.Value()) 
      return Standard_True;
  return Standard_False;

}

void  SelectMgr_CompositionFilter::Clear()
{
  myFilters.Clear();
}


Standard_Boolean SelectMgr_CompositionFilter::ActsOn(const TopAbs_ShapeEnum aStandardMode) const
{
  SelectMgr_ListIteratorOfListOfFilter It(myFilters);
  for(;It.More();It.Next()){
    if (It.Value()->ActsOn(aStandardMode))
      return Standard_True;
  }
  
  return Standard_False;
}
