#include <IFGraph_StrongComponants.ixx>
//#include <IFGraph_SortedStrongs.hxx>
#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>

//#define PRINTDEB
#define pbgraphtool


    IFGraph_StrongComponants::IFGraph_StrongComponants
  (const Interface_Graph& agraph, const Standard_Boolean whole)
      : IFGraph_SubPartsIterator (agraph, whole)    {  }

    void  IFGraph_StrongComponants::Evaluate ()
{
  Interface_GraphContent iter = Loaded();
  Interface_Graph G(thegraph); G.GetFromIter(iter,0);
#ifdef pbgraphtool
  Standard_Integer nb = G.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (!G.IsPresent(i)) continue;
    AddPart();
    GetFromEntity (G.Entity(i),Standard_False);
  }
#else
#ifdef PRINTDEB
  cout<<"StrongComponants :"<<endl;
#endif
  for (IFGraph_SortedStrongs res(G); res.More(); res.Next()) {
    Standard_Integer nb = res.NbVertices();
#ifdef PRINTDEB
    cout<<"  Iteration, Vertices:"<<nb<<" :";
#endif
    if (nb == 0) continue;
    AddPart();
    for (Standard_Integer i = 1; i <= nb; i ++)
#ifdef PRINTDEB
      {
	Handle(Standard_Transient) oneres = res.Value(i);
	cout<<" "<<G.EntityNumber(oneres);
	GetFromEntity(oneres,Standard_False);
      }
    cout<<endl;
#else
      GetFromEntity(res.Value(i),Standard_False);
#endif
  }
#endif
}
