#include <IFGraph_AllShared.ixx>
#include <Interface_InterfaceModel.hxx>


    IFGraph_AllShared::IFGraph_AllShared (const Interface_Graph& agraph)
      : thegraph (agraph)    {  }


    IFGraph_AllShared::IFGraph_AllShared
  (const Interface_Graph& agraph, const Handle(Standard_Transient)& ent)
      : thegraph (agraph)
{
  if (!agraph.Model()->Contains(ent)) return;
  GetFromEntity(ent);
}

    void  IFGraph_AllShared::GetFromEntity
  (const Handle(Standard_Transient)& ent)
      {  thegraph.GetFromEntity(ent,Standard_True);  }  // le fait pour nous

     void IFGraph_AllShared::GetFromIter (const Interface_EntityIterator& iter)
{
  for (iter.Start(); iter.More(); iter.Next())
    thegraph.GetFromEntity(iter.Value(),Standard_True);
}

     void IFGraph_AllShared::ResetData ()
      {  Reset();  thegraph.Reset();  }

     void IFGraph_AllShared::Evaluate()
      {  Reset();  GetFromGraph(thegraph);  }
