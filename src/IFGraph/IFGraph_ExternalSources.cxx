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

#include <IFGraph_ExternalSources.ixx>



// ExternalSources exploite les resultats stockes dans le Graphe sur Sharings
// Soit les "Sharings" des entites notees par GetFromEntity et GetFromIter
// Celles des "Sharings" qui n etaient pas deja notees sont ExternalSources
// Les status :
// - Les entites de depart sont au Status 0
// - Les entites Sharing NOUVELLES (ExternalSources) sont au Status 1


IFGraph_ExternalSources::IFGraph_ExternalSources
  (const Interface_Graph& agraph)
      : thegraph (agraph)    {  }


    void  IFGraph_ExternalSources::GetFromEntity
  (const Handle(Standard_Transient)& ent)
      {  thegraph.GetFromEntity(ent,Standard_True);  }

    void  IFGraph_ExternalSources::GetFromIter
  (const Interface_EntityIterator& iter)
      {  thegraph.GetFromIter(iter,0);  }

    void  IFGraph_ExternalSources::ResetData ()
      {  Reset();  thegraph.Reset();  }


    void  IFGraph_ExternalSources::Evaluate ()
{
  Reset();
  thegraph.RemoveStatus(1);
  Standard_Integer nb = thegraph.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thegraph.IsPresent(i) && thegraph.Status(i) == 0)
      thegraph.GetFromIter ( thegraph.Sharings(thegraph.Entity(i)), 1 );
  }
  GetFromGraph(thegraph,1);
}

    Standard_Boolean IFGraph_ExternalSources::IsEmpty ()
{
  Evaluate();
  Standard_Integer nb = thegraph.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thegraph.IsPresent(i) || thegraph.Status(i) == 1)
      return Standard_False;
  }
  return Standard_True;
}
