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
#include <Interface_EntityList.hxx>
#include <Interface_InterfaceError.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>

// An EntityList is basically a well-wrapped "Handle":
// If it is null, the list is empty
// If it is an Entity, the list includes this entity and nothing else
// If it is an EntityCluster, it defines (with its possible Next) the content
// of the list
Interface_EntityList::Interface_EntityList() {}

void Interface_EntityList::Clear()
{
  theval.Nullify();
}

//  ....                EDITIONS (add-remove)                ....

void Interface_EntityList::Append(const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull())
    throw Standard_NullObject("Interface_EntityList Append");
  if (theval.IsNull())
  {
    theval = ent;
    return;
  }
  Handle(Interface_EntityCluster) aValEC = Handle(Interface_EntityCluster)::DownCast(theval);
  if (!aValEC.IsNull())
    aValEC->Append(ent); // EntityCluster
  else
  { // reste InterfaceEntity ...
    Handle(Interface_EntityCluster) ec = new Interface_EntityCluster(theval);
    ec->Append(ent);
    theval = ec;
  }
}

// Difference with Append : we optimize, avoiding recursion
// Indeed, when an EntityCluster is full, Append transmits to Next
// Here, EntityList keeps control, the processing time remains the same
// With which, the order is not guaranteed

void Interface_EntityList::Add(const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull())
    throw Standard_NullObject("Interface_EntityList Add");
  if (theval.IsNull())
  {
    theval = ent;
    return;
  }
  Handle(Interface_EntityCluster) aValEC = Handle(Interface_EntityCluster)::DownCast(theval);
  if (!aValEC.IsNull())
  { // EntityCluster
    if (aValEC->IsLocalFull())
      theval = new Interface_EntityCluster(ent, aValEC);
    else
      aValEC->Append(ent);
  }
  else
  { // reste InterfaceEntity ...
    Handle(Interface_EntityCluster) ec = new Interface_EntityCluster(theval);
    ec->Append(ent);
    theval = ec;
  }
}

//  Remove : By Identification of Item to remove, or by Rank
//  Identification : Item removed wherever it is
//  N.B.: The list can become empty ... cf return Remove from Cluster

void Interface_EntityList::Remove(const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull())
    throw Standard_NullObject("Interface_EntityList Remove");
  if (theval.IsNull())
    return;
  if (theval == ent)
  {
    theval.Nullify();
    return;
  }
  Handle(Interface_EntityCluster) ec = Handle(Interface_EntityCluster)::DownCast(theval);
  if (ec.IsNull())
    return; // A single Entity and not the right one
  Standard_Boolean res = ec->Remove(ent);
  if (res)
    theval.Nullify();
}

//  Remove by rank : test OutOfRange

void Interface_EntityList::Remove(const Standard_Integer num)
{
  if (theval.IsNull())
    throw Standard_OutOfRange("EntityList : Remove");
  Handle(Interface_EntityCluster) ec = Handle(Interface_EntityCluster)::DownCast(theval);
  if (ec.IsNull())
  {
    if (num != 1)
      throw Standard_OutOfRange("EntityList : Remove");
    theval.Nullify();
    return;
  }
  Standard_Boolean res = ec->Remove(num);
  if (res)
    theval.Nullify();
}

//  ....                    UNIT ACCESS TO DATA                    ....

Standard_Boolean Interface_EntityList::IsEmpty() const
{
  return (theval.IsNull());
}

Standard_Integer Interface_EntityList::NbEntities() const
{
  if (theval.IsNull())
    return 0;
  Handle(Interface_EntityCluster) ec = Handle(Interface_EntityCluster)::DownCast(theval);
  if (ec.IsNull())
    return 1; // A single Entity
  return ec->NbEntities();
}

const Handle(Standard_Transient)& Interface_EntityList::Value(const Standard_Integer num) const
{
  if (theval.IsNull())
    throw Standard_OutOfRange("Interface EntityList : Value");
  Handle(Interface_EntityCluster) ec = Handle(Interface_EntityCluster)::DownCast(theval);
  if (!ec.IsNull())
    return ec->Value(num); // EntityCluster
  else if (num != 1)
    throw Standard_OutOfRange("Interface EntityList : Value");
  return theval;
}

void Interface_EntityList::SetValue(const Standard_Integer            num,
                                    const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull())
    throw Standard_NullObject("Interface_EntityList SetValue");
  if (theval.IsNull())
    throw Standard_OutOfRange("Interface EntityList : SetValue");
  Handle(Interface_EntityCluster) ec = Handle(Interface_EntityCluster)::DownCast(theval);
  if (!ec.IsNull())
    ec->SetValue(num, ent); // EntityCluster
  else if (num != 1)
    throw Standard_OutOfRange("Interface EntityList : SetValue");
  else
    theval = ent;
}

//  ....                Interrogations Generales                ....

void Interface_EntityList::FillIterator(Interface_EntityIterator& iter) const
{
  if (theval.IsNull())
    return;
  Handle(Interface_EntityCluster) ec = Handle(Interface_EntityCluster)::DownCast(theval);
  if (!ec.IsNull())
    ec->FillIterator(iter); // EntityCluster;
  else
    iter.GetOneItem(theval);
}

Standard_Integer Interface_EntityList::NbTypedEntities(const Handle(Standard_Type)& atype) const
{
  Standard_Integer res = 0;
  if (theval.IsNull())
    return 0;
  Handle(Interface_EntityCluster) ec = Handle(Interface_EntityCluster)::DownCast(theval);
  if (!ec.IsNull())
  { // EntityCluster
    while (!ec.IsNull())
    {
      for (Standard_Integer i = ec->NbLocal(); i > 0; i--)
      {
        if (ec->Value(i)->IsKind(atype))
          res++;
      }
      if (!ec->HasNext())
        break;
      ec = ec->Next();
    }
  }
  else
  { // Une seule Entite
    if (theval->IsKind(atype))
      res = 1;
  }
  return res;
}

Handle(Standard_Transient) Interface_EntityList::TypedEntity(const Handle(Standard_Type)& atype,
                                                             const Standard_Integer       num) const
{
  Standard_Integer           res = 0;
  Handle(Standard_Transient) entres;
  if (theval.IsNull())
    throw Interface_InterfaceError("Interface EntityList : TypedEntity , none found");
  Handle(Interface_EntityCluster) ec = Handle(Interface_EntityCluster)::DownCast(theval);
  if (!ec.IsNull())
  { // EntityCluster
    while (!ec.IsNull())
    {
      for (Standard_Integer i = ec->NbLocal(); i > 0; i--)
      {
        if (ec->Value(i)->IsKind(atype))
        {
          res++;
          if (num == 0 && res > 1)
            throw Interface_InterfaceError("Interface EntityList : TypedEntity , several found");
          entres = ec->Value(i);
          if (res == num)
            return entres;
        }
      }
      if (!ec->HasNext())
        break;
      ec = ec->Next();
    }
  }
  else if (num > 1)
  {
    throw Interface_InterfaceError("Interface EntityList : TypedEntity ,out of range");
  }
  else
  { // InterfaceEntity
    if (!theval->IsKind(atype))
      throw Interface_InterfaceError("Interface EntityList : TypedEntity , none found");
    entres = theval;
  }
  return entres;
}
