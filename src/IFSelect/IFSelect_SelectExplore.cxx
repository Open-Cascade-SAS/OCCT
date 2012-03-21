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

#include <IFSelect_SelectExplore.ixx>
#include <TColStd_IndexedMapOfTransient.hxx>
#include <stdio.h>


IFSelect_SelectExplore::IFSelect_SelectExplore (const Standard_Integer level)
: thelevel (level)    {  }


    Standard_Integer  IFSelect_SelectExplore::Level () const
      {  return thelevel;  }


    Interface_EntityIterator  IFSelect_SelectExplore::RootResult
  (const Interface_Graph& G) const
{
//  Attention, voila comme on procede
//  On a une IndexedMapOfTransient en entree (entites deja traitees/a traiter)
//    Elle est initialisee par InputResult
//  Et une map en sortie (resultats pris) -> le resultat sera unique
//  En entree, un curseur d entite courante
//  Pour chaque entite, on appelle Explore. 3 cas possibles :
//    retour False, on passe
//    retour True et liste vide, on prend cette entite sans aller plus loin
//    retour True et liste non vide, on ne prend pas cette entite mais on
//      considere son resultat.
//      Si dernier niveau, on le prend en entier. Sinon, il alimente l entree

  Standard_Integer nb = G.Size();
  TColStd_IndexedMapOfTransient entrees (nb);
  TColStd_IndexedMapOfTransient result  (nb);
//  Initialisation
  Standard_Integer i, j, level = 1, ilev = 0;
  Interface_EntityIterator input; input = InputResult(G);
  for (input.Start(); input.More(); input.Next())
    i = entrees.Add (input.Value());
  ilev = entrees.Extent();

// Exploration
  for (i = 1; i <= nb; i ++) {
    if (i > entrees.Extent()) break;
    if (i > ilev)  {
      level ++;
      if (level > thelevel && thelevel > 0) break;
      ilev = entrees.Extent();
    }
    Handle(Standard_Transient) ent = entrees.FindKey(i);
    if (ent.IsNull()) continue;
    Interface_EntityIterator exp;
    if (!Explore (level,ent,G,exp)) continue;

//  On prend en compte : entite a prendre directement ?
//  reprendre liste en entree (niveau pas atteint) ou resultat (niveau atteint)
    if (exp.NbEntities() == 0) {
      j = result.Add (ent);
      continue;
    } else if (level == thelevel) {
      for (exp.Start(); exp.More(); exp.Next()) j = result.Add (exp.Value());
    } else {
      for (exp.Start(); exp.More(); exp.Next()) j = entrees.Add (exp.Value());
    }
  }

//  On recolte le resultat
  Interface_EntityIterator res;
  nb = result.Extent();
  for (j = 1; j <= nb; j ++) res.AddItem (result.FindKey(j));
  return res;
}


    TCollection_AsciiString  IFSelect_SelectExplore::Label () const
{
  TCollection_AsciiString labl;
  if (thelevel == 0) labl.AssignCat("(Recursive)");
  else if (thelevel > 0) {
    char lab[30];
    sprintf (lab,"(Level %d)",thelevel);
    labl.AssignCat(lab);
  }
  labl.AssignCat(ExploreLabel());
  return labl;
}
