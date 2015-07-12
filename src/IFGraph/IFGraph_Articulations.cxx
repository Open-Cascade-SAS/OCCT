// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <IFGraph_Articulations.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>

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
    thenow = 0;
    if (thegraph.IsPresent(i)) Visit(i);
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
