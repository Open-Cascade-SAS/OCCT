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
  if (HasUniqueResult() || !G.ModeStat()) 
    return iter;
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
