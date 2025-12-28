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
#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

// SubPartsIterator allows grouping entities into several sub-parts
// To each sub-part is attached a Status : the 1st has 1, the 2nd has 2, etc...
// (consequence, the sub-parts are necessarily disjoint)
IFGraph_SubPartsIterator::IFGraph_SubPartsIterator(const Interface_Graph& agraph, const bool whole)
    : thegraph(agraph)
{
  if (whole)
    thegraph.GetFromModel();
  theparts  = new NCollection_HSequence<int>();
  thefirsts = new NCollection_HSequence<int>();
  thepart   = 0;
  thecurr   = 0;
}

IFGraph_SubPartsIterator::IFGraph_SubPartsIterator(IFGraph_SubPartsIterator& other)
    : thegraph(other.Graph())
{
  int nb   = thegraph.Size();
  theparts = new NCollection_HSequence<int>();
  thepart  = 0;
  for (other.Start(); other.More(); other.Next())
  {
    thepart++;
    int nbent = 0;
    GetFromIter(other.Entities());
    for (int i = 1; i <= nb; i++)
    {
      if (thegraph.Status(i) == thepart)
        nbent++;
    }
    theparts->Append(nbent); // empty count
  }
  thepart = 0;
  thecurr = 1;
}

void IFGraph_SubPartsIterator::GetParts(IFGraph_SubPartsIterator& other)
{
  if (Model() != other.Model())
    throw Interface_InterfaceError("SubPartsIterator : GetParts");
  //  We ADD the Parts from other, without losing our own
  //  (same principle as the constructor above)
  int nb  = thegraph.Size();
  thepart = theparts->Length();
  for (other.Start(); other.More(); other.Next())
  {
    thepart++;
    int nbent = 0;
    GetFromIter(other.Entities());
    for (int i = 1; i <= nb; i++)
    {
      if (thegraph.Status(i) == thepart)
        nbent++;
    }
    theparts->Append(nbent); // empty count
  }
}

const Interface_Graph& IFGraph_SubPartsIterator::Graph() const
{
  return thegraph;
}

//  ....            Internal Management (filling, etc...)            .... //

occ::handle<Interface_InterfaceModel> IFGraph_SubPartsIterator::Model() const
{
  return thegraph.Model();
}

void IFGraph_SubPartsIterator::AddPart()
{
  theparts->Append(int(0));
  thepart = theparts->Length();
}

int IFGraph_SubPartsIterator::NbParts() const
{
  return theparts->Length();
}

int IFGraph_SubPartsIterator::PartNum() const
{
  return thepart;
}

void IFGraph_SubPartsIterator::SetLoad()
{
  thepart = 0;
}

void IFGraph_SubPartsIterator::SetPartNum(const int num)
{
  if (num <= 0 || num > theparts->Length())
    throw Standard_OutOfRange("IFGraph_SubPartsIterator : SetPartNum");
  thepart = num;
}

void IFGraph_SubPartsIterator::GetFromEntity(const occ::handle<Standard_Transient>& ent,
                                             const bool                             shared)
{
  thegraph.GetFromEntity(ent, shared, thepart, thepart, false);
}

void IFGraph_SubPartsIterator::GetFromIter(const Interface_EntityIterator& iter)
{
  thegraph.GetFromIter(iter, thepart, thepart, false);
}

void IFGraph_SubPartsIterator::Reset()
{
  thegraph.Reset();
  theparts->Clear();
  thepart = 0;
  thecurr = 0;
}

//  ....              Result (Evaluation, Iterations)              .... //

void IFGraph_SubPartsIterator::Evaluate() {} // by default, does nothing; redefined by subclasses

Interface_GraphContent IFGraph_SubPartsIterator::Loaded() const
{
  Interface_EntityIterator iter;
  //  int nb = thegraph.Size();
  return Interface_GraphContent(thegraph, 0);
}

Interface_Graph IFGraph_SubPartsIterator::LoadedGraph() const
{
  Interface_Graph G(Model());
  int             nb = thegraph.Size();
  for (int i = 1; i <= nb; i++)
  {
    if (thegraph.IsPresent(i) && thegraph.Status(i) == 0)
      G.GetFromEntity(thegraph.Entity(i), false);
  }
  return G;
}

bool IFGraph_SubPartsIterator::IsLoaded(const occ::handle<Standard_Transient>& ent) const
{
  return thegraph.IsPresent(thegraph.EntityNumber(ent));
}

bool IFGraph_SubPartsIterator::IsInPart(const occ::handle<Standard_Transient>& ent) const
{
  int num = thegraph.EntityNumber(ent);
  if (!thegraph.IsPresent(num))
    return false;
  return (thegraph.Status(num) != 0);
}

int IFGraph_SubPartsIterator::EntityPartNum(const occ::handle<Standard_Transient>& ent) const
{
  int num = thegraph.EntityNumber(ent);
  if (!thegraph.IsPresent(num))
    return 0;
  return thegraph.Status(num);
}

void IFGraph_SubPartsIterator::Start()
{
  Evaluate();
  //  Evaluate the sizes of the Parts contents
  int nb  = thegraph.Size();
  int nbp = theparts->Length();
  if (thepart > nbp)
    thepart = nbp;
  if (nbp == 0)
  {
    thecurr = 1;
    return;
  } // Iteration stops immediately

  //  - Perform counts (via arrays for performance)
  NCollection_Array1<int> partcounts(1, nbp);
  partcounts.Init(0);
  NCollection_Array1<int> partfirsts(1, nbp);
  partfirsts.Init(0);
  for (int i = 1; i <= nb; i++)
  {
    if (!thegraph.IsPresent(i))
      continue;
    int nump = thegraph.Status(i);
    if (nump < 1 || nump > nbp)
      continue;
    int nbent = partcounts.Value(nump);
    partcounts.SetValue(nump, nbent + 1);
    if (nbent == 0)
      partfirsts.SetValue(nump, i);
  }
  //  - Format them (i.e. in sequences)
  theparts->Clear();
  thefirsts->Clear();
  int lastp = 0;
  for (int np = 1; np <= nbp; np++)
  {
    int nbent = partcounts.Value(np);
    if (np != 0)
      lastp = np;
    theparts->Append(nbent);
    thefirsts->Append(partfirsts.Value(np));
  }
  if (lastp < nbp)
    theparts->Remove(lastp + 1, nbp);
  //  Finally, prepare to iterate
  thecurr = 1;
}

bool IFGraph_SubPartsIterator::More()
{
  if (thecurr == 0)
    Start();
  return (thecurr <= theparts->Length());
}

void IFGraph_SubPartsIterator::Next()
{
  thecurr++;
  if (thecurr > theparts->Length())
    return;
  if (theparts->Value(thecurr) == 0)
    Next(); // skip empty parts
}

bool IFGraph_SubPartsIterator::IsSingle() const
{
  if (thecurr < 1 || thecurr > theparts->Length())
    throw Standard_NoSuchObject("IFGraph_SubPartsIterator : IsSingle");
  return (theparts->Value(thecurr) == 1);
}

occ::handle<Standard_Transient> IFGraph_SubPartsIterator::FirstEntity() const
{
  if (thecurr < 1 || thecurr > theparts->Length())
    throw Standard_NoSuchObject("IFGraph_SubPartsIterator : FirstEntity");
  int nument = thefirsts->Value(thecurr);
  if (nument == 0)
    throw Standard_NoSuchObject("IFGraph_SubPartsIterator : FirstEntity (current part is empty)");
  return thegraph.Entity(nument);
}

Interface_EntityIterator IFGraph_SubPartsIterator::Entities() const
{
  if (thecurr < 1 || thecurr > theparts->Length())
    throw Standard_NoSuchObject("IFGraph_SubPartsIterator : Entities");
  Interface_EntityIterator iter;
  int                      nb     = thegraph.Size();
  int                      nument = thefirsts->Value(thecurr);
  if (nument == 0)
    return iter;
  if (theparts->Value(thecurr) == 1)
    nb = nument; // obvious: 1 single Entity
  for (int i = nument; i <= nb; i++)
  {
    if (thegraph.Status(i) == thecurr && thegraph.IsPresent(i))
      iter.GetOneItem(thegraph.Entity(i));
  }
  return iter;
}

//=================================================================================================

IFGraph_SubPartsIterator::~IFGraph_SubPartsIterator() = default;
