#include <IFSelect_SelectDeduct.ixx>


    void  IFSelect_SelectDeduct::SetInput (const Handle(IFSelect_Selection)& sel)
      {  thesel = sel;  }

    Handle(IFSelect_Selection)  IFSelect_SelectDeduct::Input () const 
      {  return thesel;  }

    Standard_Boolean  IFSelect_SelectDeduct::HasInput () const 
      {  return (!thesel.IsNull());  }

    Standard_Boolean  IFSelect_SelectDeduct::HasAlternate () const 
{  if (!thealt.IsNull()) return thealt->IsSet();  return Standard_False;  }

    Handle(IFSelect_SelectPointed)&  IFSelect_SelectDeduct::Alternate ()
{
  if (thealt.IsNull()) thealt = new IFSelect_SelectPointed;
  return thealt;
}


    Interface_EntityIterator  IFSelect_SelectDeduct::InputResult
  (const Interface_Graph& G) const
{
  Interface_EntityIterator res;
  if (!thealt.IsNull()) {
    if (thealt->IsSet()) {
      res = thealt->UniqueResult (G);
      thealt->Clear();
      return res;
    }
  }
  if (thesel.IsNull()) return res;
  return  thesel->UniqueResult(G);
}

    void  IFSelect_SelectDeduct::FillIterator
  (IFSelect_SelectionIterator& iter) const 
      {  iter.AddItem(thesel);  }
