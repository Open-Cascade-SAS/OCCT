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

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <StepData_Described.hxx>
#include <StepData_ECDescr.hxx>
#include <StepData_EDescr.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_PDescr.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_UndefinedEntity.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(StepData_Protocol, Interface_Protocol)

//  The base Protocol recognizes UnknownEntity
// static TCollection_AsciiString  thename("(DEFAULT)");
static const char* thename = "(DEFAULT)";

StepData_Protocol::StepData_Protocol() {}

int StepData_Protocol::NbResources() const
{
  return 0;
}

occ::handle<Interface_Protocol> StepData_Protocol::Resource(const int /*num*/) const
{
  occ::handle<Interface_Protocol> nulproto;
  return nulproto;
}

int StepData_Protocol::CaseNumber(const occ::handle<Standard_Transient>& obj) const
{
  if (obj.IsNull())
    return 0;
  int num = TypeNumber(obj->DynamicType());
  if (num > 0)
    return num;
  occ::handle<StepData_Described> dc = occ::down_cast<StepData_Described>(obj);
  if (dc.IsNull())
    return 0;
  return DescrNumber(dc->Description());
}

int StepData_Protocol::TypeNumber(const occ::handle<Standard_Type>& atype) const
{
  if (atype == STANDARD_TYPE(StepData_UndefinedEntity))
    return 1;
  return 0;
}

const char* StepData_Protocol::SchemaName(const occ::handle<Interface_InterfaceModel>&) const
{
  return thename;
}

occ::handle<Interface_InterfaceModel> StepData_Protocol::NewModel() const
{
  return new StepData_StepModel;
}

bool StepData_Protocol::IsSuitableModel(
  const occ::handle<Interface_InterfaceModel>& model) const
{
  return model->IsKind(STANDARD_TYPE(StepData_StepModel));
}

occ::handle<Standard_Transient> StepData_Protocol::UnknownEntity() const
{
  return new StepData_UndefinedEntity;
}

bool StepData_Protocol::IsUnknownEntity(const occ::handle<Standard_Transient>& ent) const
{
  if (!ent.IsNull())
    return ent->IsKind(STANDARD_TYPE(StepData_UndefinedEntity));
  return false;
}

//  ####    Description for LateBinding (runtime entity description support)

int StepData_Protocol::DescrNumber(const occ::handle<StepData_EDescr>& adescr) const
{
  if (thedscnum.IsBound(adescr))
    return thedscnum.Find(adescr);
  return 0;
}

void StepData_Protocol::AddDescr(const occ::handle<StepData_EDescr>& adescr, const int CN)
{
  occ::handle<StepData_ESDescr> sd = occ::down_cast<StepData_ESDescr>(adescr);
  thedscnum.Bind(adescr, CN);

  //  Simple descriptor: stored according to its name
  //  Otherwise what to do? Store according to the passed number in alpha-numeric form...
  //  (temporary solution)
  if (!sd.IsNull())
    thedscnam.Bind(sd->TypeName(), sd);
  char fonom[10];
  Sprintf(fonom, "%d", CN);
  thedscnam.Bind(fonom, adescr);
}

bool StepData_Protocol::HasDescr() const
{
  return !thedscnam.IsEmpty();
}

occ::handle<StepData_EDescr> StepData_Protocol::Descr(const int num) const
{
  occ::handle<StepData_EDescr> dsc;
  if (thedscnam.IsEmpty())
    return dsc;
  char fonom[10];
  Sprintf(fonom, "%d", num);
  occ::handle<Standard_Transient> aTDsc;
  if (thedscnam.Find(fonom, aTDsc))
    dsc = occ::down_cast<StepData_EDescr>(aTDsc);
  else
    dsc.Nullify();
  return dsc;
}

occ::handle<StepData_EDescr> StepData_Protocol::Descr(const char* const name,
                                                 const bool anylevel) const
{
  occ::handle<StepData_EDescr> sd;
  if (!thedscnam.IsEmpty())
  {
    occ::handle<Standard_Transient> aTSd;
    if (thedscnam.Find(name, aTSd))
      return occ::down_cast<StepData_EDescr>(aTSd);
  }
  if (!anylevel)
    return sd;

  int i, nb = NbResources();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<StepData_Protocol> sp = occ::down_cast<StepData_Protocol>(Resource(i));
    if (sp.IsNull())
      continue;
    sd = sp->Descr(name, anylevel);
    if (!sd.IsNull())
      return sd;
  }
  return sd;
}

occ::handle<StepData_ESDescr> StepData_Protocol::ESDescr(const char* const name,
                                                    const bool anylevel) const
{
  return occ::down_cast<StepData_ESDescr>(Descr(name, anylevel));
}

occ::handle<StepData_ECDescr> StepData_Protocol::ECDescr(const NCollection_Sequence<TCollection_AsciiString>& names,
                                                    const bool anylevel) const
{
  int                                     i, nb = names.Length();
  occ::handle<StepData_ECDescr>                             cd;
  NCollection_DataMap<occ::handle<Standard_Transient>, int>::Iterator iter(thedscnum);
  for (; iter.More(); iter.Next())
  {
    cd = occ::down_cast<StepData_ECDescr>(iter.Key());
    if (cd.IsNull())
      continue;
    if (cd->NbMembers() != nb)
      continue;
    bool ok = true;
    for (i = 1; i <= nb; i++)
    {
      if (!names(i).IsEqual(cd->Member(i)->TypeName()))
      {
        ok = false;
        break;
      }
    }
    if (ok)
      return cd;
  }
  cd.Nullify();
  if (!anylevel)
    return cd;

  nb = NbResources();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<StepData_Protocol> sp = occ::down_cast<StepData_Protocol>(Resource(i));
    if (sp.IsNull())
      continue;
    cd = sp->ECDescr(names, anylevel);
    if (!cd.IsNull())
      return cd;
  }
  return cd;
}

void StepData_Protocol::AddPDescr(const occ::handle<StepData_PDescr>& pdescr)
{
  thepdescr.Bind(pdescr->Name(), pdescr);
}

occ::handle<StepData_PDescr> StepData_Protocol::PDescr(const char* const name,
                                                  const bool anylevel) const
{
  occ::handle<StepData_PDescr> sd;
  if (!thepdescr.IsEmpty())
  {
    occ::handle<Standard_Transient> aTSd;
    if (thepdescr.Find(name, aTSd))
      return occ::down_cast<StepData_PDescr>(aTSd);
  }
  if (!anylevel)
    return sd;

  int i, nb = NbResources();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<StepData_Protocol> sp = occ::down_cast<StepData_Protocol>(Resource(i));
    if (sp.IsNull())
      continue;
    sd = sp->PDescr(name, anylevel);
    if (!sd.IsNull())
      return sd;
  }
  return sd;
}

void StepData_Protocol::AddBasicDescr(const occ::handle<StepData_ESDescr>& esdescr)
{
  thedscbas.Bind(esdescr->TypeName(), esdescr);
}

occ::handle<StepData_EDescr> StepData_Protocol::BasicDescr(const char* const name,
                                                      const bool anylevel) const
{
  occ::handle<StepData_EDescr> sd;
  if (!thedscbas.IsEmpty())
  {
    occ::handle<Standard_Transient> aTSd;
    if (thedscbas.Find(name, aTSd))
      return occ::down_cast<StepData_EDescr>(aTSd);
  }
  if (!anylevel)
    return sd;

  int i, nb = NbResources();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<StepData_Protocol> sp = occ::down_cast<StepData_Protocol>(Resource(i));
    if (sp.IsNull())
      continue;
    sd = sp->BasicDescr(name, anylevel);
    if (!sd.IsNull())
      return sd;
  }
  return sd;
}
