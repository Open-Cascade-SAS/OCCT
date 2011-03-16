#include <SelectMgr_OrFilter.ixx>

#include <SelectMgr_Filter.hxx>
#include <SelectMgr_ListIteratorOfListOfFilter.hxx>
 SelectMgr_OrFilter::SelectMgr_OrFilter()
{
}


Standard_Boolean SelectMgr_OrFilter::IsOk(const Handle(SelectMgr_EntityOwner)& anobj) const 
{

  if(myFilters.IsEmpty())
    return Standard_True;
  SelectMgr_ListIteratorOfListOfFilter it(myFilters);
  for ( ; it.More();it.Next())
    if(it.Value()->IsOk(anobj))
      return Standard_True;
  return Standard_False;
}
