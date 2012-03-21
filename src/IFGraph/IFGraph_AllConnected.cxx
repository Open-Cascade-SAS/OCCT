// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
