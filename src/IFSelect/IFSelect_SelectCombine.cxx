#include <IFSelect_SelectCombine.ixx>
#include <IFGraph_Cumulate.hxx>


    IFSelect_SelectCombine::IFSelect_SelectCombine ()    {  }
//      {  thelist = new IFSelect_SequenceOfSelection();  }

    Standard_Integer  IFSelect_SelectCombine::NbInputs () const 
      {  return thelist.Length();  }

    Handle(IFSelect_Selection)  IFSelect_SelectCombine::Input
  (const Standard_Integer num) const 
      {  return thelist.Value(num);  }

    Standard_Integer  IFSelect_SelectCombine::InputRank
  (const Handle(IFSelect_Selection)& sel) const 
{
  if (sel.IsNull()) return 0;
  Standard_Integer i, nb = thelist.Length();
  for (i = 1; i <= nb; i ++)
    if (sel == thelist.Value(i)) return i;
  return 0;
}

    void  IFSelect_SelectCombine::Add
  (const Handle(IFSelect_Selection)& sel, const Standard_Integer atnum)
{
  if (atnum <= 0 || atnum > thelist.Length()) thelist.Append(sel);
  else thelist.InsertBefore(atnum,sel);
}

    Standard_Boolean  IFSelect_SelectCombine::Remove
  (const Handle(IFSelect_Selection)& sel)
      {  return Remove (InputRank(sel));  }

    Standard_Boolean  IFSelect_SelectCombine::Remove
  (const Standard_Integer num)
{
  if (num <= 0 || num > thelist.Length()) return Standard_False;
  thelist.Remove(num);
  return Standard_True;
}


    Standard_Boolean  IFSelect_SelectCombine::HasUniqueResult () const 
      {  return Standard_True;  }

    void  IFSelect_SelectCombine::FillIterator
  (IFSelect_SelectionIterator& iter) const 
      {  iter.AddList(thelist);  }
