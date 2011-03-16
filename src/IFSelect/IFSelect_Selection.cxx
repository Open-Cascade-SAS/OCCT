#include <IFSelect_Selection.ixx>
#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>



    Standard_Boolean  IFSelect_Selection::HasUniqueResult () const 
      {  return Standard_False;  }    // eminemment redefinissable

// UniqueResult, c est RootResult passe par une Map (-> mis a plat)

    Interface_EntityIterator  IFSelect_Selection::UniqueResult
  (const Interface_Graph& G) const 
{
  Interface_EntityIterator iter = RootResult(G);
  if (HasUniqueResult()) return iter;
  Interface_Graph GG(G);
  GG.GetFromIter(iter,0);
  return Interface_GraphContent(GG);   // EntityIterator specialise (meme taille)
}

// CompleteResult, c est RootResult + propagation du partage (Shareds)

    Interface_EntityIterator  IFSelect_Selection::CompleteResult
  (const Interface_Graph& G) const 
{
  Interface_EntityIterator iter = RootResult(G);
//  On peut utiliser le Graphe a present
  Interface_Graph GG(G);
  for (iter.Start(); iter.More(); iter.Next()) {
    Handle(Standard_Transient) ent = iter.Value();
    GG.GetFromEntity(ent,Standard_True);    // et voila
  }
  return Interface_GraphContent(GG); // EntityIterator specialise (meme taille)
}
