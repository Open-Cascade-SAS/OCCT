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

#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_IntVal.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_HAsciiString.hxx>

static const occ::handle<Interface_Check>& nulcheck()
{
  static occ::handle<Interface_Check> nulch = new Interface_Check;
  return nulch;
}

//=================================================================================================

Interface_CheckIterator::Interface_CheckIterator()
{
  Clear();
}

//=================================================================================================

Interface_CheckIterator::Interface_CheckIterator(const char* name)
    : thename(name)
{
  Clear();
}

//=================================================================================================

void Interface_CheckIterator::SetName(const char* name)
{
  thename.Clear();
  if (name[0] != '\0')
    thename.AssignCat(name);
}

//=================================================================================================

const char* Interface_CheckIterator::Name() const
{
  return thename.ToCString();
}

//=================================================================================================

void Interface_CheckIterator::SetModel(const occ::handle<Interface_InterfaceModel>& model)
{
  themod = model;
}

//=================================================================================================

occ::handle<Interface_InterfaceModel> Interface_CheckIterator::Model() const
{
  return themod;
}

//=================================================================================================

void Interface_CheckIterator::Clear()
{
  thelist           = new NCollection_HSequence<occ::handle<Interface_Check>>();
  thenums           = new NCollection_HSequence<int>();
  thecurr           = new Interface_IntVal;
  thecurr->CValue() = 1;
}

//=================================================================================================

void Interface_CheckIterator::Merge(Interface_CheckIterator& other)
{
  themod = other.Model();
  for (other.Start(); other.More(); other.Next())
    Add(other.Value(), other.Number());
}

//=================================================================================================

void Interface_CheckIterator::Add(const occ::handle<Interface_Check>& ach, const int num)
{
  //  Add <same num as the last> -> accumulate Checks
  if (ach->NbWarnings() + ach->NbFails() == 0)
    return;
  int nm = num;
  if (num <= 0 && ach->HasEntity())
  {
    if (!themod.IsNull())
    {
      nm = themod->Number(ach->Entity());
      if (nm <= 0)
        nm = -1;
    }
    else
      nm = -1;
  }
  if (nm >= 0 && nm <= -(thecurr->Value()))
  {
    int i, numpos = 0, nb = thelist->Length();
    for (i = nb; i > 0; i--)
      if (thenums->Value(i) == nm)
      {
        numpos = i;
        break;
      }
    if (numpos > 0 && nm >= 0)
    {
      occ::handle<Interface_Check> lch = thelist->ChangeValue(numpos);
      lch->GetMessages(ach);
    }
    //  Normal case: add at end of list
    else
    {
      thelist->Append(ach);
      thenums->Append(nm);
    }
  }
  //  Not yet seen passed: no need to search
  else
  {
    thelist->Append(ach);
    thenums->Append(nm);
    thecurr->CValue() = -nm;
  }
}

//=================================================================================================

const occ::handle<Interface_Check>& Interface_CheckIterator::Check(const int num) const
{
  int i, nb = thelist->Length();
  for (i = 1; i <= nb; i++)
  {
    if (num == thenums->Value(i))
      return thelist->Value(i);
  }
  return nulcheck();
}

//=================================================================================================

const occ::handle<Interface_Check>& Interface_CheckIterator::Check(
  const occ::handle<Standard_Transient>& ent) const
{
  int num = -1;
  if (!themod.IsNull())
    num = themod->Number(ent);
  if (num > 0)
    return Check(num);

  int i, nb = thelist->Length();
  for (i = 1; i <= nb; i++)
  {
    if (ent == thelist->Value(i)->Entity())
      return thelist->Value(i);
  }
  return nulcheck();
}

//=================================================================================================

occ::handle<Interface_Check>& Interface_CheckIterator::CCheck(const int num)
{
  int i, nb = thenums->Length();
  for (i = 1; i <= nb; i++)
  {
    if (num == thenums->Value(i))
      return thelist->ChangeValue(i);
  }
  occ::handle<Interface_Check> ach = new Interface_Check;
  thelist->Append(ach);
  thenums->Append(num);
  return thelist->ChangeValue(thelist->Length());
}

//=================================================================================================

occ::handle<Interface_Check>& Interface_CheckIterator::CCheck(const occ::handle<Standard_Transient>& ent)
{
  int num = -1;
  if (!themod.IsNull())
    num = themod->Number(ent);
  if (num > 0)
    return CCheck(num);

  int i, nb = thelist->Length();
  for (i = 1; i <= nb; i++)
  {
    if (ent == thelist->Value(i)->Entity())
      return thelist->ChangeValue(i);
  }

  occ::handle<Interface_Check> ach = new Interface_Check;
  thelist->Append(ach);
  thenums->Append(num);
  return thelist->ChangeValue(thelist->Length());
}

//=================================================================================================

bool Interface_CheckIterator::IsEmpty(const bool failsonly) const
{
  if (thelist->IsEmpty())
    return true;
  if (!failsonly)
    return false;
  int i, nb = thelist->Length();
  for (i = 1; i <= nb; i++)
  {
    if (thelist->Value(i)->HasFailed())
      return false;
  }
  return true;
}

//=================================================================================================

Interface_CheckStatus Interface_CheckIterator::Status() const
{
  Interface_CheckStatus stat = Interface_CheckOK;
  int      i, nb = thelist->Length();
  for (i = 1; i <= nb; i++)
  {
    const occ::handle<Interface_Check> ach = thelist->Value(i);
    if (ach->HasFailed())
      return Interface_CheckFail;
    if (ach->NbWarnings() > 0)
      stat = Interface_CheckWarning;
  }
  return stat;
}

//=================================================================================================

bool Interface_CheckIterator::Complies(const Interface_CheckStatus stat) const
{
  bool res = (stat == Interface_CheckNoFail);
  int nb  = thelist->Length();
  for (int i = 1; i <= nb; ++i)
  {
    const occ::handle<Interface_Check> ach = thelist->Value(i);
    int              nbf = ach->NbFails(), nbw = ach->NbWarnings();
    switch (stat)
    {
      case Interface_CheckOK: {
        if (nbf + nbw > 0)
        {
          return false;
        }
        break;
      }
      case Interface_CheckWarning: {
        if (nbf > 0)
        {
          return false;
        }
        if (nbw > 0)
        {
          res = true;
        }
        break;
      }
      case Interface_CheckFail: {
        if (nbf > 0)
        {
          return true;
        }
        break;
      }
      case Interface_CheckAny: {
        return true;
      }
      case Interface_CheckMessage: {
        if (nbf + nbw > 0)
        {
          return true;
        }
        break;
      }
      case Interface_CheckNoFail: {
        if (nbf > 0)
        {
          return false;
        }
        break;
      }
      default:
        break;
    }
  }
  return res;
}

//=================================================================================================

Interface_CheckIterator Interface_CheckIterator::Extract(const Interface_CheckStatus stat) const
{
  Interface_CheckIterator res;
  res.SetModel(themod);
  res.SetName(thename.ToCString());
  int i, nb = thelist->Length();
  for (i = 1; i <= nb; i++)
  {
    const occ::handle<Interface_Check> ach = thelist->Value(i);
    int              nbf = ach->NbFails(), nbw = ach->NbWarnings();
    bool              prend = false;
    switch (stat)
    {
      case Interface_CheckOK:
        prend = (nbf + nbw == 0);
        break;
      case Interface_CheckWarning:
        prend = (nbf == 0 && nbw > 0);
        break;
      case Interface_CheckFail:
        prend = (nbf > 0);
        break;
      case Interface_CheckAny:
        prend = true;
        break;
      case Interface_CheckMessage:
        prend = (nbf + nbw > 0);
        break;
      case Interface_CheckNoFail:
        prend = (nbf == 0);
        break;
      default:
        break;
    }
    if (prend)
      res.Add(ach, thenums->Value(i));
  }
  return res;
}

//=================================================================================================

Interface_CheckIterator Interface_CheckIterator::Extract(const char*      mess,
                                                         const int      incl,
                                                         const Interface_CheckStatus stat) const
{
  occ::handle<TCollection_HAsciiString> str = new TCollection_HAsciiString(mess);
  Interface_CheckIterator          res;
  res.SetModel(themod);
  res.SetName(thename.ToCString());
  int i, nb = thelist->Length();
  for (i = 1; i <= nb; i++)
  {
    const occ::handle<Interface_Check> ach = thelist->Value(i);
    if (ach->Complies(str, incl, stat))
      res.Add(ach, thenums->Value(i));
  }
  return res;
}

//=================================================================================================

bool Interface_CheckIterator::Remove(const char*      mess,
                                                 const int      incl,
                                                 const Interface_CheckStatus stat)
{
  occ::handle<TCollection_HAsciiString> str = new TCollection_HAsciiString(mess);
  bool                 res = false;
  int                 i, nb = thelist->Length();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<Interface_Check> ach = thelist->ChangeValue(i);
    if (ach->Remove(str, incl, stat))
      res = true;
  }
  return res;
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Interface_CheckIterator::Checkeds(
  const bool failsonly,
  const bool global) const
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list;
  if (themod.IsNull())
    return list;
  list = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  int num, i, nb = thelist->Length();
  for (i = 1; i <= nb; i++)
  {
    const occ::handle<Interface_Check> chk = thelist->Value(i);
    if (failsonly && !chk->HasFailed())
      continue;
    if (chk->NbWarnings() == 0)
      continue;
    num = thenums->Value(i);
    if (num == 0 && global)
      list->Append(themod);
    else if (num > 0)
      list->Append(themod->Value(num));
  }
  return list;
}

//=================================================================================================

void Interface_CheckIterator::Start() const
{
  thecurr->CValue() = 1;
}

//=================================================================================================

bool Interface_CheckIterator::More() const
{
  if (thecurr->Value() < 0)
    thecurr->CValue() = 1;
  return (thecurr->Value() <= thelist->Length());
}

//=================================================================================================

void Interface_CheckIterator::Next() const
{
  if (thecurr->Value() < 0)
    thecurr->CValue() = 1;
  thecurr->CValue()++;
}

//=================================================================================================

const occ::handle<Interface_Check>& Interface_CheckIterator::Value() const
{
  if (thecurr->Value() > thelist->Length())
    throw Standard_NoSuchObject("Interface Check Iterator : Value");
  return thelist->Value(thecurr->Value());
}

//=================================================================================================

int Interface_CheckIterator::Number() const
{
  if (thecurr->Value() > thenums->Length())
    throw Standard_NoSuchObject("Interface Check Iterator : Value");
  return thenums->Value(thecurr->Value());
}

//=================================================================================================

void Interface_CheckIterator::Print(Standard_OStream&      S,
                                    const bool failsonly,
                                    const int final) const
{
  Print(S, themod, failsonly, final);
}

//=================================================================================================

void Interface_CheckIterator::Print(Standard_OStream&                       S,
                                    const occ::handle<Interface_InterfaceModel>& model,
                                    const bool                  failsonly,
                                    const int /*final*/) const
{
  bool titre = false;
  /*const char* mesnum;
    const char* mesnum0 = ":";
    const char* mesnum1 = " (original):";
    const char* mesnum2 = " (computed):";    */
  int i, nb = thelist->Length(); //,j; svv #2
  bool yamod = !model.IsNull();
  for (i = 1; i <= nb; i++)
  {
    const occ::handle<Interface_Check> ach = thelist->Value(i);
    int              nbw = 0, nbf = ach->NbFails();
    if (!failsonly)
      nbw = ach->NbWarnings();
    if (nbf + nbw == 0)
      continue;
    occ::handle<Standard_Transient> ent    = ach->Entity();
    int           nm0    = thenums->Value(i);
    bool           entnul = ent.IsNull();
    int           num    = nm0;
    if (nm0 <= 0 && !entnul && yamod)
      num = model->Number(ent);
    if (nm0 <= 0 && entnul)
      num = -1; // Global
                //  mesnum = mesnum0;
                //    if (yamod) mesnum = (nm0 > 0 ? mesnum1 : mesnum2);

    if (!titre)
      S << " **  " << Name() << "  **" << std::endl;
    titre = true;
    S << "Check:";
    if (nb > 9 && i < 10)
      S << " ";
    if (nb > 99 && i < 100)
      S << " ";
    S << i;
    if (num < 0)
      S << " -- Global Check" << std::endl;
    else if (num == 0)
      S << " -- Entity n0 ??:";
    else
    {
      if (yamod)
      {
        S << " -- Entity (n0:id) ";
        model->Print(ent, S);
      }
      else
        S << " -- Entity n0 " << num;
      //      S<<" -- Entity n0 "<<num<<mesnum;
      //      if (yamod) model->PrintLabel(ent,S);
    }
    if (num >= 0 && entnul)
      S << " (unknown Type)" << std::endl;
    else if (num >= 0 && !entnul)
    {
      if (yamod)
        S << "   Type:" << model->TypeName(ent) << std::endl;
      else
        S << "   Type:" << ent->DynamicType()->Name() << std::endl;
    }

    ach->Print(S, (failsonly ? 1 : 3));
  }
  //  if (nbch > 0)  S<<" ----  Checks : "<<nbch<<"  ----"<<std::endl;
}

//=================================================================================================

void Interface_CheckIterator::Destroy()
{
  thecurr.Nullify();
} // redevient standard
