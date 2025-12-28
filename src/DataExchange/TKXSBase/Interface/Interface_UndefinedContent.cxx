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

#include <Interface_CopyTool.hxx>
#include <Interface_InterfaceMismatch.hxx>
#include <Interface_UndefinedContent.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_UndefinedContent, Standard_Transient)

#define Content_TypeField 31
#define Content_LocalField 7
#define Content_LocalRef 1
#define Content_LocalShift 5
#define Content_NumberShift 8

// This class provides basic services for defining entities
// Unknown (this, for lack of double inheritance) : literal description

Interface_UndefinedContent::Interface_UndefinedContent() // Unknown
{
  thenbparams = 0;
  thenbstr    = 0;
}

// ....  The Parameters ....

// The parameters are organized as follows (no FileParameter) :
// - a list of descriptors (each fitting in an integer) in array, with
// the location (Entity/literal), the type (ParamType), the rank in the
// the ad hoc list (Entity or literal)
// (5 right bits for type; 3 bits for location; remaining for address)
// - for literals, a list of String (array)
// - for Entity, a list of Entities (EntityList)
// The "memory space" aspect makes Arrays preferable to Sequences, even though
// the latter are simpler to manage
// Indeed, we must reserve and extend if necessary ...

int Interface_UndefinedContent::NbParams() const
{
  return thenbparams;
}

int Interface_UndefinedContent::NbLiterals() const
{
  return thenbstr;
}

bool Interface_UndefinedContent::ParamData(const int                              num,
                                           Interface_ParamType&                   ptype,
                                           occ::handle<Standard_Transient>&       ent,
                                           occ::handle<TCollection_HAsciiString>& val) const
{
  if (num < 1 || num > thenbparams)
    throw Standard_OutOfRange("Interface UndefinedContent : ParamData");
  int desc  = theparams->Value(num);
  int local = ((desc >> Content_LocalShift) & Content_LocalField);
  ptype     = Interface_ParamType(desc & Content_TypeField);
  int adr   = desc >> Content_NumberShift;
  if (local == Content_LocalRef)
    ent = theentities.Value(adr);
  else
    val = thevalues->Value(adr);
  return (local == Content_LocalRef);
}

Interface_ParamType Interface_UndefinedContent::ParamType(const int num) const
{
  return Interface_ParamType(theparams->Value(num) & Content_TypeField);
}

bool Interface_UndefinedContent::IsParamEntity(const int num) const
{
  return (((theparams->Value(num) >> Content_LocalShift) & Content_LocalField) == Content_LocalRef);
}

occ::handle<Standard_Transient> Interface_UndefinedContent::ParamEntity(const int num) const
{
  int desc = theparams->Value(num);
  if (((desc >> Content_LocalShift) & Content_LocalField) != Content_LocalRef)
    throw Interface_InterfaceError("UndefinedContent : Param is not Entity type");
  return theentities.Value(desc >> Content_NumberShift);
}

occ::handle<TCollection_HAsciiString> Interface_UndefinedContent::ParamValue(const int num) const
{
  int desc = theparams->Value(num);
  if (((desc >> Content_LocalShift) & Content_LocalField) != 0)
    throw Interface_InterfaceError("UndefinedContent : Param is not literal");
  return thevalues->Value(desc >> Content_NumberShift);
}

// ....  Parameter filling ....

void Interface_UndefinedContent::Reservate(const int nb, const int nblit)
{
  // Reservation: If enlargement, copy old to new ...
  if (nb > thenbparams)
  { // Total reservation
    if (theparams.IsNull())
      theparams = new NCollection_HArray1<int>(1, nb);
    else if (nb > theparams->Length())
    {
      int nbnew = 2 * thenbparams; // reserve a bit more
      if (nbnew < nb)
        nbnew = nb;
      occ::handle<NCollection_HArray1<int>> newparams = new NCollection_HArray1<int>(1, nbnew);
      for (int i = 1; i <= thenbparams; i++)
        newparams->SetValue(i, theparams->Value(i));
      theparams = newparams;
    }
  }

  if (nblit > thenbstr)
  { // Literal reservation
    if (thevalues.IsNull())
      thevalues = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nblit);
    else if (nblit > thevalues->Length())
    {
      int nbnew = 2 * thenbstr; // reserve a bit more
      if (nbnew < nblit)
        nbnew = nblit;
      occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> newvalues =
        new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nbnew);
      for (int i = 1; i <= thenbstr; i++)
        newvalues->SetValue(i, thevalues->Value(i));
      thevalues = newvalues;
    }
  }
  //  Entities: Parameters - Literals. In fact, EntityList is dynamic
}

void Interface_UndefinedContent::AddLiteral(const Interface_ParamType                    ptype,
                                            const occ::handle<TCollection_HAsciiString>& val)
{
  Reservate(thenbparams + 1, thenbstr + 1);
  int desc = int(ptype);
  thenbstr++;
  thenbparams++;
  thevalues->SetValue(thenbstr, val);
  desc += (thenbstr << Content_NumberShift);
  theparams->SetValue(thenbparams, desc);
}

void Interface_UndefinedContent::AddEntity(const Interface_ParamType              ptype,
                                           const occ::handle<Standard_Transient>& ent)
{
  Reservate(thenbparams + 1, 0);
  int desc = int(ptype);
  theentities.Append(ent);
  desc += Content_LocalRef << Content_LocalShift; // "It is an Entity"
  thenbparams++;                                  // Rank:  thenbparams - thenbstr
  desc += ((thenbparams - thenbstr) << Content_NumberShift);
  theparams->SetValue(thenbparams, desc);
}

// ....  Parameter editing ....

void Interface_UndefinedContent::RemoveParam(const int num)
{
  int  desc  = theparams->Value(num);
  int  rang  = desc >> Content_NumberShift;
  int  local = ((desc >> Content_LocalShift) & Content_LocalField);
  bool c1ent = (local == Content_LocalRef);
  //    Remove an Entity
  if (c1ent)
    theentities.Remove(rang);
  //    Remove a Literal
  else
  { // thevalues->Remove(rang)  but it is an array
    for (int i = rang + 1; i <= thenbstr; i++)
      thevalues->SetValue(i - 1, thevalues->Value(i));
    occ::handle<TCollection_HAsciiString> nulstr;
    thevalues->SetValue(thenbstr, nulstr);
    thenbstr--;
  }
  //    Remove this parameter from the list (which is an array)
  int np; // svv Jan11 2000 : porting on DEC
  for (np = num + 1; np <= thenbparams; np++)
    theparams->SetValue(np - 1, theparams->Value(np));
  theparams->SetValue(thenbparams, 0);
  thenbparams--;
  //    Renumber, Entity or Literal, depending
  for (np = 1; np <= thenbparams; np++)
  {
    desc = theparams->Value(np);
    if (((desc >> Content_LocalShift) & Content_LocalField) == local
        && (desc >> Content_NumberShift) > rang)
      theparams->SetValue(np, desc - (1 << Content_NumberShift));
  }
}

void Interface_UndefinedContent::SetLiteral(const int                                    num,
                                            const Interface_ParamType                    ptype,
                                            const occ::handle<TCollection_HAsciiString>& val)
{
  //  Change a parameter. If already literal, simple substitution
  //  If Entity, remove the entity and renumber the "Entity" Parameters
  int  desc  = theparams->Value(num);
  int  rang  = desc >> Content_NumberShift;
  int  local = ((desc >> Content_LocalShift) & Content_LocalField);
  bool c1ent = (local == Content_LocalRef);
  if (c1ent)
  {
    //  Entity: remove it and renumber the "Entity" type Parameters
    theentities.Remove(rang);
    for (int i = 1; i <= thenbparams; i++)
    {
      desc = theparams->Value(i);
      if (((desc >> Content_LocalShift) & Content_LocalField) == Content_LocalRef
          && (desc >> Content_NumberShift) > rang)
        theparams->SetValue(i, desc - (1 << Content_NumberShift));
    }
    //  And prepare arrival of an additional Literal
    Reservate(thenbparams, thenbstr + 1);
    thenbstr++;
    rang = thenbstr;
  }
  //  Put the new value in place and rebuild the Param descriptor
  thevalues->SetValue(rang, val);
  desc = int(ptype) + (rang << Content_NumberShift);
  theparams->SetValue(num, desc);
}

void Interface_UndefinedContent::SetEntity(const int                              num,
                                           const Interface_ParamType              ptype,
                                           const occ::handle<Standard_Transient>& ent)
{
  //  Change a Parameter. If already Entity, simple substitution
  //  If Literal, remove its value and renumber the Literal parameters
  int  desc  = theparams->Value(num);
  int  rang  = desc >> Content_NumberShift;
  int  local = ((desc >> Content_LocalShift) & Content_LocalField);
  bool c1ent = (local == Content_LocalRef);
  if (!c1ent)
  {
    //  Literal: remove it and renumber the "Entity" type Parameters
    //  (Remove Literal but in an array)
    int i; // svv Jan11 2000 : porting on DEC
    for (i = rang + 1; i <= thenbstr; i++)
      thevalues->SetValue(i - 1, thevalues->Value(i));
    occ::handle<TCollection_HAsciiString> nulstr;
    thevalues->SetValue(thenbstr, nulstr);

    for (i = 1; i <= thenbparams; i++)
    {
      desc = theparams->Value(i);
      if (((desc >> Content_LocalShift) & Content_LocalField) == 0
          && (desc >> Content_NumberShift) > rang)
        theparams->SetValue(i, desc - (1 << Content_NumberShift));
    }
    //  And prepare arrival of an additional Entity
    thenbstr--;
    rang = thenbparams - thenbstr;
    //  Put the new value in place and rebuild the Param descriptor
    theentities.Append(ent);
  }
  else
    theentities.SetValue(rang, ent);

  desc = int(ptype) + (Content_LocalRef << Content_LocalShift) + (rang << Content_NumberShift);
  theparams->SetValue(num, desc);
}

void Interface_UndefinedContent::SetEntity(const int                              num,
                                           const occ::handle<Standard_Transient>& ent)
{
  //  Change the Entity defined by a Parameter, all other things being equal,
  //  PROVIDED that it is already an "Entity" type Parameter
  int  desc  = theparams->Value(num);
  int  rang  = desc >> Content_NumberShift;
  int  local = ((desc >> Content_LocalShift) & Content_LocalField);
  bool c1ent = (local == Content_LocalRef);
  if (!c1ent)
    throw Interface_InterfaceError("UndefinedContent : SetEntity");
  theentities.SetValue(rang, ent);
}

Interface_EntityList Interface_UndefinedContent::EntityList() const
{
  return theentities;
}

//    all copies of UndefinedEntity are similar ... Common part
void Interface_UndefinedContent::GetFromAnother(
  const occ::handle<Interface_UndefinedContent>& other,
  Interface_CopyTool&                            TC)
{
  int nb = other->NbParams();
  theentities.Clear();
  thevalues.Nullify();
  theparams.Nullify();
  Reservate(nb, other->NbLiterals());

  occ::handle<Standard_Transient> ent;
  for (int i = 1; i <= nb; i++)
  {
    Interface_ParamType                   ptype;
    occ::handle<TCollection_HAsciiString> val;
    if (other->ParamData(i, ptype, ent, val))
    {
      ent = TC.Transferred(ent);
      AddEntity(ptype, ent);
    }
    else
      AddLiteral(ptype, val);
  }
}
