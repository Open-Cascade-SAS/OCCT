#include <IFSelect_SelectExtract.ixx>


    IFSelect_SelectExtract::IFSelect_SelectExtract ()
      {  thesort = Standard_True;  }

    Standard_Boolean  IFSelect_SelectExtract::IsDirect () const 
      {  return thesort;  }

    void  IFSelect_SelectExtract::SetDirect (const Standard_Boolean direct)
      {  thesort = direct;  }


    Interface_EntityIterator  IFSelect_SelectExtract::RootResult
  (const Interface_Graph& G) const 
{
  Interface_EntityIterator iter;
  Interface_EntityIterator inputer = InputResult(G);  // tient compte de tout
  Handle(Interface_InterfaceModel) model = G.Model();
  Standard_Integer rank = 0;
  for (inputer.Start(); inputer.More(); inputer.Next()) {
    Handle(Standard_Transient) ent = inputer.Value();
    rank ++;
    if (SortInGraph(rank,ent,G) == thesort) iter.GetOneItem(ent);
  }
  return iter;
}


    Standard_Boolean  IFSelect_SelectExtract::SortInGraph
  (const Standard_Integer rank, const Handle(Standard_Transient)& ent,
   const Interface_Graph& G) const
      {  return Sort (rank, ent, G.Model());  }


    TCollection_AsciiString  IFSelect_SelectExtract::Label () const
{
  TCollection_AsciiString labl;
  if ( thesort) labl.AssignCat("Picked: ");
  if (!thesort) labl.AssignCat("Removed: ");
  labl.AssignCat(ExtractLabel());
  return labl;
}
