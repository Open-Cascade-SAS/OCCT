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


#include <IFGraph_SubPartsIterator.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_Array1OfInteger.hxx>

// SubPartsIterator permet de regrouper les entites en plusieurs sous-parties
// A chaque sous-partie est attache un Status : la 1re a 1, la 2e a 2, etc...
// (consequence, les sous-parties sont necessairement disjointes)
IFGraph_SubPartsIterator::IFGraph_SubPartsIterator
  (const Interface_Graph& agraph, const Standard_Boolean whole)
      : thegraph (agraph)
{
  if (whole) thegraph.GetFromModel();
  theparts  = new TColStd_HSequenceOfInteger();
  thefirsts = new TColStd_HSequenceOfInteger();
  thepart   = 0;
  thecurr   = 0;
}

    IFGraph_SubPartsIterator::IFGraph_SubPartsIterator
  (IFGraph_SubPartsIterator& other)
      : thegraph (other.Graph())
{
  Standard_Integer nb = thegraph.Size();
  theparts = new TColStd_HSequenceOfInteger();
  thepart = 0;
  for (other.Start(); other.More(); other.Next()) {
    thepart ++;
    Standard_Integer nbent = 0;
    GetFromIter (other.Entities());
    for (Standard_Integer i = 1; i <= nb; i ++) {
      if (thegraph.Status(i) == thepart) nbent ++;
    }
    theparts->Append(nbent);  // compte vide
  }
  thepart = 0;
  thecurr = 1;
}

    void  IFGraph_SubPartsIterator::GetParts
  (IFGraph_SubPartsIterator& other)
{
  if (Model() != other.Model()) throw Interface_InterfaceError("SubPartsIterator : GetParts");
//  On AJOUTE les Parts de other, sans perdre les siennes propres
//  (meme principe que le constructeur ci-dessus)
  Standard_Integer nb = thegraph.Size();
  thepart = theparts->Length();
  for (other.Start(); other.More(); other.Next()) {
    thepart ++;
    Standard_Integer nbent = 0;
    GetFromIter (other.Entities());
    for (Standard_Integer i = 1; i <= nb; i ++) {
      if (thegraph.Status(i) == thepart) nbent ++;
    }
    theparts->Append(nbent);  // compte vide
  }
}

    const Interface_Graph&  IFGraph_SubPartsIterator::Graph () const
      {  return thegraph;  }

//  ....            Gestion Interne (remplissage, etc...)            .... //

    Handle(Interface_InterfaceModel)  IFGraph_SubPartsIterator::Model() const 
      {  return thegraph.Model();  }

    void  IFGraph_SubPartsIterator::AddPart ()
{
  theparts->Append( Standard_Integer(0) );
  thepart = theparts->Length();
}

    Standard_Integer  IFGraph_SubPartsIterator::NbParts () const 
      {  return theparts->Length();  }

    Standard_Integer  IFGraph_SubPartsIterator::PartNum () const 
      {  return thepart;  }

    void  IFGraph_SubPartsIterator::SetLoad ()
      {  thepart = 0;  }

    void  IFGraph_SubPartsIterator::SetPartNum (const Standard_Integer num)
{
  if (num <= 0 || num > theparts->Length()) throw Standard_OutOfRange("IFGraph_SubPartsIterator : SetPartNum");
  thepart = num;
}

    void  IFGraph_SubPartsIterator::GetFromEntity
  (const Handle(Standard_Transient)& ent, const Standard_Boolean shared)
{
  thegraph.GetFromEntity(ent,shared, thepart,thepart,Standard_False);
}

    void IFGraph_SubPartsIterator::GetFromIter (const Interface_EntityIterator& iter)
{
  thegraph.GetFromIter(iter, thepart,thepart, Standard_False);
}

    void  IFGraph_SubPartsIterator::Reset ()
{
  thegraph.Reset();
  theparts->Clear();
  thepart = 0;
  thecurr = 0;
}


//  ....              Resultat (Evaluation, Iterations)              .... //

    void  IFGraph_SubPartsIterator::Evaluate ()
{  }    // par defaut, ne fait rien; redefinie par les sous-classes

    Interface_GraphContent  IFGraph_SubPartsIterator::Loaded () const 
{
  Interface_EntityIterator iter;
//  Standard_Integer nb = thegraph.Size();
  return Interface_GraphContent(thegraph,0);
}

    Interface_Graph  IFGraph_SubPartsIterator::LoadedGraph () const 
{
  Interface_Graph G(Model());
  Standard_Integer nb = thegraph.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thegraph.IsPresent(i) && thegraph.Status(i) == 0)
      G.GetFromEntity(thegraph.Entity(i),Standard_False);
  }
  return G;
}


    Standard_Boolean IFGraph_SubPartsIterator::IsLoaded
  (const Handle(Standard_Transient)& ent) const
{  return thegraph.IsPresent(thegraph.EntityNumber(ent));  }

    Standard_Boolean IFGraph_SubPartsIterator::IsInPart
  (const Handle(Standard_Transient)& ent) const
{
  Standard_Integer num = thegraph.EntityNumber(ent);
  if (!thegraph.IsPresent(num)) return Standard_False;
  return (thegraph.Status(num) != 0);
}

    Standard_Integer IFGraph_SubPartsIterator::EntityPartNum
  (const Handle(Standard_Transient)& ent) const
{
  Standard_Integer num = thegraph.EntityNumber(ent);
  if (!thegraph.IsPresent(num)) return 0;
  return thegraph.Status(num);
}


    void  IFGraph_SubPartsIterator::Start ()
{
  Evaluate();
//  On evalue les tailles des contenus des Parts
  Standard_Integer nb  = thegraph.Size();
  Standard_Integer nbp = theparts->Length();
  if (thepart > nbp) thepart = nbp;
  if (nbp == 0) {  thecurr = 1; return;  }    // L Iteration s arrete de suite

//  - On fait les comptes (via tableaux pour performances)
  TColStd_Array1OfInteger partcounts (1,nbp); partcounts.Init(0);
  TColStd_Array1OfInteger partfirsts (1,nbp); partfirsts.Init(0);
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (!thegraph.IsPresent(i)) continue;
    Standard_Integer nump = thegraph.Status(i);
    if (nump < 1 || nump > nbp) continue;
    Standard_Integer nbent = partcounts.Value(nump);
    partcounts.SetValue(nump,nbent+1);
    if (nbent == 0) partfirsts.SetValue(nump,i);
  }
//  - On les met en forme (c-a-d dans les sequences)
  theparts->Clear(); thefirsts->Clear();
  Standard_Integer lastp = 0;
  for (Standard_Integer np = 1; np <= nbp; np ++) {
    Standard_Integer nbent = partcounts.Value(np);
    if (np != 0) lastp = np;
    theparts->Append  (nbent);
    thefirsts->Append (partfirsts.Value(np));
  }
  if (lastp < nbp) theparts->Remove(lastp+1,nbp);
//  Enfin, on se prepare a iterer
  thecurr = 1;
}

    Standard_Boolean  IFGraph_SubPartsIterator::More ()
{
  if (thecurr == 0) Start();
  return (thecurr <= theparts->Length());
}

    void  IFGraph_SubPartsIterator::Next ()
{
  thecurr ++; if (thecurr > theparts->Length()) return;
  if (theparts->Value(thecurr) == 0) Next();  // sauter parties vides
}

    Standard_Boolean  IFGraph_SubPartsIterator::IsSingle () const
{
  if (thecurr < 1 || thecurr > theparts->Length()) throw Standard_NoSuchObject("IFGraph_SubPartsIterator : IsSingle");
  return (theparts->Value(thecurr) == 1);
}

    Handle(Standard_Transient)  IFGraph_SubPartsIterator::FirstEntity
  () const 
{
  if (thecurr < 1 || thecurr > theparts->Length()) throw Standard_NoSuchObject("IFGraph_SubPartsIterator : FirstEntity");
  Standard_Integer nument = thefirsts->Value(thecurr);
  if (nument == 0) throw Standard_NoSuchObject("IFGraph_SubPartsIterator : FirstEntity (current part is empty)");
  return thegraph.Entity(nument);
}

    Interface_EntityIterator  IFGraph_SubPartsIterator::Entities () const 
{
  if (thecurr < 1 || thecurr > theparts->Length()) throw Standard_NoSuchObject("IFGraph_SubPartsIterator : Entities");
  Interface_EntityIterator iter;
  Standard_Integer nb = thegraph.Size();
  Standard_Integer nument = thefirsts->Value(thecurr);
  if (nument == 0) return iter;
  if (theparts->Value(thecurr) == 1) nb = nument;   // evident : 1 seule Entite
  for (Standard_Integer i = nument; i <= nb; i ++) {
    if (thegraph.Status(i) == thecurr && thegraph.IsPresent(i))
      iter.GetOneItem(thegraph.Entity(i));
  }
  return iter;
}

//=======================================================================
//function : ~IFGraph_SubPartsIterator
//purpose  : 
//=======================================================================

IFGraph_SubPartsIterator::~IFGraph_SubPartsIterator() 
{} 
