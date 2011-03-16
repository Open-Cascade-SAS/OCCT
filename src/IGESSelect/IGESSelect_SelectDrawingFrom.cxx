#include <IGESSelect_SelectDrawingFrom.ixx>
#include <IGESSelect_ViewSorter.hxx>
#include <IGESData_IGESModel.hxx>
#include <Interface_Macros.hxx>



    IGESSelect_SelectDrawingFrom::IGESSelect_SelectDrawingFrom ()    {  }


    Standard_Boolean  IGESSelect_SelectDrawingFrom::HasUniqueResult () const
      {  return Standard_True;  }

    Interface_EntityIterator  IGESSelect_SelectDrawingFrom::RootResult
  (const Interface_Graph& G) const
{
  Handle(IGESSelect_ViewSorter) sorter = new IGESSelect_ViewSorter;
  sorter->SetModel (GetCasted(IGESData_IGESModel,G.Model()));
  sorter->Clear();
  sorter->AddList (InputResult(G).Content());
  sorter->SortDrawings(G);
  Interface_EntityIterator list;
  Standard_Integer nb = sorter->NbSets(Standard_True);
  for (Standard_Integer i = 1; i <= nb; i ++)
    list.GetOneItem(sorter->SetItem(i,Standard_True));
  return list;
}

    TCollection_AsciiString  IGESSelect_SelectDrawingFrom::Label () const
      {  return TCollection_AsciiString ("Drawings attached");  }
