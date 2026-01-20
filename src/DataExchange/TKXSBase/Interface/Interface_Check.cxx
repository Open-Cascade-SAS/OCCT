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
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_Check, Standard_Transient)

// A Check is created initially without a message list: this is created
// only if at least one error must be recorded (Fail-Warning)
//=================================================================================================

Interface_Check::Interface_Check() {} // construit a vide

//=================================================================================================

Interface_Check::Interface_Check(const occ::handle<Standard_Transient>& anentity)
{
  theent = anentity;
}

// ....      Adding a true Error message (Fail)

//=================================================================================================

void Interface_Check::SendFail(const Message_Msg& amsg)
{
  AddFail(amsg);
}

//=================================================================================================

void Interface_Check::AddFail(const occ::handle<TCollection_HAsciiString>& mess)
{
  if (thefails.IsNull())
    thefails = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  if (thefailo.IsNull())
    thefailo = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  thefails->Append(mess);
  thefailo->Append(mess);
}

//=================================================================================================

void Interface_Check::AddFail(const occ::handle<TCollection_HAsciiString>& mess,
                              const occ::handle<TCollection_HAsciiString>& orig)
{
  if (thefails.IsNull())
    thefails = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  if (thefailo.IsNull())
    thefailo = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  thefails->Append(mess);
  thefailo->Append(orig);
}

//=================================================================================================

void Interface_Check::AddFail(const char* const amess, const char* const orig)
{
  if (amess[0] == '\0')
    return;
  if (!orig || orig[0] == '\0')
    AddFail(new TCollection_HAsciiString(amess));
  else
    AddFail(new TCollection_HAsciiString(amess), new TCollection_HAsciiString(orig));
}

//=================================================================================================

void Interface_Check::AddFail(const Message_Msg& amsg)
{
  if (amsg.IsEdited())
    AddFail(new TCollection_HAsciiString(TCollection_AsciiString(amsg.Value())),
            new TCollection_HAsciiString(TCollection_AsciiString(amsg.Original())));
  else
    AddFail(new TCollection_HAsciiString(TCollection_AsciiString(amsg.Value())));
}

//=================================================================================================

bool Interface_Check::HasFailed() const
{
  return (!thefails.IsNull());
}

//=================================================================================================

int Interface_Check::NbFails() const
{
  return (thefails.IsNull() ? 0 : thefails->Length());
}

//=================================================================================================

const occ::handle<TCollection_HAsciiString>& Interface_Check::Fail(const int num,
                                                              const bool final) const
{
  if (thefails.IsNull())
    throw Standard_OutOfRange();
  return (final ? thefails->Value(num) : thefailo->Value(num));
}

//=================================================================================================

const char* Interface_Check::CFail(const int num,
                                        const bool final) const
{
  return Fail(num, final)->ToCString();
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> Interface_Check::Fails(const bool final) const
{
  if (thefails.IsNull())
    return new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  return (final ? thefails : thefailo);
}

//  ....        Adding a Warning message

//=================================================================================================

void Interface_Check::SendWarning(const Message_Msg& amsg)
{
  AddWarning(amsg);
}

//=================================================================================================

void Interface_Check::AddWarning(const occ::handle<TCollection_HAsciiString>& mess)
{
  if (thewarns.IsNull())
    thewarns = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  if (thewarno.IsNull())
    thewarno = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  thewarns->Append(mess);
  thewarno->Append(mess);
}

//=================================================================================================

void Interface_Check::AddWarning(const occ::handle<TCollection_HAsciiString>& mess,
                                 const occ::handle<TCollection_HAsciiString>& orig)
{
  if (thewarns.IsNull())
    thewarns = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  if (thewarno.IsNull())
    thewarno = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  thewarns->Append(mess);
  thewarno->Append(orig);
}

//=================================================================================================

void Interface_Check::AddWarning(const char* const amess, const char* const orig)
{
  if (amess[0] == '\0')
    return;
  if (!orig || orig[0] == '\0')
    AddWarning(new TCollection_HAsciiString(amess));
  else
    AddWarning(new TCollection_HAsciiString(amess), new TCollection_HAsciiString(orig));
}

//=================================================================================================

void Interface_Check::AddWarning(const Message_Msg& amsg)
{
  if (amsg.IsEdited())
    AddWarning(new TCollection_HAsciiString(TCollection_AsciiString(amsg.Value())),
               new TCollection_HAsciiString(TCollection_AsciiString(amsg.Original())));
  else
    AddWarning(new TCollection_HAsciiString(TCollection_AsciiString(amsg.Value())));
}

//=================================================================================================

bool Interface_Check::HasWarnings() const
{
  return (!thewarns.IsNull());
}

//=================================================================================================

int Interface_Check::NbWarnings() const
{
  return (thewarns.IsNull() ? 0 : thewarns->Length());
}

//=================================================================================================

const occ::handle<TCollection_HAsciiString>& Interface_Check::Warning(const int num,
                                                                 const bool final) const
{
  if (thewarns.IsNull())
    throw Standard_OutOfRange();
  return (final ? thewarns->Value(num) : thewarno->Value(num));
}

//=================================================================================================

const char* Interface_Check::CWarning(const int num,
                                           const bool final) const
{
  return Warning(num, final)->ToCString();
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> Interface_Check::Warnings(
  const bool final) const
{
  if (thewarns.IsNull())
    return new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  return (final ? thewarns : thewarno);
}

//  ....        Adding a simple Info message (not yet completed)

//=================================================================================================

void Interface_Check::SendMsg(const Message_Msg& amsg)
{
  occ::handle<TCollection_HAsciiString> mess = new TCollection_HAsciiString(amsg.Value());
  occ::handle<TCollection_HAsciiString> orig = mess;
  if (amsg.IsEdited())
    orig = new TCollection_HAsciiString(amsg.Original());

  if (theinfos.IsNull())
    theinfos = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  if (theinfoo.IsNull())
    theinfoo = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  theinfos->Append(mess);
  theinfoo->Append(mess);
}

//=================================================================================================

int Interface_Check::NbInfoMsgs() const
{
  return (theinfos.IsNull() ? 0 : theinfos->Length());
}

//=================================================================================================

const occ::handle<TCollection_HAsciiString>& Interface_Check::InfoMsg(const int num,
                                                                 const bool final) const
{
  if (theinfos.IsNull())
    throw Standard_OutOfRange();
  return (final ? theinfos->Value(num) : theinfoo->Value(num));
}

//=================================================================================================

const char* Interface_Check::CInfoMsg(const int num,
                                           const bool final) const
{
  return InfoMsg(num, final)->ToCString();
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> Interface_Check::InfoMsgs(
  const bool final) const
{
  if (theinfos.IsNull())
    return new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  return (final ? theinfos : theinfoo);
}

//    ....  General management

//=================================================================================================

Interface_CheckStatus Interface_Check::Status() const
{
  if (!thefails.IsNull())
    return Interface_CheckFail;
  if (!thewarns.IsNull())
    return Interface_CheckWarning;
  return Interface_CheckOK;
}

//=================================================================================================

bool Interface_Check::Complies(const Interface_CheckStatus status) const
{
  bool now = thewarns.IsNull();
  bool nof = thefails.IsNull();
  switch (status)
  {
    case Interface_CheckOK:
      return (nof && now);
    case Interface_CheckWarning:
      return (nof && !now);
    case Interface_CheckFail:
      return !nof;
    case Interface_CheckAny:
      return true;
    case Interface_CheckMessage:
      return (!nof || !now);
    case Interface_CheckNoFail:
      return nof;
    default:
      break;
  }
  return false;
}

//=================================================================================================

bool Interface_Check::Complies(const occ::handle<TCollection_HAsciiString>& mess,
                                           const int                  incl,
                                           const Interface_CheckStatus             status) const
{
  if (mess.IsNull())
    return false;
  int lng = mess->Length();
  if (status == Interface_CheckWarning || status == Interface_CheckAny)
  {
    int i, nb = NbWarnings();
    for (i = nb; i > 0; i--)
    {
      occ::handle<TCollection_HAsciiString> ames = Warning(i);
      if (incl == 0)
      {
        if (mess->IsSameString(ames))
          return true;
      }
      if (incl < 0)
      {
        if (ames->Location(mess, 1, ames->Length()) > 0)
          return true;
      }
      if (incl < 0)
      {
        if (mess->Location(mess, 1, lng) > 0)
          return true;
      }
    }
  }
  if (status == Interface_CheckFail || status == Interface_CheckAny)
  {
    int i, nb = NbWarnings();
    for (i = nb; i > 0; i--)
    {
      occ::handle<TCollection_HAsciiString> ames = Warning(i);
      if (incl == 0)
      {
        if (mess->IsSameString(ames))
          return true;
      }
      if (incl < 0)
      {
        if (ames->Location(mess, 1, ames->Length()) > 0)
          return true;
      }
      if (incl < 0)
      {
        if (mess->Location(mess, 1, lng) > 0)
          return true;
      }
    }
  }
  return false;
}

//=================================================================================================

bool Interface_Check::HasEntity() const
{
  return (!theent.IsNull());
}

//=================================================================================================

const occ::handle<Standard_Transient>& Interface_Check::Entity() const
{
  return theent;
}

//=================================================================================================

void Interface_Check::Clear()
{
  thefails.Nullify();
  thefailo.Nullify();
  thewarns.Nullify();
  thewarno.Nullify();
  theinfos.Nullify();
  theinfoo.Nullify();
  theent.Nullify();
}

//=================================================================================================

void Interface_Check::ClearFails()
{
  thefails.Nullify();
  thefailo.Nullify();
}

//=================================================================================================

void Interface_Check::ClearWarnings()
{
  thewarns.Nullify();
  thewarno.Nullify();
}

//=================================================================================================

void Interface_Check::ClearInfoMsgs()
{
  theinfos.Nullify();
  theinfoo.Nullify();
}

//=================================================================================================

bool Interface_Check::Remove(const occ::handle<TCollection_HAsciiString>& mess,
                                         const int                  incl,
                                         const Interface_CheckStatus             status)
{
  if (mess.IsNull())
    return false;
  bool res = false;
  int lng = mess->Length();
  if (status == Interface_CheckWarning || status == Interface_CheckAny)
  {
    int i, nb = NbWarnings();
    for (i = nb; i > 0; i--)
    {
      bool                 rem  = false;
      occ::handle<TCollection_HAsciiString> ames = Warning(i);
      if (incl == 0)
        rem = mess->IsSameString(ames);
      if (incl < 0)
        rem = (ames->Location(mess, 1, ames->Length()) > 0);
      if (incl < 0)
        rem = (mess->Location(mess, 1, lng) > 0);
      if (rem)
      {
        thewarns->Remove(i);
        thewarno->Remove(i);
        res = rem;
      }
    }
  }
  if (status == Interface_CheckFail || status == Interface_CheckAny)
  {
    int i, nb = NbWarnings();
    for (i = nb; i > 0; i--)
    {
      bool                 rem  = false;
      occ::handle<TCollection_HAsciiString> ames = Warning(i);
      if (incl == 0)
        rem = mess->IsSameString(ames);
      if (incl < 0)
        rem = (ames->Location(mess, 1, ames->Length()) > 0);
      if (incl < 0)
        rem = (mess->Location(mess, 1, lng) > 0);
      if (rem)
      {
        thefails->Remove(i);
        thefailo->Remove(i);
        res = rem;
      }
    }
  }
  return res;
}

//=================================================================================================

bool Interface_Check::Mend(const char* const pref, const int num)
{
  int i, n1 = num, n2 = num;
  if (pref && pref[2] == '\0')
  {
    if (pref[0] == 'F' && pref[1] == 'M')
      return Mend("Mended", num);
    if (pref[0] == 'C' && pref[1] == 'A')
    {
      Clear();
      return true;
    }
    if (pref[0] == 'C' && pref[1] == 'W')
    {
      if (num == 0)
      {
        ClearWarnings();
        return true;
      }
      if (num < 0 || num > NbWarnings())
        return false;
      thewarns->Remove(num);
      thewarno->Remove(num);
      return true;
    }
    if (pref[0] == 'C' && pref[1] == 'F')
    {
      if (num == 0)
      {
        ClearFails();
        return true;
      }
      if (num < 0 || num > NbFails())
        return false;
      thefails->Remove(num);
      thefailo->Remove(num);
      return true;
    }
  }
  //  Common case
  if (num == 0)
  {
    n1 = 1;
    n2 = NbFails();
    for (i = n2; i >= n1; i--)
      Mend(pref, i);
    return true;
  }
  else if (num < 0 || num > NbFails())
    return false;
  //  A message
  occ::handle<TCollection_HAsciiString> strf = thefails->Value(num);
  occ::handle<TCollection_HAsciiString> stro = thefailo->Value(num);
  if (pref && pref[0] != '\0')
  {
    strf->Insert(1, " : ");
    strf->Insert(1, pref);
    if (stro != strf)
    {
      stro->Insert(1, " : ");
      stro->Insert(1, pref);
    }
  }
  thefails->Remove(num);
  thefailo->Remove(num);
  if (stro == strf)
    AddFail(strf);
  else
    AddFail(strf, stro);
  return true;
}

//=================================================================================================

void Interface_Check::SetEntity(const occ::handle<Standard_Transient>& anentity)
{
  theent = anentity;
}

//=================================================================================================

void Interface_Check::GetEntity(const occ::handle<Standard_Transient>& anentity)
{
  if (theent.IsNull())
    SetEntity(anentity);
}

// .. GetMessages, takes messages by accumulating them with its own
// .. GetAsWarning, takes messages by accumulating them and
//    considering them all as "Warning". Furthermore, according to <failsonly>
//      failsonly True  : do not take original Warnings
//      failsonly False : take them as well

//=================================================================================================

void Interface_Check::GetMessages(const occ::handle<Interface_Check>& other)
{
  int nb, i;
  if ((nb = other->NbFails()) != 0)
  {
    if (thefails.IsNull())
      thefails = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
    if (thefailo.IsNull())
      thefailo = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
    for (i = 1; i <= nb; i++)
      thefails->Append(other->Fail(i, true));
    for (i = 1; i <= nb; i++)
      thefailo->Append(other->Fail(i, false));
  }
  if ((nb = other->NbWarnings()) != 0)
  {
    if (thewarns.IsNull())
      thewarns = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
    if (thewarno.IsNull())
      thewarno = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
    for (i = 1; i <= nb; i++)
      thewarns->Append(other->Warning(i, true));
    for (i = 1; i <= nb; i++)
      thewarno->Append(other->Warning(i, false));
  }
  if ((nb = other->NbInfoMsgs()) != 0)
  {
    if (theinfos.IsNull())
      theinfos = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
    if (theinfoo.IsNull())
      theinfoo = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
    for (i = 1; i <= nb; i++)
      theinfos->Append(other->InfoMsg(i, true));
    for (i = 1; i <= nb; i++)
      theinfoo->Append(other->InfoMsg(i, false));
  }
}

//=================================================================================================

void Interface_Check::GetAsWarning(const occ::handle<Interface_Check>& other,
                                   const bool         failsonly)
{
  int nb, i;
  if ((nb = other->NbFails()) != 0)
  {
    if (thewarns.IsNull())
      thewarns = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
    if (thewarno.IsNull())
      thewarno = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
    for (i = 1; i <= nb; i++)
      thewarns->Append(other->Fail(i, true));
    for (i = 1; i <= nb; i++)
      thewarno->Append(other->Fail(i, false));
  }
  if ((nb = other->NbWarnings()) != 0 || !failsonly)
  {
    if (thewarns.IsNull())
      thewarns = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
    if (thewarno.IsNull())
      thewarno = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
    for (i = 1; i <= nb; i++)
      thewarns->Append(other->Warning(i, true));
    for (i = 1; i <= nb; i++)
      thewarno->Append(other->Warning(i, false));
  }
}

//=================================================================================================

void Interface_Check::Print(Standard_OStream&      S,
                            const int level,
                            const int final) const
{
  int j, nb = NbFails();

  if (level >= 1)
  {
    nb = NbFails();
    //    if (nb > 0) S<<" -->   Fails    : "<<nb<<std::endl;
    for (j = 1; j <= nb; j++)
    {
      if (final >= 0)
        S << CFail(j, true) << "\n";
      else
        S << CFail(j, false) << "\n";
      //      if (final == 0)
      //	if (Fail(j,true) != Fail(j,false))
      //	  S<<"[original:"<<CFail(j,false)<<"]\n";
    }
  }

  if (level >= 2)
  {
    nb = NbWarnings();
    //    if (nb > 0) S<<" -->   Warnings : "<<nb<<std::endl;
    for (j = 1; j <= nb; j++)
    {
      if (final >= 0)
        S << CWarning(j, true) << "\n";
      else
        S << CWarning(j, false) << "\n";
      //      if (final == 0)
      //	if (Warning(j,true) != Warning(j,false))
      //	  S<<"[original:"<<CWarning(j,false)<<"]\n";
    }
  }

  //   InfoMsg : causal rule (user message)
  if (level >= 0)
  {
    nb = NbInfoMsgs();
    //    if (nb > 0) S<<" -->   Infos    : "<<nb<<std::endl;
    for (j = 1; j <= nb; j++)
    {
      if (final >= 0)
        S << CInfoMsg(j, true) << "\n";
      else
        S << CInfoMsg(j, false) << "\n";
      //      if (final == 0)
      //	if (InfoMsg(j,true) != InfoMsg(j,false))
      //	  S<<"[original:"<<CInfoMsg(j,false)<<"]\n";
    }
  }
}

//=================================================================================================

void Interface_Check::Trace(const int level, const int final) const
{
  // clang-format off
  Message_Messenger::StreamBuffer sout = Message::SendInfo(); // shouldn't it be changed to SendTrace()?
  // clang-format on
  Print(sout, level, final);
}
