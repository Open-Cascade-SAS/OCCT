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
#include <IFSelect_Modifier.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_TransformStandard.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CopyControl.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_TransformStandard, IFSelect_Transformer)

IFSelect_TransformStandard::IFSelect_TransformStandard()
{
  thecopy = true;
}

void IFSelect_TransformStandard::SetCopyOption(const bool option)
{
  thecopy = option;
}

bool IFSelect_TransformStandard::CopyOption() const
{
  return thecopy;
}

void IFSelect_TransformStandard::SetSelection(const occ::handle<IFSelect_Selection>& sel)
{
  thesel = sel;
}

occ::handle<IFSelect_Selection> IFSelect_TransformStandard::Selection() const
{
  return thesel;
}

int IFSelect_TransformStandard::NbModifiers() const
{
  return themodifs.Length();
}

occ::handle<IFSelect_Modifier> IFSelect_TransformStandard::Modifier(const int num) const
{
  return GetCasted(IFSelect_Modifier, themodifs.Value(num));
}

int IFSelect_TransformStandard::ModifierRank(const occ::handle<IFSelect_Modifier>& modif) const
{
  for (int i = themodifs.Length(); i >= 1; i--)
    if (modif == themodifs.Value(i))
      return i;
  return 0;
}

bool IFSelect_TransformStandard::AddModifier(const occ::handle<IFSelect_Modifier>& modif,
                                             const int                             atnum)
{
  if (atnum < 0 || atnum > themodifs.Length())
    return false;
  if (atnum == 0)
    themodifs.Append(modif);
  else
    themodifs.InsertBefore(atnum, modif);
  return true;
}

bool IFSelect_TransformStandard::RemoveModifier(const occ::handle<IFSelect_Modifier>& modif)
{
  int num = ModifierRank(modif);
  return RemoveModifier(num);
}

bool IFSelect_TransformStandard::RemoveModifier(const int num)
{
  if (num <= 0 || num > themodifs.Length())
    return false;
  themodifs.Remove(num);
  return true;
}

//  #################################################################
//  ########                     ACTION                      ########

bool IFSelect_TransformStandard::Perform(const Interface_Graph&                 G,
                                         const occ::handle<Interface_Protocol>& protocol,
                                         Interface_CheckIterator&               checks,
                                         occ::handle<Interface_InterfaceModel>& newmod)
{
  Interface_CopyTool TC(G.Model(), protocol);
  themap = TC.Control();
  Copy(G, TC, newmod);
  return ApplyModifiers(G, protocol, TC, checks, newmod);
}

void IFSelect_TransformStandard::Copy(const Interface_Graph&                 G,
                                      Interface_CopyTool&                    TC,
                                      occ::handle<Interface_InterfaceModel>& newmod) const
{
  if (CopyOption())
    StandardCopy(G, TC, newmod);
  else
    OnTheSpot(G, TC, newmod);
}

void IFSelect_TransformStandard::StandardCopy(const Interface_Graph&                 G,
                                              Interface_CopyTool&                    TC,
                                              occ::handle<Interface_InterfaceModel>& newmod) const
{
  const occ::handle<Interface_InterfaceModel>& original = G.Model();
  newmod                                                = original->NewEmptyModel();
  TC.Clear();
  int                                   nb     = G.Size();
  occ::handle<NCollection_HArray1<int>> remain = new NCollection_HArray1<int>(0, nb + 1);
  remain->Init(0);
  for (int i = 1; i <= nb; i++)
  {
    //    if (G.Status(i) == 0) TC.TransferEntity (original->Value(i));
    TC.TransferEntity(original->Value(i));
  }
  TC.FillModel(newmod);
}

void IFSelect_TransformStandard::OnTheSpot(const Interface_Graph&                 G,
                                           Interface_CopyTool&                    TC,
                                           occ::handle<Interface_InterfaceModel>& newmod) const
{
  int nb = G.Size();
  for (int i = 1; i <= nb; i++)
    TC.Bind(G.Entity(i), G.Entity(i));
  newmod = G.Model();
}

bool IFSelect_TransformStandard::ApplyModifiers(const Interface_Graph&                 G,
                                                const occ::handle<Interface_Protocol>& protocol,
                                                Interface_CopyTool&                    TC,
                                                Interface_CheckIterator&               checks,
                                                occ::handle<Interface_InterfaceModel>& newmod) const
{
  Message_Messenger::StreamBuffer              sout     = Message::SendInfo();
  bool                                         res      = true;
  bool                                         chg      = false;
  int                                          nb       = NbModifiers();
  const occ::handle<Interface_InterfaceModel>& original = G.Model();

  for (int i = 1; i <= nb; i++)
  {
    occ::handle<IFSelect_Modifier> unmod = Modifier(i);
    if (unmod->MayChangeGraph())
      chg = true;

    //    Apply this Modifier (nb : the Dispatch, we don't care about it)
    //    First, the Selection
    IFSelect_ContextModif ctx(G, TC);
    //    Then, the Selection
    //    If there is one here, it has priority. Otherwise, each Modifier has its own

    occ::handle<IFSelect_Selection> sel = thesel;
    if (sel.IsNull())
      sel = unmod->Selection();
    if (!sel.IsNull())
    {
      Interface_EntityIterator entiter = sel->UniqueResult(G);
      ctx.Select(entiter);
    }
    if (ctx.IsForNone())
      continue;
    unmod->Perform(ctx, newmod, protocol, TC);

    //    Error Reporting
    //    Should we record them in newmod ? good question
    Interface_CheckIterator checklist = ctx.CheckList();
    if (!checklist.IsEmpty(false))
    {
      checks.Merge(checklist);
      sout << "IFSelect_TransformStandard :  Messages from Modifier n0 " << i << " of " << nb
           << std::endl;
      checklist.Print(sout, newmod, false);
    }
    if (!checklist.IsEmpty(true))
    {
      sout << " --  Abandon TransformStandard  --" << std::endl;
      res = false;
      break;
    }
  }

  //   Model not modified and Graph not modified: say it
  if (newmod == original && !chg)
    newmod.Nullify();
  return res;
}

bool IFSelect_TransformStandard::Updated(const occ::handle<Standard_Transient>& entfrom,
                                         occ::handle<Standard_Transient>&       entto) const
{
  if (themap.IsNull())
    return false;
  return themap->Search(entfrom, entto);
}

TCollection_AsciiString IFSelect_TransformStandard::Label() const
{
  char                    lab[30];
  TCollection_AsciiString labl("");
  if (CopyOption())
    labl.AssignCat("Standard Copy");
  else
    labl.AssignCat("On the spot Edition");
  int nb = NbModifiers();
  if (nb == 0)
    Sprintf(lab, " (no Modifier)");
  if (nb == 1)
    Sprintf(lab, " - %s", Modifier(1)->Label().ToCString());
  if (nb > 1)
    Sprintf(lab, " - %d Modifiers", nb);
  labl.AssignCat(lab);
  return labl;
}
