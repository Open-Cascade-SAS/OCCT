#include <SelectMgr_AndFilter.ixx>

#include <SelectMgr_Filter.hxx>
#include <SelectMgr_ListIteratorOfListOfFilter.hxx>

 SelectMgr_AndFilter::SelectMgr_AndFilter()
{
}
Standard_Boolean SelectMgr_AndFilter::IsOk(const Handle(SelectMgr_EntityOwner)& anobj) const 
{
  SelectMgr_ListIteratorOfListOfFilter it(myFilters);
  for ( ; it.More();it.Next()) 
    if(!it.Value()->IsOk(anobj)) 
      return Standard_False;
  return Standard_True;
}


