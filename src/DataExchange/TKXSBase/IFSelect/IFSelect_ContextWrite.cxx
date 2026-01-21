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
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>

//=================================================================================================

IFSelect_ContextWrite::IFSelect_ContextWrite(const occ::handle<Interface_InterfaceModel>&  model,
                                             const occ::handle<Interface_Protocol>&        proto,
                                             const occ::handle<IFSelect_AppliedModifiers>& applieds,
                                             const char* const                             filename)
    : themodel(model),
      theproto(proto),
      thefile(filename),
      theapply(applieds),
      thenumod(0),
      thenbent(0),
      thecurr(0)
{
}

//=================================================================================================

IFSelect_ContextWrite::IFSelect_ContextWrite(const occ::handle<Interface_HGraph>&          hgraph,
                                             const occ::handle<Interface_Protocol>&        proto,
                                             const occ::handle<IFSelect_AppliedModifiers>& applieds,
                                             const char* const                             filename)
    : themodel(hgraph->Graph().Model()),
      theproto(proto),
      thefile(filename),
      theapply(applieds),
      thehgraf(hgraph),
      thenumod(0),
      thenbent(0),
      thecurr(0)
{
}

//=================================================================================================

occ::handle<Interface_InterfaceModel> IFSelect_ContextWrite::Model() const
{
  return themodel;
}

//=================================================================================================

occ::handle<Interface_Protocol> IFSelect_ContextWrite::Protocol() const
{
  return theproto;
}

//=================================================================================================

const char* IFSelect_ContextWrite::FileName() const
{
  return thefile.ToCString();
}

//=================================================================================================

occ::handle<IFSelect_AppliedModifiers> IFSelect_ContextWrite::AppliedModifiers() const
{
  return theapply;
}

//=================================================================================================

const Interface_Graph& IFSelect_ContextWrite::Graph()
{
  if (thehgraf.IsNull())
    thehgraf = new Interface_HGraph(themodel, theproto);
  return thehgraf->Graph();
}

//=================================================================================================

int IFSelect_ContextWrite::NbModifiers() const
{
  return (theapply.IsNull() ? 0 : theapply->Count());
}

bool IFSelect_ContextWrite::SetModifier(const int numod)
{
  themodif.Nullify();
  thenumod = thenbent = thecurr = 0;
  if (theapply.IsNull())
    return false;
  if (numod < 1 || numod > theapply->Count())
    return false;
  theapply->Item(numod, themodif, thenbent);
  return true;
}

//=================================================================================================

occ::handle<IFSelect_GeneralModifier> IFSelect_ContextWrite::FileModifier() const
{
  return themodif;
}

//=================================================================================================

bool IFSelect_ContextWrite::IsForNone() const
{
  return (thenbent == 0);
}

//=================================================================================================

bool IFSelect_ContextWrite::IsForAll() const
{
  return theapply->IsForAll();
}

//=================================================================================================

int IFSelect_ContextWrite::NbEntities() const
{
  return thenbent;
}

//=================================================================================================

void IFSelect_ContextWrite::Start()
{
  thecurr = 1;
}

//=================================================================================================

bool IFSelect_ContextWrite::More() const
{
  return (thecurr <= thenbent);
}

//=================================================================================================

void IFSelect_ContextWrite::Next()
{
  thecurr++;
}

//=================================================================================================

occ::handle<Standard_Transient> IFSelect_ContextWrite::Value() const
{
  if (thecurr < 1 || thecurr > thenbent)
    throw Standard_NoSuchObject("IFSelect_ContextWrite:Value");
  int num = theapply->ItemNum(thecurr);
  return themodel->Value(num);
}

//=================================================================================================

void IFSelect_ContextWrite::AddCheck(const occ::handle<Interface_Check>& check)
{
  if (check->NbFails() + check->NbWarnings() == 0)
    return;
  const occ::handle<Standard_Transient>& ent = check->Entity();
  int                                    num = themodel->Number(ent);
  if (num == 0 && !ent.IsNull())
    num = -1; // force enregistrement
  thecheck.Add(check, num);
}

//=================================================================================================

void IFSelect_ContextWrite::AddWarning(const occ::handle<Standard_Transient>& start,
                                       const char* const                      mess,
                                       const char* const                      orig)
{
  thecheck.CCheck(themodel->Number(start))->AddWarning(mess, orig);
}

//=================================================================================================

void IFSelect_ContextWrite::AddFail(const occ::handle<Standard_Transient>& start,
                                    const char* const                      mess,
                                    const char* const                      orig)
{
  thecheck.CCheck(themodel->Number(start))->AddFail(mess, orig);
}

//=================================================================================================

occ::handle<Interface_Check> IFSelect_ContextWrite::CCheck(const int num)
{
  occ::handle<Interface_Check> ach = thecheck.CCheck(num);
  if (num > 0 && num <= themodel->NbEntities())
    ach->SetEntity(themodel->Value(num));
  return ach;
}

//=================================================================================================

occ::handle<Interface_Check> IFSelect_ContextWrite::CCheck(
  const occ::handle<Standard_Transient>& ent)
{
  int num = themodel->Number(ent);
  if (num == 0)
    num = -1; // force l enregistrement
  occ::handle<Interface_Check> ach = thecheck.CCheck(num);
  ach->SetEntity(ent);
  return ach;
}

//=================================================================================================

Interface_CheckIterator IFSelect_ContextWrite::CheckList() const
{
  return thecheck;
}
