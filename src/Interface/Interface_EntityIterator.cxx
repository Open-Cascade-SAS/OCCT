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


#include <Interface_EntityIterator.hxx>
#include <Interface_IntVal.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>

//  Iterateur pour ecriture for, ou while avec Next en fin :
//  for (creer iterateur; iter.More(); iter.Next()) { val = iter.Value(); ... }
// .... Definitions initiales : en particulier celles requises pour
//      les outils de graphe (construction avec le graphe, avec un vertex)
Interface_EntityIterator::Interface_EntityIterator ()
{
//  thecurr = new Interface_IntVal;
//  thecurr->CValue() = 0;
//  thelist = new TColStd_HSequenceOfTransient();  // constructeur vide
//  thelist sera construit au premier Add (quelquefois, il nyena pas)
}

    Interface_EntityIterator::Interface_EntityIterator
  (const Handle(TColStd_HSequenceOfTransient)& list)
{
  thecurr = new Interface_IntVal;
  thecurr->CValue() = 0;
  thelist = list;
}

    void Interface_EntityIterator::AddList
  (const Handle(TColStd_HSequenceOfTransient)& list)
{
  if (thelist.IsNull()) thelist = new TColStd_HSequenceOfTransient();
  if (thecurr.IsNull()) thecurr = new Interface_IntVal;
  thecurr->CValue() = 0;
  thelist->Append(list);
}

    void Interface_EntityIterator::AddItem
  (const Handle(Standard_Transient)& anentity)
{
  if (anentity.IsNull()) return;
  if (thecurr.IsNull()) thecurr = new Interface_IntVal;
  if (thelist.IsNull()) thelist = new TColStd_HSequenceOfTransient();
  thecurr->CValue() = 0;
  thelist->Append(anentity);
}

    void Interface_EntityIterator::GetOneItem
  (const Handle(Standard_Transient)& anentity)
      {  AddItem(anentity);  }

    void Interface_EntityIterator::Reset ()
{
  if (thecurr.IsNull()) thecurr = new Interface_IntVal;
  thecurr->CValue() = 0;
  thelist = new TColStd_HSequenceOfTransient();
}


// .... Fonctionnalites de tri prealable a l'iteration ....

//  Facon "bete" : supprimer les termes qui ne conviennent pas : lent !
//  Mieux vaut refaire une autre sequence a cote

    void Interface_EntityIterator::SelectType
  (const Handle(Standard_Type)& atype, const Standard_Boolean keep)
{
  if (thelist.IsNull()) return;
  Standard_Integer i, n = thelist->Length();
  Handle(TColStd_HSequenceOfTransient) nlist = new TColStd_HSequenceOfTransient();
  for (i = 1 ; i <= n ; i ++) {
    if (thelist->Value(i)->IsKind(atype) == keep) nlist->Append(thelist->Value(i));
  }
  thelist = nlist;
}

//  ....  Iteration proprement dite  ....

    Standard_Integer Interface_EntityIterator::NbEntities () const
{
  if (thelist.IsNull()) return 0;
  if (!thecurr.IsNull() && thecurr->Value() == 0) Start();
  return thelist->Length();
}

    Standard_Integer Interface_EntityIterator::NbTyped
  (const Handle(Standard_Type)& atype) const
{
  Standard_Integer res = 0;
  if (thelist.IsNull()) return res;
  Standard_Integer i, n = thelist->Length();
  for (i = 1 ; i <= n ; i ++) {
    if (thelist->Value(i)->IsKind(atype)) res ++;
  }
  return res;
}

    Interface_EntityIterator  Interface_EntityIterator::Typed
  (const Handle(Standard_Type)& atype) const
{
  Interface_EntityIterator res;
  if (thelist.IsNull()) return res;
  Standard_Integer i, n = thelist->Length();
  for (i = 1 ; i <= n ; i ++) {
    if (thelist->Value(i)->IsKind(atype)) res.AddItem (thelist->Value(i));
  }
  return res;
}


    void Interface_EntityIterator::Start () const
      {    if (!thecurr.IsNull()) thecurr->CValue() = 1 ;  }     // peut etre redefini ...

    Standard_Boolean Interface_EntityIterator::More () const
{
  if (thecurr.IsNull()) return Standard_False;
  if (thecurr->Value() == 0) Start();  // preparation de l iteration
  if (thelist.IsNull()) return Standard_False;
  return (thecurr->Value() <= thelist->Length());
}

    void Interface_EntityIterator::Next () const
      {    thecurr->CValue() ++;  }    // Next ne verifie rien : soin laisse a Value

    const Handle(Standard_Transient)& Interface_EntityIterator::Value () const
{
//  NbEntity pas const (on ne sait pas comment il est implemente apres tout)
  if (thelist.IsNull()) throw Standard_NoSuchObject("Interface_EntityIterator");
  if (thecurr->Value() < 1 || thecurr->Value() > thelist->Length())
    throw Standard_NoSuchObject("Interface_EntityIterator");
  return thelist->Value(thecurr->Value());
}

    Handle(TColStd_HSequenceOfTransient)  Interface_EntityIterator::Content () const
{
  if (!thecurr.IsNull() && thecurr->Value() == 0) Start();
  if (thelist.IsNull()) return new TColStd_HSequenceOfTransient();  // vide
  return thelist;
}

void  Interface_EntityIterator::Destroy ()
      {  thecurr.Nullify();  }  // redevient vide !

Interface_EntityIterator::~Interface_EntityIterator()
{
  Destroy();
}

