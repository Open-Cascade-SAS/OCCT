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

#include <IFSelect_ContextModif.hxx>
#include <IFSelect_GeneralModifier.hxx>
#include <IFSelect_Selection.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CopyControl.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>

//=================================================================================================

IFSelect_ContextModif::IFSelect_ContextModif(const Interface_Graph&    graph,
                                             const Interface_CopyTool& TC,
                                             const Standard_CString    filename)
    : thegraf(graph, Standard_False),
      thefile(filename),
      thelist(graph.Size(), ' ')
{
  themap  = TC.Control();
  thesel  = Standard_False;
  thecurr = thecurt             = 0;
  Standard_Integer           nb = thelist.Length();
  Handle(Standard_Transient) newent;
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    if (themap->Search(graph.Entity(i), newent))
      thelist.SetValue(i, '1');
  }
}

//=================================================================================================

IFSelect_ContextModif::IFSelect_ContextModif(const Interface_Graph& graph,
                                             const Standard_CString filename)
    : thegraf(graph, Standard_False),
      thefile(filename),
      thelist(graph.Size(), ' ')
{
  thesel  = Standard_False;
  thecurr = thecurt             = 0;
  Standard_Integer           nb = thelist.Length();
  Handle(Standard_Transient) newent;
  for (Standard_Integer i = 1; i <= nb; i++)
    thelist.SetValue(i, '1');
}

//=================================================================================================

void IFSelect_ContextModif::Select(Interface_EntityIterator& list)
{
  thesel              = Standard_True;
  Standard_Integer nb = thelist.Length();
  for (Standard_Integer i = 1; i <= nb; i++)
    thelist.SetValue(i, ' ');
  for (list.Start(); list.More(); list.Next())
  {
    Handle(Standard_Transient) start, newent;
    start                = list.Value();
    Standard_Integer num = thegraf.EntityNumber(start);
    if (num > nb || num < 0)
      num = 0;
    if (themap.IsNull() && num > 0)
      thelist.SetValue(num, '1');
    else if (themap->Search(start, newent))
    {
      if (num > 0)
        thelist.SetValue(num, '1');
    }
  }
}

//=================================================================================================

const Interface_Graph& IFSelect_ContextModif::OriginalGraph() const
{
  return thegraf;
}

//=================================================================================================

Handle(Interface_InterfaceModel) IFSelect_ContextModif::OriginalModel() const
{
  return thegraf.Model();
}

//=================================================================================================

void IFSelect_ContextModif::SetProtocol(const Handle(Interface_Protocol)& prot)
{
  theprot = prot;
}

//=================================================================================================

Handle(Interface_Protocol) IFSelect_ContextModif::Protocol() const
{
  return theprot;
}

//=================================================================================================

Standard_Boolean IFSelect_ContextModif::HasFileName() const
{
  return (thefile.Length() > 0);
}

//=================================================================================================

Standard_CString IFSelect_ContextModif::FileName() const
{
  return thefile.ToCString();
}

//=================================================================================================

Handle(Interface_CopyControl) IFSelect_ContextModif::Control() const
{
  return themap;
}

//=================================================================================================

Standard_Boolean IFSelect_ContextModif::IsForNone() const
{
  if (!thesel)
    return Standard_False;
  Standard_Integer nb = thelist.Length();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    if (thelist.Value(i) != ' ')
      return Standard_False;
  }
  return Standard_True;
}

//=================================================================================================

Standard_Boolean IFSelect_ContextModif::IsForAll() const
{
  return (!thesel);
}

//=================================================================================================

Standard_Boolean IFSelect_ContextModif::IsTransferred(const Handle(Standard_Transient)& ent) const
{
  if (themap.IsNull())
    return Standard_True;
  Handle(Standard_Transient) newent;
  return themap->Search(ent, newent);
}

//=================================================================================================

Standard_Boolean IFSelect_ContextModif::IsSelected(const Handle(Standard_Transient)& ent) const
{
  //  Select a deja verifie "IsTransferred"
  Standard_Integer num = thegraf.EntityNumber(ent);
  if (num == 0)
    return Standard_False;
  return (thelist.Value(num) != ' ');
}

//=================================================================================================

Interface_EntityIterator IFSelect_ContextModif::SelectedOriginal() const
{
  Interface_EntityIterator list;
  Standard_Integer         nb = thelist.Length();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    if (thelist.Value(i) != ' ')
      list.GetOneItem(thegraf.Entity(i));
  }
  return list;
}

//=================================================================================================

Interface_EntityIterator IFSelect_ContextModif::SelectedResult() const
{
  Interface_EntityIterator list;
  Standard_Integer         nb = thelist.Length();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    Handle(Standard_Transient) newent;
    if (themap.IsNull())
      newent = thegraf.Entity(i);
    else if (thelist.Value(i) != ' ')
      themap->Search(thegraf.Entity(i), newent);
    if (!newent.IsNull())
      list.GetOneItem(newent);
  }
  return list;
}

//=================================================================================================

Standard_Integer IFSelect_ContextModif::SelectedCount() const
{
  Standard_Integer nb = thelist.Length();
  Standard_Integer ns = 0;
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    if (thelist.Value(i) != ' ')
      ns++;
  }
  return ns;
}

//=================================================================================================

void IFSelect_ContextModif::Start()
{
  thecurr = thecurt = 0;
  Next();
}

//=================================================================================================

Standard_Boolean IFSelect_ContextModif::More() const
{
  return (thecurr > 0);
}

//=================================================================================================

void IFSelect_ContextModif::Next()
{
  Standard_Integer nb = thelist.Length();
  //  thecurr = thecurt;
  //  if (thecurr <= 0 && thecurt >= 0) return;
  for (Standard_Integer i = thecurr + 1; i <= nb; i++)
  {
    if (thelist.Value(i) != ' ')
    {
      thecurr = i;
      thecurt++;
      return;
    }
  }
  thecurr = thecurt = 0;
}

//=================================================================================================

Handle(Standard_Transient) IFSelect_ContextModif::ValueOriginal() const
{
  if (thecurr <= 0)
    throw Standard_NoSuchObject("IFSelect_ContextModif");
  return thegraf.Entity(thecurr);
}

//=================================================================================================

Handle(Standard_Transient) IFSelect_ContextModif::ValueResult() const
{
  if (thecurr <= 0)
    throw Standard_NoSuchObject("IFSelect_ContextModif");
  Handle(Standard_Transient) ent, newent;
  ent = thegraf.Entity(thecurr);
  if (themap.IsNull())
    newent = ent;
  else
    themap->Search(ent, newent);
  return newent;
}

//=================================================================================================

void IFSelect_ContextModif::TraceModifier(const Handle(IFSelect_GeneralModifier)& modif)
{
  if (modif.IsNull())
    return;

  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  sout << "---   Run Modifier:" << std::endl;
  Handle(IFSelect_Selection) sel = modif->Selection();
  if (!sel.IsNull())
    sout << "      Selection:" << sel->Label();
  else
    sout << "  (no Selection)";

  //  on va simplement compter les entites
  Standard_Integer ne = 0, nb = thelist.Length();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    if (thelist.Value(i) != ' ')
      ne++;
  }
  if (nb == ne)
    sout << "  All Model (" << nb << " Entities)" << std::endl;
  else
    sout << "  Entities,Total:" << nb << " Concerned:" << ne << std::endl;
}

//=================================================================================================

void IFSelect_ContextModif::Trace(const Standard_CString mess)
{
  //  Trace courante
  if (thecurr <= 0)
    return;
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (ValueOriginal() == ValueResult())
    sout << "--  ContextModif. Entity  n0 " << thecurr << std::endl;
  else
    sout << "--  ContextModif. Entity in Original, n0 " << thecurr << " in Result, n0 " << thecurt
         << std::endl;
  if (mess[0] != '\0')
    sout << "--  Message:" << mess << std::endl;
}

//=================================================================================================

void IFSelect_ContextModif::AddCheck(const Handle(Interface_Check)& check)
{
  if (check->NbFails() + check->NbWarnings() == 0)
    return;
  const Handle(Standard_Transient)& ent = check->Entity();
  Standard_Integer                  num = thegraf.EntityNumber(ent);
  if (num == 0 && !ent.IsNull())
    num = -1; // force enregistrement
  thechek.Add(check, num);
}

//=================================================================================================

void IFSelect_ContextModif::AddWarning(const Handle(Standard_Transient)& start,
                                       const Standard_CString            mess,
                                       const Standard_CString            orig)
{
  thechek.CCheck(thegraf.EntityNumber(start))->AddWarning(mess, orig);
}

//=================================================================================================

void IFSelect_ContextModif::AddFail(const Handle(Standard_Transient)& start,
                                    const Standard_CString            mess,
                                    const Standard_CString            orig)
{
  thechek.CCheck(thegraf.EntityNumber(start))->AddFail(mess, orig);
}

//=================================================================================================

Handle(Interface_Check) IFSelect_ContextModif::CCheck(const Standard_Integer num)
{
  Handle(Interface_Check) ach = thechek.CCheck(num);
  if (num > 0 && num <= thegraf.Size())
    ach->SetEntity(thegraf.Entity(num));
  return ach;
}

//=================================================================================================

Handle(Interface_Check) IFSelect_ContextModif::CCheck(const Handle(Standard_Transient)& ent)
{
  Standard_Integer num = thegraf.EntityNumber(ent);
  if (num == 0)
    num = -1; // force l enregistrement
  Handle(Interface_Check)& ach = thechek.CCheck(num);
  ach->SetEntity(ent);
  return ach;
}

//=================================================================================================

Interface_CheckIterator IFSelect_ContextModif::CheckList() const
{
  return thechek;
}
