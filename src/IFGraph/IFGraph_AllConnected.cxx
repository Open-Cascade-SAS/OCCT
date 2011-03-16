#include <IFGraph_AllConnected.ixx>



// AllConnected prend toutes les Entites connectees a une Entite donnee
// c-a-d toutes les "Shared" + toutes les "Sharings" et on recommence
// Autrement dit le contenu du "Composant Connexe" du graphe d'ensemble
// qui contient cette entite
// Le calcul est effectue par GetFromEntity (Evaluate n'a rien a faire)


    IFGraph_AllConnected::IFGraph_AllConnected (const Interface_Graph& agraph)
      :  thegraph (agraph)    {  }


    IFGraph_AllConnected::IFGraph_AllConnected
  (const Interface_Graph& agraph, const Handle(Standard_Transient)& ent)
      :  thegraph (agraph)
      {  GetFromEntity(ent);  }

    void  IFGraph_AllConnected::GetFromEntity
  (const Handle(Standard_Transient)& ent)
{
  if (!thegraph.IsPresent(thegraph.EntityNumber(ent))) return;
  thegraph.GetFromEntity(ent,Standard_False);

  for (Interface_EntityIterator shareds = thegraph.Shareds(ent);
       shareds.More(); shareds.Next())
    GetFromEntity(shareds.Value());

  for (Interface_EntityIterator sharings = thegraph.Sharings(ent);
       sharings.More(); sharings.Next())
    GetFromEntity(sharings.Value());
}

    void  IFGraph_AllConnected::ResetData ()
      {  Reset();  thegraph.Reset();  }

    void  IFGraph_AllConnected::Evaluate()
      {  GetFromGraph(thegraph);  }    // GetFromEntity a tout fait
