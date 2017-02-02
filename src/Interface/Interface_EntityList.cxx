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

// Une EntityList, c est au fond un "Handle" bien entoure :
// S il est nul, la liste est vide
// Si c est une Entite, la liste comprend cette entite et rien d autre
// Si c est un EntityCluster, il definit (avec ses Next eventuels) le contenu
// de la liste
Interface_EntityList::Interface_EntityList ()    {  }

    void  Interface_EntityList::Clear ()
      {  theval.Nullify();  }

//  ....                EDITIONS (ajout-suppression)                ....

    void  Interface_EntityList::Append
  (const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull()) throw Standard_NullObject("Interface_EntityList Append");
  if (theval.IsNull()) {  theval = ent;  return;  }
  Handle(Interface_EntityCluster) aValEC =
    Handle(Interface_EntityCluster)::DownCast(theval);
  if (!aValEC.IsNull()) aValEC->Append(ent);    // EntityCluster
  else {                                // reste InterfaceEntity ...
    Handle(Interface_EntityCluster) ec = new Interface_EntityCluster(theval);
    ec->Append(ent);
    theval = ec;
  }
}

// Difference avec Append : on optimise, en evitant la recursivite
// En effet, quand un EntityCluster est plein, Append transmet au Next
// Ici, EntityList garde le controle, le temps de traitement reste le meme
// Moyennant quoi, l ordre n est pas garanti

    void  Interface_EntityList::Add
  (const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull()) throw Standard_NullObject("Interface_EntityList Add");
  if (theval.IsNull()) {  theval = ent;  return;  }
  Handle(Interface_EntityCluster) aValEC =
    Handle(Interface_EntityCluster)::DownCast(theval);
  if (!aValEC.IsNull()) {               // EntityCluster
    if (aValEC->IsLocalFull()) theval = new Interface_EntityCluster(ent, aValEC);
    else aValEC->Append (ent);
  } else {                          // reste InterfaceEntity ...
    Handle(Interface_EntityCluster) ec = new Interface_EntityCluster(theval);
    ec->Append(ent);
    theval = ec;
  }
}

//  Remove : Par Identification d Item a supprimer, ou par Rang
//  Identification : Item supprime ou qu il soit
//  N.B.: La liste peut devenir vide ... cf retour Remove de Cluster

    void  Interface_EntityList::Remove (const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull()) throw Standard_NullObject("Interface_EntityList Remove");
  if (theval.IsNull()) return;
  if (theval == ent) {
    theval.Nullify();
    return;
  }
  Handle(Interface_EntityCluster) ec =
    Handle(Interface_EntityCluster)::DownCast(theval);
  if (ec.IsNull()) return;   // Une seule Entite et pas la bonne
  Standard_Boolean res = ec->Remove(ent);
  if (res) theval.Nullify();
}

//  Remove par rang : tester OutOfRange

    void  Interface_EntityList::Remove  (const Standard_Integer num)
{
  if (theval.IsNull()) throw Standard_OutOfRange("EntityList : Remove");
  Handle(Interface_EntityCluster) ec =
    Handle(Interface_EntityCluster)::DownCast(theval);
  if (ec.IsNull()) {
    if (num != 1) throw Standard_OutOfRange("EntityList : Remove");
    theval.Nullify();
    return;
  }
  Standard_Boolean res = ec->Remove(num);
  if (res) theval.Nullify();
}

//  ....                    ACCES Unitaire AUX DONNEES                    ....

    Standard_Boolean  Interface_EntityList::IsEmpty () const 
      {  return (theval.IsNull());  }

    Standard_Integer  Interface_EntityList::NbEntities () const 
{
  if (theval.IsNull()) return 0;
  Handle(Interface_EntityCluster) ec =
    Handle(Interface_EntityCluster)::DownCast(theval);
  if (ec.IsNull()) return 1;   // Une seuke Entite
  return ec->NbEntities();
}


    const Handle(Standard_Transient)&  Interface_EntityList::Value
  (const Standard_Integer num) const 
{
  if (theval.IsNull()) throw Standard_OutOfRange("Interface EntityList : Value");
  Handle(Interface_EntityCluster) ec =
    Handle(Interface_EntityCluster)::DownCast(theval);
  if (!ec.IsNull()) return ec->Value(num);  // EntityCluster
  else if (num != 1) throw Standard_OutOfRange("Interface EntityList : Value");
  return theval;
}

    void  Interface_EntityList::SetValue
  (const Standard_Integer num, const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull()) throw Standard_NullObject("Interface_EntityList SetValue");
  if (theval.IsNull()) throw Standard_OutOfRange("Interface EntityList : SetValue");
  Handle(Interface_EntityCluster) ec =
    Handle(Interface_EntityCluster)::DownCast(theval);
  if (!ec.IsNull()) ec->SetValue(num,ent);   // EntityCluster
  else if (num != 1) throw Standard_OutOfRange("Interface EntityList : SetValue");
  else theval = ent;

}

//  ....                Interrogations Generales                ....

    void  Interface_EntityList::FillIterator
  (Interface_EntityIterator& iter) const 
{
  if (theval.IsNull()) return;
  Handle(Interface_EntityCluster) ec =
    Handle(Interface_EntityCluster)::DownCast(theval);
  if (!ec.IsNull()) ec->FillIterator(iter);    // EntityCluster;
  else iter.GetOneItem(theval);
}


    Standard_Integer Interface_EntityList::NbTypedEntities
  (const Handle(Standard_Type)& atype) const
{
  Standard_Integer res = 0;
  if (theval.IsNull()) return 0;
  Handle(Interface_EntityCluster) ec =
    Handle(Interface_EntityCluster)::DownCast(theval);
  if (!ec.IsNull()) {                // EntityCluster
    while (!ec.IsNull()) {
      for (Standard_Integer i = ec->NbLocal(); i > 0; i --) {
	if (ec->Value(i)->IsKind(atype)) res ++;
      }
      if (!ec->HasNext()) break;
      ec = ec->Next();
    }
  } else {                           // Une seule Entite
    if (theval->IsKind(atype)) res = 1;
  }
  return res;
}

    Handle(Standard_Transient) Interface_EntityList::TypedEntity
  (const Handle(Standard_Type)& atype, const Standard_Integer num) const
{
  Standard_Integer res = 0;
  Handle(Standard_Transient) entres;
  if (theval.IsNull()) throw Interface_InterfaceError("Interface EntityList : TypedEntity , none found");
  Handle(Interface_EntityCluster) ec =
    Handle(Interface_EntityCluster)::DownCast(theval);
  if (!ec.IsNull()) {                // EntityCluster
    while (!ec.IsNull()) {
      for (Standard_Integer i = ec->NbLocal(); i > 0; i --) {
	if (ec->Value(i)->IsKind(atype)) {
	  res ++;
	  if (num == 0 && res > 1) throw Interface_InterfaceError("Interface EntityList : TypedEntity , several found");
	  entres = ec->Value(i);
	  if (res == num) return entres;
	}
      }
      if (!ec->HasNext()) break;
      ec = ec->Next();
    }
  } else if (num > 1) {
    throw Interface_InterfaceError("Interface EntityList : TypedEntity ,out of range");
  } else {                          // InterfaceEntity
    if (!theval->IsKind(atype)) throw Interface_InterfaceError("Interface EntityList : TypedEntity , none found");
    entres = theval;
  }
  return entres;
}
