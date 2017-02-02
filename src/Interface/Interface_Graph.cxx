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


#include <Interface_BitMap.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_Graph.hxx>
#include <Interface_GTool.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReportEntity.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

// Flags : 0 = Presence, 1 = Sharing Error
#define Graph_Present 0
#define Graph_ShareError 1


//  ###########################################################################

//  ....                           CONSTRUCTEURS                           ....

//  ....       Construction a partir de la connaissance des Entites        ....


Interface_Graph::Interface_Graph
(const Handle(Interface_InterfaceModel)& amodel,
 const Interface_GeneralLib& /*lib*/,
 Standard_Boolean theModeStat)
 : themodel   (amodel), thepresents ("") 
{
  if(theModeStat)
    InitStats();
  Evaluate();
}

Interface_Graph::Interface_Graph
(const Handle(Interface_InterfaceModel)& amodel,
 const Handle(Interface_Protocol)& /*protocol*/,
 Standard_Boolean theModeStat)
 : themodel   (amodel) , thepresents ("")    

{
 if(theModeStat)
    InitStats();  
  Evaluate();
}

Interface_Graph::Interface_Graph
(const Handle(Interface_InterfaceModel)& amodel,
 const Handle(Interface_GTool)& /*gtool*/,
 Standard_Boolean theModeStat)
 : themodel   (amodel) , thepresents ("") 
{
  if(theModeStat)
    InitStats();
  Evaluate();
}

Interface_Graph::Interface_Graph
(const Handle(Interface_InterfaceModel)& amodel,
 Standard_Boolean theModeStat)
: themodel   (amodel) , thepresents ("")  
{
  if(theModeStat)
    InitStats();
  Evaluate ();
}

//  ....                Construction depuis un autre Graph                ....

Interface_Graph::Interface_Graph
(const Interface_Graph& agraph, const Standard_Boolean /*copied*/)
: themodel   (agraph.Model()), thepresents ("") 
{
  thesharings = agraph.SharingTable();
  Standard_Integer nb = agraph.NbStatuses();
  if(!nb)
    return;
  if(thestats.IsNull())
    thestats = new TColStd_HArray1OfInteger(1,nb);
  for (Standard_Integer i = 1; i <= nb; i ++)
    thestats->SetValue (i,agraph.Status(i));
  theflags.Initialize(agraph.BitMap(),Standard_True);
}

void Interface_Graph::InitStats()
{
  thestats = new TColStd_HArray1OfInteger(1,themodel->NbEntities()) , 
    theflags.Initialize(themodel->NbEntities(),2);
  theflags.AddFlag ("ShareError");
}

Standard_Integer Interface_Graph::NbStatuses() const
{
  return (thestats.IsNull() ? 0 : thestats->Length());
}

const Handle(TColStd_HArray1OfListOfInteger)& Interface_Graph::SharingTable () const
{  return thesharings;  }

void Interface_Graph::Evaluate()
{
  //  Evaluation d un Graphe de dependances : sur chaque Entite, on prend sa
  //  liste "Shared". On en deduit les "Sharing"  directement
  Standard_Integer n = Size();
  thesharings = new TColStd_HArray1OfListOfInteger(1,n);//TColStd_HArray1OfTransient(1,n);//Clear();
  if(themodel->GTool().IsNull())
    return;
  

  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= n; i ++) {
     //    ATTENTION : Si Entite non chargee donc illisible, basculer sur son
    //    "Contenu" equivalent
    Handle(Standard_Transient) ent = themodel->Value(i);
  

    //    Resultat obtenu via GeneralLib
    Interface_EntityIterator iter = GetShareds(ent);

    //    Mise en forme : liste d entiers
    for (iter.Start(); iter.More(); iter.Next()) {
      //    num = 0 -> on sort du Model de depart, le noter "Error" et passer
      Handle(Standard_Transient) entshare = iter.Value();
      if(entshare == ent)
        continue;

      Standard_Integer num = EntityNumber(entshare);

      if (!num )  
      {
        if(!thestats.IsNull())
          theflags.SetTrue (i,Graph_ShareError);
        continue;
      }
      thesharings->ChangeValue(num).Append(i);
    }
  }
}

//  ....                Construction depuis un autre Graph                ....


//  ###########################################################################

//  ....                ACCES UNITAIRES AUX DONNEES DE BASE                ....

void  Interface_Graph::Reset ()
{
  if(!thestats.IsNull())
  {
    thestats.Nullify();
    theflags.Init (Standard_False, Graph_Present);
  }

}

void  Interface_Graph::ResetStatus ()
{ 
  if(!thestats.IsNull())
  {
    thestats->Init(0);  
    theflags.Init (Standard_False, Graph_Present);
  }
}


Standard_Integer  Interface_Graph::Size () const 
{  return themodel->NbEntities(); }//thestats.Upper();  }

Standard_Integer  Interface_Graph::EntityNumber
(const Handle(Standard_Transient)& ent) const 
{  return themodel->Number(ent);  }

Standard_Boolean  Interface_Graph::IsPresent
(const Standard_Integer num) const 
{
  if (num <= 0 || num > Size()) 
    return Standard_False;
  return (!thestats.IsNull() ? theflags.Value (num,Graph_Present) : Standard_False);
}

Standard_Boolean  Interface_Graph::IsPresent
(const Handle(Standard_Transient)& ent) const 
{  return IsPresent(EntityNumber(ent));  }

const Handle(Standard_Transient)&  Interface_Graph::Entity
(const Standard_Integer num) const 
{  
  return themodel->Value(num);  
}


Standard_Integer  Interface_Graph::Status (const Standard_Integer num) const
{  
  return (!thestats.IsNull() ? thestats->Value(num) : 0);  
}

void  Interface_Graph::SetStatus
(const Standard_Integer num, const Standard_Integer stat)
{  
  if(!thestats.IsNull())
    thestats->SetValue(num,stat);  
}

void  Interface_Graph::RemoveItem(const Standard_Integer num)
{
  if(!thestats.IsNull())
  {
    thestats->SetValue(num,0);  
    theflags.SetFalse (num,Graph_Present);
  }
}

void  Interface_Graph::ChangeStatus
(const Standard_Integer oldstat, const Standard_Integer newstat)
{
  if(thestats.IsNull())
    return;
  Standard_Integer nb = thestats->Upper();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thestats->Value(i) == oldstat) 
      thestats->SetValue(i,newstat);
  }
}

void  Interface_Graph::RemoveStatus(const Standard_Integer stat)
{
  if(thestats.IsNull())
    return;
  Standard_Integer nb = thestats->Upper();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thestats->Value(i) == stat) RemoveItem(i);
  }
}

const Interface_BitMap&  Interface_Graph::BitMap () const
{  return theflags;  }

Interface_BitMap&  Interface_Graph::CBitMap ()
{  return theflags;  }

//  ###########################################################################

//  ....      Chargements Elementaires avec Propagation de "Share"      .... //

const Handle(Interface_InterfaceModel)&  Interface_Graph::Model() const 
{  return themodel;  }

void  Interface_Graph::GetFromModel ()
{
  if (themodel.IsNull() || thestats.IsNull()) 
    return;    // no model ... (-> on n ira pas loin)
  theflags.Init (Standard_True,Graph_Present);
  thestats->Init (0);
}

void  Interface_Graph::GetFromEntity
(const Handle(Standard_Transient)& ent, const Standard_Boolean shared,
 const Standard_Integer newstat)
{
  if(thestats.IsNull())
    return;
  Standard_Integer num = EntityNumber(ent);
  if (!num ) 
    return;
  if (theflags.CTrue(num,Graph_Present)) return;  // deja pris : on passe
  thestats->SetValue(num,newstat);
  if (!shared) return;
  //  Attention a la redefinition !
  Interface_EntityIterator aIter = GetShareds(ent);

  for ( ; aIter.More() ; aIter.Next())    
    GetFromEntity(aIter.Value(),Standard_True,newstat);
}

void  Interface_Graph::GetFromEntity
(const Handle(Standard_Transient)& ent, const Standard_Boolean shared,
 const Standard_Integer newstat, const Standard_Integer overlapstat,
 const Standard_Boolean cumul)
{
  if(thestats.IsNull())
    return;
  Standard_Integer num   = EntityNumber(ent);
  if (!num ) return;
  Standard_Boolean pasla = !theflags.CTrue (num,Graph_Present);
  Standard_Integer stat  = thestats->Value(num); 

  if (pasla) {
    ///    theflags.SetTrue (num, Graph_Present);   // nouveau : noter avec newstat
    thestats->SetValue(num,newstat);
  } else {
    Standard_Integer overstat = stat;
    if (stat != newstat) {                   // deja pris, meme statut : passer
      if (cumul) overstat += overlapstat;    // nouveau statut : avec cumul ...
      else       overstat  = overlapstat;    // ... ou sans (statut force)
      if (stat != overstat)                  // si repasse deja faite, passer
        thestats->SetValue(num,overstat);
    }
  }
  if (!shared) return;
  //  Attention a la redefinition !
  Interface_EntityIterator aIter = GetShareds(ent);

  for ( ; aIter.More() ; aIter.Next())    
    GetFromEntity(aIter.Value(),Standard_True,newstat);
}

void  Interface_Graph::GetFromIter
(const Interface_EntityIterator& iter, const Standard_Integer newstat)
{
   if(thestats.IsNull())
    return;
  for (iter.Start(); iter.More(); iter.Next()) {
    Handle(Standard_Transient) ent = iter.Value();
    Standard_Integer num = EntityNumber(ent);
    if (!num) 
      continue;
    if (theflags.CTrue(num,Graph_Present)) 
      continue;
    thestats->SetValue(num,newstat);
  }
}


void  Interface_Graph::GetFromIter
(const Interface_EntityIterator& iter,
 const Standard_Integer newstat, const Standard_Integer overlapstat,
 const Standard_Boolean cumul)
{
  if(thestats.IsNull())
    return;
  for (iter.Start(); iter.More(); iter.Next()) {
    Handle(Standard_Transient) ent = iter.Value();
    Standard_Integer num   = EntityNumber(ent);
    if (!num) 
      continue;
    /*Standard_Boolean pasla = !*/theflags.Value(num,Graph_Present);
    /*Standard_Integer stat  = */thestats->Value(num); 
    GetFromEntity (ent,Standard_False,newstat,overlapstat,cumul);
  }
}


void Interface_Graph::GetFromGraph (const Interface_Graph& agraph)
{
  if (Model() != agraph.Model()) throw Standard_DomainError("Graph from Interface : GetFromGraph");
  Standard_Integer nb = Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (agraph.IsPresent(i))
      GetFromEntity (agraph.Entity(i),Standard_False,agraph.Status(i));
  }
}

void Interface_Graph::GetFromGraph
(const Interface_Graph& agraph, const Standard_Integer stat)
{
  if (Model() != agraph.Model()) throw Standard_DomainError("Graph from Interface : GetFromGraph");
  Standard_Integer nb = Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (agraph.IsPresent(i) && agraph.Status(i) == stat)
      GetFromEntity (agraph.Entity(i),Standard_False,stat);
  }
}

//  #####################################################################

//  ....                Listage des Entites Partagees                ....

Standard_Boolean Interface_Graph::HasShareErrors
(const Handle(Standard_Transient)& ent) const
{
  if(thestats.IsNull())
    return Standard_False;
  Standard_Integer num   = EntityNumber(ent);
  if (num == 0) return Standard_True;
  return theflags.Value (num,Graph_ShareError);
}

Interface_EntityIterator Interface_Graph::Shareds
(const Handle(Standard_Transient)& ent) const
{
  Interface_EntityIterator iter;
  Standard_Integer num   = EntityNumber(ent);
  if(!num)
    return iter;

  Handle(Standard_Transient) aCurEnt =  ent;
  if (themodel->IsRedefinedContent(num)) 
     aCurEnt = themodel->ReportEntity(num)->Content();

  //if (num == 0)  throw Standard_DomainError("Interface : Shareds");
  Handle(Interface_GeneralModule) module;
  Standard_Integer CN;
  if (themodel->GTool()->Select(aCurEnt,module,CN))  
    module->FillShared(themodel,CN,aCurEnt,iter);
  return iter;
}

Handle(TColStd_HSequenceOfTransient) Interface_Graph::GetShareds(const Handle(Standard_Transient)& ent) const
{
  Handle(TColStd_HSequenceOfTransient) aseq = new TColStd_HSequenceOfTransient;
  Interface_EntityIterator iter = Shareds(ent);
  for( ; iter.More(); iter.Next())
    aseq->Append(iter.Value());
  return aseq;
}

Handle(TColStd_HSequenceOfTransient) Interface_Graph::GetSharings(const Handle(Standard_Transient)& ent) const
{
  Standard_Integer num   = EntityNumber(ent);
  if(!num)
    return 0;
  //return Handle(TColStd_HSequenceOfTransient)::DownCast(thesharings->Value(num));
  const TColStd_ListOfInteger& alist = thesharings->Value(num);
  Handle(TColStd_HSequenceOfTransient) aSharings = new TColStd_HSequenceOfTransient;
  TColStd_ListIteratorOfListOfInteger aIt(alist);
  for( ; aIt.More() ; aIt.Next())
    aSharings->Append(Entity(aIt.Value()));
  return aSharings;
}

Interface_EntityIterator Interface_Graph::Sharings
(const Handle(Standard_Transient)& ent) const
{
  Interface_EntityIterator iter;
  iter.AddList(GetSharings(ent));
  return iter;

}

static void AddTypedSharings
(const Handle(Standard_Transient)& ent, const Handle(Standard_Type)& type,
 Interface_EntityIterator& iter, const Standard_Integer n,
 const Interface_Graph& G)
{
  if (ent.IsNull()) return;
  if (ent->IsKind(type))  {  iter.AddItem (ent);  return;  }
  if (iter.NbEntities() > n) return;

  Handle(TColStd_HSequenceOfTransient) list = G.GetSharings(ent);
  if(list.IsNull())
    return;

  Standard_Integer nb = list->Length();
  for (Standard_Integer i = 1; i <= nb; i ++)
    AddTypedSharings (list->Value(i) ,type,iter,nb,G);
}

Interface_EntityIterator Interface_Graph::TypedSharings
(const Handle(Standard_Transient)& ent, const Handle(Standard_Type)& type) const
{
  Interface_EntityIterator iter;
  Standard_Integer n = Size();
  AddTypedSharings (ent,type,iter,n,*this);
  return iter;
}


Interface_EntityIterator Interface_Graph::RootEntities () const
{
  Interface_EntityIterator iter;
  Standard_Integer nb = thesharings->Length();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if(!thesharings->Value(i).IsEmpty())
      continue;
    iter.AddItem(Entity(i));
  }
  return iter;
}

Handle(TCollection_HAsciiString)  Interface_Graph::Name(const Handle(Standard_Transient)& ent) const
{
  Handle(TCollection_HAsciiString) str;
  if (themodel.IsNull()) return str;
  if (themodel->Number(ent)) return str;

  Handle(Interface_GTool) gtool = themodel->GTool();
  if (gtool.IsNull()) return str;

  Handle(Interface_GeneralModule) module;
  Standard_Integer CN;
  if (!gtool->Select(ent,module,CN)) return str;

  Interface_ShareTool sht (*this);
  return module->Name (CN,ent,sht);
}

Standard_Boolean Interface_Graph::ModeStat() const
{
  return (!thestats.IsNull());
}
