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

#include <IFGraph_Articulations.ixx>
#include <Interface_InterfaceModel.hxx>



// Points d'Articulation d'un Graphe : ce sont les "passages obliges" du graphe
// Algorithme tire du Sedgewick, p 392

IFGraph_Articulations::IFGraph_Articulations
  (const Interface_Graph& agraph, const Standard_Boolean whole)
      :  thegraph (agraph)
      {  if (whole) thegraph.GetFromModel();  }


   void  IFGraph_Articulations::GetFromEntity
  (const Handle(Standard_Transient)& ent)
      {  thegraph.GetFromEntity(ent,Standard_True);  }

   void  IFGraph_Articulations::GetFromIter(const Interface_EntityIterator& iter)
      {  thegraph.GetFromIter(iter,0);  }


   void  IFGraph_Articulations::ResetData ()
{  Reset();  thegraph.Reset(); thelist = new TColStd_HSequenceOfInteger();  }

   void  IFGraph_Articulations::Evaluate ()
{
//  Algorithme, cf Sedgewick "Algorithms", p 392
  thelist = new TColStd_HSequenceOfInteger();
//  Utilisation de Visit
  Standard_Integer nb = thegraph.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Standard_Integer visited;
    thenow = 0;
    if (thegraph.IsPresent(i)) visited = Visit(i);
  }
//  Resultat dans thelist
  Reset();
  Standard_Integer nbres = thelist->Length();
  for (Standard_Integer ires = 1; ires <= nbres; ires ++) {
    Standard_Integer num = thelist->Value(ires);
    GetOneItem(thegraph.Model()->Value(num));
  }
}

    Standard_Integer IFGraph_Articulations::Visit (const Standard_Integer num)
{
  thenow ++;
  thegraph.SetStatus(num,thenow);
  Standard_Integer min = thenow;

  for (Interface_EntityIterator iter = thegraph.Shareds(thegraph.Entity(num));
       iter.More(); iter.Next()) {
    Handle(Standard_Transient) ent = iter.Value();
    Standard_Integer nument  = thegraph.EntityNumber(ent);
    if (!thegraph.IsPresent(num)) {
      thegraph.GetFromEntity(ent,Standard_False);
      nument  = thegraph.EntityNumber(ent);
    }
    Standard_Integer statent = thegraph.Status(nument);     // pas reevalue
    if (statent == 0) {
      Standard_Integer mm = Visit(nument);
      if (mm < min) min = mm;
      if (mm > thegraph.Status(num)) thelist->Append(num);  // ON EN A UN : num
    }
    else if (statent < min) min = statent;
  }
  return min;
}
