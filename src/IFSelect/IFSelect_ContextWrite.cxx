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


#include <IFSelect_AppliedModifiers.hxx>
#include <IFSelect_ContextWrite.hxx>
#include <IFSelect_GeneralModifier.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>

//=======================================================================
//function : IFSelect_ContextWrite
//purpose  : 
//=======================================================================
IFSelect_ContextWrite::IFSelect_ContextWrite
  (const Handle(Interface_InterfaceModel)& model,
   const Handle(Interface_Protocol)& proto,
   const Handle(IFSelect_AppliedModifiers)& applieds,
   const Standard_CString filename)
    : themodel (model) , theproto (proto) , thefile (filename) ,
      theapply (applieds) , thenumod(0) , thenbent (0) , thecurr (0)      {  }


//=======================================================================
//function : IFSelect_ContextWrite
//purpose  : 
//=======================================================================

IFSelect_ContextWrite::IFSelect_ContextWrite
  (const Handle(Interface_HGraph)& hgraph,
   const Handle(Interface_Protocol)& proto,
   const Handle(IFSelect_AppliedModifiers)& applieds,
   const Standard_CString filename)
    : themodel (hgraph->Graph().Model()) ,
      theproto (proto)   , thefile (filename) , theapply (applieds) , 
      thehgraf (hgraph) , thenumod(0) , thenbent (0) , thecurr (0)      {  }


//=======================================================================
//function : Model
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel)  IFSelect_ContextWrite::Model () const
{
  return themodel;
}


//=======================================================================
//function : Protocol
//purpose  : 
//=======================================================================

Handle(Interface_Protocol)  IFSelect_ContextWrite::Protocol () const
{
  return theproto;
}


//=======================================================================
//function : FileName
//purpose  : 
//=======================================================================

Standard_CString  IFSelect_ContextWrite::FileName () const
{
  return thefile.ToCString();
}


//=======================================================================
//function : AppliedModifiers
//purpose  : 
//=======================================================================

Handle(IFSelect_AppliedModifiers)  IFSelect_ContextWrite::AppliedModifiers () const
{
  return theapply;
}


//=======================================================================
//function : Graph
//purpose  : 
//=======================================================================

const Interface_Graph&  IFSelect_ContextWrite::Graph ()
{
  if (thehgraf.IsNull()) thehgraf = new Interface_HGraph(themodel,theproto);
  return thehgraf->Graph();
}


//=======================================================================
//function : NbModifiers
//purpose  : 
//=======================================================================

Standard_Integer  IFSelect_ContextWrite::NbModifiers () const
      {  return (theapply.IsNull() ? 0 : theapply->Count());  }

    Standard_Boolean  IFSelect_ContextWrite::SetModifier
  (const Standard_Integer numod)
{
  themodif.Nullify();  thenumod = thenbent = thecurr = 0;
  if (theapply.IsNull()) return Standard_False;
  if (numod < 1 || numod > theapply->Count()) return Standard_False;
  theapply->Item(numod,themodif,thenbent);
  return Standard_True;
}


//=======================================================================
//function : FileModifier
//purpose  : 
//=======================================================================

Handle(IFSelect_GeneralModifier)  IFSelect_ContextWrite::FileModifier () const
{
  return themodif;
}


//=======================================================================
//function : IsForNone
//purpose  : 
//=======================================================================

Standard_Boolean  IFSelect_ContextWrite::IsForNone () const
{
  return (thenbent == 0);
}


//=======================================================================
//function : IsForAll
//purpose  : 
//=======================================================================

Standard_Boolean  IFSelect_ContextWrite::IsForAll () const
{
  return theapply->IsForAll();
}


//=======================================================================
//function : NbEntities
//purpose  : 
//=======================================================================

Standard_Integer  IFSelect_ContextWrite::NbEntities () const
{
  return thenbent;
}


//=======================================================================
//function : Start
//purpose  : 
//=======================================================================

void  IFSelect_ContextWrite::Start ()
{
  thecurr = 1;
}


//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean  IFSelect_ContextWrite::More () const
{
  return (thecurr <= thenbent);
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void IFSelect_ContextWrite::Next ()
{
  thecurr ++;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_ContextWrite::Value() const
{
  if (thecurr < 1 || thecurr > thenbent)
    throw Standard_NoSuchObject("IFSelect_ContextWrite:Value");
  Standard_Integer num = theapply->ItemNum (thecurr);
  return themodel->Value(num);
}


//=======================================================================
//function : AddCheck
//purpose  : 
//=======================================================================

void IFSelect_ContextWrite::AddCheck (const Handle(Interface_Check)& check)
{
  if (check->NbFails() + check->NbWarnings() == 0) return;
  const Handle(Standard_Transient)& ent = check->Entity();
  Standard_Integer num = themodel->Number(ent);
  if (num == 0 && !ent.IsNull()) num = -1;  // force enregistrement
  thecheck.Add(check,num);
}


//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void IFSelect_ContextWrite::AddWarning(const Handle(Standard_Transient)& start,
                                       const Standard_CString mess,
                                       const Standard_CString orig)
{
  thecheck.CCheck(themodel->Number(start))->AddWarning(mess,orig);
}


//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void IFSelect_ContextWrite::AddFail(const Handle(Standard_Transient)& start,
                                    const Standard_CString mess,
                                    const Standard_CString orig)
{
  thecheck.CCheck(themodel->Number(start))->AddFail(mess,orig);
}


//=======================================================================
//function : CCheck
//purpose  : 
//=======================================================================

Handle(Interface_Check) IFSelect_ContextWrite::CCheck (const Standard_Integer num)
{
  Handle(Interface_Check) ach = thecheck.CCheck(num);
  if (num > 0 && num <= themodel->NbEntities()) ach->SetEntity(themodel->Value(num));
  return ach;
}


//=======================================================================
//function : CCheck
//purpose  : 
//=======================================================================

Handle(Interface_Check) IFSelect_ContextWrite::CCheck(const Handle(Standard_Transient)& ent)
{
  Standard_Integer num = themodel->Number(ent);
  if (num == 0) num = -1;    // force l enregistrement
  Handle(Interface_Check) ach = thecheck.CCheck(num);
  ach->SetEntity(ent);
  return ach;
}


//=======================================================================
//function : CheckList
//purpose  : 
//=======================================================================

Interface_CheckIterator  IFSelect_ContextWrite::CheckList () const
{
  return thecheck;
}
