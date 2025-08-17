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

#include <Interface_EntityCluster.hxx>
#include <Interface_EntityIterator.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Sequence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_EntityCluster, Standard_Transient)

// A Cluster is 4 entities that follow each other (in principle, fixed number,
// even if not 4). They are filled from 0. There are therefore as many Entities
// as there are non-Null Handles, plus the fact that they are filled in order
// Thus (with Next), we consume 5 Handles for 4 Entities, with a spike
// for 1 and 2 Entities (we stay at 5 Handles)
// Deletion: We compact the Cluster so that the Nulls are always at the end
//  ....                        CONSTRUCTEURS                        ....
Interface_EntityCluster::Interface_EntityCluster() {}

Interface_EntityCluster::Interface_EntityCluster(const Handle(Standard_Transient)& ent)
{
  theents[0] = ent;
}

Interface_EntityCluster::Interface_EntityCluster(const Handle(Interface_EntityCluster)& ec)
{
  thenext = ec;
}

Interface_EntityCluster::Interface_EntityCluster(const Handle(Standard_Transient)&      ent,
                                                 const Handle(Interface_EntityCluster)& ec)
{
  theents[0] = ent;
  thenext    = ec;
}

//  ....                        AJOUT - SUPPRESSION                        ....

void Interface_EntityCluster::Append(const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull())
    throw Standard_NullObject("Interface_EntityCluster Append");
  if (theents[0].IsNull())
    theents[0] = ent;
  else if (theents[1].IsNull())
    theents[1] = ent;
  else if (theents[2].IsNull())
    theents[2] = ent;
  else if (theents[3].IsNull())
    theents[3] = ent;
  else
  { // If this one is full ...
    if (thenext.IsNull())
      thenext = new Interface_EntityCluster(ent);
    else
    {
      Handle(Interface_EntityCluster) aCurEntClust = thenext;
      while (aCurEntClust->HasNext() && aCurEntClust->IsLocalFull())
        aCurEntClust = aCurEntClust->thenext;
      aCurEntClust->Append(ent);
    }
  }
}

Standard_Boolean Interface_EntityCluster::Remove(const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull())
    throw Standard_NullObject("Interface_EntityCluster Remove");
  Standard_Integer i;
  //  Is <ent> here? if yes, we have its rank
  if (ent == theents[0])
    i = 1;
  else if (ent == theents[1])
    i = 2;
  else if (ent == theents[2])
    i = 3;
  else if (ent == theents[3])
    i = 4;

  //  Otherwise, go to the next one, which can then become empty ->
  //  We remove the empty cluster from the list (in principle it's the last one)
  else
  { // Not found in this one ...
    if (thenext.IsNull())
      return Standard_False;
    Standard_Integer res = thenext->Remove(ent);
    if (res)
      thenext = thenext->Next();
    return Standard_False;
  }
  return Remove(i);
}

Standard_Boolean Interface_EntityCluster::Remove(const Standard_Integer num)
{
  if (num < 1)
    throw Standard_OutOfRange("EntityCluster : Remove");
  Standard_Integer n = NbLocal();
  if (num > n)
  {
    if (thenext.IsNull())
      throw Standard_OutOfRange("EntityCluster : Remove");
    Standard_Boolean res = thenext->Remove(num - n);
    if (res)
      thenext = thenext->Next();
    return Standard_False;
  }
  for (Standard_Integer j = num; j < n; j--)
    theents[j - 1] = theents[j];
  theents[3].Nullify(); // We Nullify at the end
  return (n == 1);      // Old NbLocal == 1  -> becomes null
}

//  ....                        DATA ACCESS                        ....

Standard_Integer Interface_EntityCluster::NbEntities() const
{
  Standard_Integer nb = NbLocal();
  if (!thenext.IsNull())
    nb += thenext->NbEntities();
  return nb;
}

const Handle(Standard_Transient)& Interface_EntityCluster::Value(const Standard_Integer num) const
{
  Standard_Integer nb = NbLocal(), aLocalNum = num;
  if (num <= 0)
    throw Standard_OutOfRange("Interface EntityCluster : Value");
  if (num > nb)
  {
    Handle(Interface_EntityCluster) aCurEntClust = thenext;
    aLocalNum -= nb;
    while (aLocalNum > aCurEntClust->NbLocal())
    {
      if (!aCurEntClust->HasNext())
        throw Standard_OutOfRange("Interface EntityCluster : Value");
      aCurEntClust = aCurEntClust->thenext;
      aLocalNum -= nb;
    }
    return aCurEntClust->theents[aLocalNum - 1];
  }
  return theents[num - 1]; // numbering from 0
}

void Interface_EntityCluster::SetValue(const Standard_Integer            num,
                                       const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull())
    throw Standard_NullObject("Interface_EntityCluster SetValue");
  Standard_Integer nb = NbLocal(), aLocalNum = num;
  if (num <= 0)
    throw Standard_OutOfRange("Interface EntityCluster : SetValue");
  if (num > nb)
  {
    Handle(Interface_EntityCluster) aCurEntClust = thenext;
    aLocalNum -= nb;
    while (aLocalNum > aCurEntClust->NbLocal())
    {
      if (thenext.IsNull())
        throw Standard_OutOfRange("Interface EntityCluster : SetValue");
      aCurEntClust = aCurEntClust->thenext;
      aLocalNum -= nb;
    }
    aCurEntClust->theents[aLocalNum - 1] = ent;
  }
  else
    theents[num - 1] = ent; // numbering from 0
}

void Interface_EntityCluster::FillIterator(Interface_EntityIterator& iter) const
{
  if (!theents[0].IsNull())
    iter.GetOneItem(theents[0]);
  if (!theents[1].IsNull())
    iter.GetOneItem(theents[1]);
  if (!theents[2].IsNull())
    iter.GetOneItem(theents[2]);
  if (!theents[3].IsNull())
    iter.GetOneItem(theents[3]);
  if (!thenext.IsNull())
    thenext->FillIterator(iter);
}

//  ....                    Actions atomiques internes                    ....

Standard_Boolean Interface_EntityCluster::IsLocalFull() const
{
  // Solaris Forte C++ compiler insisted it couldn't cast this,
  // even though it seems to do so elsewhere
  Handle(Standard_Transient) tmp = Handle(Standard_Transient)(theents[3]);
  return (!tmp.IsNull());
}

Standard_Integer Interface_EntityCluster::NbLocal() const
{
  Standard_Integer nb;
  if (!theents[3].IsNull())
    nb = 4;
  else if (!theents[2].IsNull())
    nb = 3;
  else if (!theents[1].IsNull())
    nb = 2;
  else if (!theents[0].IsNull())
    nb = 1;
  else
    nb = 0;
  return nb;
}

Standard_Boolean Interface_EntityCluster::HasNext() const
{
  return (!thenext.IsNull());
}

Handle(Interface_EntityCluster) Interface_EntityCluster::Next() const
{
  return thenext;
}

Interface_EntityCluster::~Interface_EntityCluster()
{
  if (!thenext.IsNull())
  {
    // Loading entities into the collection
    // for deletion in reverse order(avoiding the recursion)
    NCollection_Sequence<Handle(Interface_EntityCluster)> aNColOfEntClust;
    Handle(Interface_EntityCluster)                       aCurEntClust = thenext;
    while (aCurEntClust->HasNext())
    {
      aNColOfEntClust.Append(aCurEntClust);
      aCurEntClust = aCurEntClust->Next();
    }
    aNColOfEntClust.Append(aCurEntClust);
    aNColOfEntClust.Reverse();
    for (NCollection_Sequence<Handle(Interface_EntityCluster)>::Iterator anEntClustIter(
           aNColOfEntClust);
         anEntClustIter.More();
         anEntClustIter.Next())
    {
      // Nullifying and destruction all fields of each entity in the collection
      for (Standard_Integer anInd = 0; anInd < anEntClustIter.ChangeValue()->NbLocal(); ++anInd)
      {
        anEntClustIter.ChangeValue()->theents[anInd].Nullify();
      }
      anEntClustIter.ChangeValue()->thenext.Nullify();
    }
  }
  for (Standard_Integer anInd = 0; anInd < NbLocal(); ++anInd)
  {
    theents[anInd].Nullify();
  }
  thenext.Nullify();
}
