#include <IFGraph_Cycles.ixx>
#include <Interface_GraphContent.hxx>
#include <IFGraph_StrongComponants.hxx>



//  Cycles utilise les services de StrongComponants :
//  Il retient les Strong Componants qui ne sont pas Single


    IFGraph_Cycles::IFGraph_Cycles
  (const Interface_Graph& agraph, const Standard_Boolean whole)
      :  IFGraph_SubPartsIterator (agraph,whole)    {  }

    IFGraph_Cycles::IFGraph_Cycles (IFGraph_StrongComponants& subparts)
      :  IFGraph_SubPartsIterator (subparts)    {  }


    void  IFGraph_Cycles::Evaluate ()
{
  IFGraph_StrongComponants complist(Model(),Standard_False);
  complist.GetFromIter(Loaded());
  for (complist.Start(); complist.More(); complist.Next()) {
    if (complist.IsSingle()) continue;
    AddPart();
    GetFromIter(complist.Entities());
  }
}
