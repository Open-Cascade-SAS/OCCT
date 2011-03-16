#include <IFGraph_Compare.ixx>
#include <IFGraph_AllShared.hxx>



//  Comparateur de deux sous-ensembles d un Modele
//  Au premier  sous-ensemble, est attribue le Status 1
//  Au deuxieme sous-ensemble, est attribue le Status 2
//  La partie commune se voit attribuer le Status 3

    IFGraph_Compare::IFGraph_Compare (const Interface_Graph& agraph)
      : thegraph (agraph)    {  }

    void  IFGraph_Compare::GetFromEntity
  (const Handle(Standard_Transient)& ent, const Standard_Boolean first)
{
  IFGraph_AllShared iter(thegraph.Model(),ent);
  GetFromIter(iter,first);
}

    void  IFGraph_Compare::GetFromIter
  (const Interface_EntityIterator& iter, const Standard_Boolean first)
{
  Standard_Integer stat = 2;
  if (first) stat = 1;
  thegraph.GetFromIter(iter,stat,3,Standard_False);
}


    void  IFGraph_Compare::Merge ()
{
  thegraph.ChangeStatus (2,1);
  thegraph.ChangeStatus (3,1);
}

    void  IFGraph_Compare::RemoveSecond ()
{
  thegraph.ChangeStatus (3,1);
  thegraph.RemoveStatus (2);
}

    void  IFGraph_Compare::KeepCommon ()
{
  thegraph.RemoveStatus (1);
  thegraph.RemoveStatus (2);
  thegraph.ChangeStatus (3,1);
}

    void  IFGraph_Compare::ResetData ()
      {  Reset();  thegraph.Reset();  }

    void  IFGraph_Compare::Evaluate ()
{
  Reset();  GetFromGraph(thegraph);  // Evaluation deja faite par le graphe
}

    Interface_EntityIterator  IFGraph_Compare::Common () const
      {  return Interface_GraphContent(thegraph,3);  }

    Interface_EntityIterator  IFGraph_Compare::FirstOnly () const 
      {  return Interface_GraphContent(thegraph,1);  }

    Interface_EntityIterator  IFGraph_Compare::SecondOnly () const 
      {  return Interface_GraphContent(thegraph,2);  }
