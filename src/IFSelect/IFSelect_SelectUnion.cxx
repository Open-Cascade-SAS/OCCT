#include <IFSelect_SelectUnion.ixx>
#include <IFGraph_Cumulate.hxx>


    IFSelect_SelectUnion::IFSelect_SelectUnion ()    {  }


    Interface_EntityIterator  IFSelect_SelectUnion::RootResult
  (const Interface_Graph& G) const 
{
  IFGraph_Cumulate GC(G);
  Standard_Integer nb = NbInputs();
  for (Standard_Integer i = 1; i <= nb; i ++)
    GC.GetFromIter(Input(i)->RootResult(G));
  return GC.Result();
}

    TCollection_AsciiString  IFSelect_SelectUnion::Label () const 
      {  return TCollection_AsciiString("Union (OR)");  }
