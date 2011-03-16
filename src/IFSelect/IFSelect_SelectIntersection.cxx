#include <IFSelect_SelectIntersection.ixx>
#include <IFGraph_Compare.hxx>


    IFSelect_SelectIntersection::IFSelect_SelectIntersection ()    {  }


    Interface_EntityIterator  IFSelect_SelectIntersection::RootResult
  (const Interface_Graph& G) const 
{
  IFGraph_Compare GC(G);
  Standard_Integer nb = NbInputs();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    GC.GetFromIter(Input(i)->RootResult(G), (i==1));
    if (i > 1 && i < nb) {
      Interface_EntityIterator comm = GC.Common();
      GC.ResetData();
      GC.GetFromIter (comm,Standard_True);
    }
  }
  return GC.Common();
}

    TCollection_AsciiString  IFSelect_SelectIntersection::Label () const 
      {  return TCollection_AsciiString("Intersection (AND)");  }
