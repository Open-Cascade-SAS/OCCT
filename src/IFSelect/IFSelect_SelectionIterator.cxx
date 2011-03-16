#include <IFSelect_SelectionIterator.ixx>


    IFSelect_SelectionIterator::IFSelect_SelectionIterator ()
{
  thecurr = 1;
  thelist = new IFSelect_HSeqOfSelection();
}

    IFSelect_SelectionIterator::IFSelect_SelectionIterator
  (const Handle(IFSelect_Selection)& sel)
{
  thecurr = 1;
  thelist = new IFSelect_HSeqOfSelection();
  sel->FillIterator(*this);
}

    void  IFSelect_SelectionIterator::AddFromIter (IFSelect_SelectionIterator& iter)
      {  for (; iter.More(); iter.Next()) AddItem(iter.Value());  }

    void  IFSelect_SelectionIterator::AddItem
  (const Handle(IFSelect_Selection)& sel)
      {  if (!sel.IsNull()) thelist->Append(sel);  }

    void  IFSelect_SelectionIterator::AddList
  (const IFSelect_TSeqOfSelection& list)
{
  Standard_Integer nb = list.Length();  // <list> Pas Handle  <thelist> Handle
  for (Standard_Integer i = 1; i <= nb; i ++) thelist->Append(list.Value(i));
}

    Standard_Boolean  IFSelect_SelectionIterator::More () const 
      {  return (thecurr <= thelist->Length());  }

    void  IFSelect_SelectionIterator::Next () 
      {  thecurr ++;  }

    const Handle(IFSelect_Selection)&  IFSelect_SelectionIterator::Value () const 
      {  return thelist->Value(thecurr);  }
