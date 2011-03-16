#include <IFGraph_ConnectedComponants.ixx>
#include <IFGraph_AllConnected.hxx>
#include <Standard_Transient.hxx>



// Pour travailler, ConnectedComponants exploite AllConnected :
// On prend un 1er Vertex, on determine ses AllConnected -> voila un 1er
//  Connected Componant
// On recommence jusqu'a ce qu'il n'y ait plus de Vertex libre


//  Honnetement, si ca ne marche pas, cf classe ConnectedVerticesIterator
//  de GraphTools  qui fait en principe la meme chose


    IFGraph_ConnectedComponants::IFGraph_ConnectedComponants
  (const Interface_Graph& agraph, const Standard_Boolean whole)
      :  IFGraph_SubPartsIterator (agraph, whole)    {  }

    void  IFGraph_ConnectedComponants::Evaluate()
{
//  On part des "loaded"
//  Pour chacun : s il est note dans le graphe, on passe
//  Sinon, on ajoute les AllConnected en tant que sub-part
  Interface_EntityIterator loaded = Loaded();
  Reset();
  for (loaded.Start(); loaded.More(); loaded.Next()) {
    Handle(Standard_Transient) ent = loaded.Value();
    if (IsInPart(ent)) continue;
    IFGraph_AllConnected connect(Model(),ent);
    AddPart();
    GetFromIter (connect);
  }
}
