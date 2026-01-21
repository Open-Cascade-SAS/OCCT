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

#include <IFSelect_Activator.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <MoniTool_Macros.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_DataMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_Activator, Standard_Transient)

static NCollection_DataMap<TCollection_AsciiString, int>     thedico;
static NCollection_Sequence<int>                             thenums, themodes;
static NCollection_Sequence<occ::handle<Standard_Transient>> theacts;

void IFSelect_Activator::Adding(const occ::handle<IFSelect_Activator>& actor,
                                const int                              number,
                                const char* const                      command,
                                const int                              mode)
{
#ifdef OCCT_DEBUG
  if (thedico.IsBound(command))
  {
    std::cout << "****  XSTEP commands, name conflict on " << command
              << " first defined remains  ****" << std::endl;
    //    throw Standard_DomainError("IFSelect_Activator : Add");
  }
#endif

  thedico.Bind(command, thenums.Length() + 1);

  thenums.Append(number);
  theacts.Append(actor);
  themodes.Append(mode);
}

void IFSelect_Activator::Add(const int number, const char* const command) const
{
  Adding(this, number, command, 0);
}

void IFSelect_Activator::AddSet(const int number, const char* const command) const
{
  Adding(this, number, command, 1);
}

void IFSelect_Activator::Remove(const char* const command)
{
  thedico.UnBind(command);
}

bool IFSelect_Activator::Select(const char* const                command,
                                int&                             number,
                                occ::handle<IFSelect_Activator>& actor)
{
  int num;
  if (!thedico.Find(command, num))
    return false;
  number = thenums(num);
  actor  = occ::down_cast<IFSelect_Activator>(theacts(num));
  return true;
}

int IFSelect_Activator::Mode(const char* const command)
{
  int num;
  if (!thedico.Find(command, num))
    return -1;
  return themodes(num);
}

occ::handle<NCollection_HSequence<TCollection_AsciiString>> IFSelect_Activator::Commands(
  const int         mode,
  const char* const command)
{
  int                                                         num;
  NCollection_DataMap<TCollection_AsciiString, int>::Iterator iter(thedico);
  occ::handle<NCollection_HSequence<TCollection_AsciiString>> list =
    new NCollection_HSequence<TCollection_AsciiString>();
  for (; iter.More(); iter.Next())
  {
    if (!iter.Key().StartsWith(command))
      continue;
    if (mode < 0)
    {
      DeclareAndCast(IFSelect_Activator, acti, theacts(iter.Value()));
      if (acti.IsNull())
        continue;
      if (command[0] == '\0' || !strcmp(command, acti->Group()))
        list->Append(iter.Key());
    }
    else
    {
      num = iter.Value();
      if (themodes(num) == mode)
        list->Append(iter.Key());
    }
  }
  return list;
}

IFSelect_Activator::IFSelect_Activator()
    : thegroup("XSTEP")
{
}

void IFSelect_Activator::SetForGroup(const char* const group, const char* const file)
{
  thegroup.Clear();
  thegroup.AssignCat(group);
  thefile.Clear();
  thefile.AssignCat(file);
}

const char* IFSelect_Activator::Group() const
{
  return thegroup.ToCString();
}

const char* IFSelect_Activator::File() const
{
  return thefile.ToCString();
}
