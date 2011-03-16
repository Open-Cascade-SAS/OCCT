// File:	SelectMgr_CompositionFilter.cxx
// Created:	Mon Jan 29 17:49:34 1996
// Author:	Robert COUBLANC
//		<rob@fidox>


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
