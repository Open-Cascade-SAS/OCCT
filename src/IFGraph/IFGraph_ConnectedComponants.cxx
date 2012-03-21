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
