// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------
// ptv and rln 14.09.2000 BUC60743

#include <IGESBasic_Group.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESBasic_Group, IGESData_IGESEntity)

IGESBasic_Group::IGESBasic_Group()
{
  InitTypeAndForm(402, 1);
}

IGESBasic_Group::IGESBasic_Group(const int nb)
{
  InitTypeAndForm(402, 1);
  if (nb <= 0)
    return;
  theEntities = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nb);
}

void IGESBasic_Group::Init(
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& allEntities)
{
  // ptv and rln September 14, 2000 BUC60743
  // Protection against empty groups
  if (!allEntities.IsNull() && (allEntities->Lower() != 1))
    throw Standard_DimensionMismatch("IGESBasic_Group : Init");
  theEntities = allEntities;
  if (FormNumber() == 0)
    InitTypeAndForm(402, 1);
}

void IGESBasic_Group::SetOrdered(const bool mode)
{
  int fn = FormNumber();
  if (mode)
  {
    if (fn == 0 || fn == 1)
      InitTypeAndForm(402, 14);
    else if (fn == 7)
      InitTypeAndForm(402, 15);
  }
  else
  {
    if (fn == 14)
      InitTypeAndForm(402, 1);
    else if (fn == 15)
      InitTypeAndForm(402, 7);
  }
}

void IGESBasic_Group::SetWithoutBackP(const bool mode)
{
  int fn = FormNumber();
  if (mode)
  {
    if (fn == 0 || fn == 1)
      InitTypeAndForm(402, 7);
    else if (fn == 14)
      InitTypeAndForm(402, 15);
  }
  else
  {
    if (fn == 7)
      InitTypeAndForm(402, 1);
    else if (fn == 15)
      InitTypeAndForm(402, 14);
  }
}

bool IGESBasic_Group::IsOrdered() const
{
  int fn = FormNumber();
  return (fn > 7);
}

bool IGESBasic_Group::IsWithoutBackP() const
{
  int fn = FormNumber();
  return (fn == 7 || fn == 15);
}

void IGESBasic_Group::SetUser(const int type, const int form)
{
  if (type <= 5000)
    throw Standard_OutOfRange("IGESBasic_Group::SetUser");
  InitTypeAndForm(type, form);
}

void IGESBasic_Group::SetNb(const int nb)
{
  int i, oldnb = NbEntities();
  if (nb == oldnb || nb <= 0)
    return;

  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> newents =
    new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nb);
  if (oldnb > nb)
    oldnb = nb;
  for (i = 1; i <= oldnb; i++)
    newents->SetValue(i, theEntities->Value(i));
  theEntities = newents;
}

int IGESBasic_Group::NbEntities() const
{
  return (theEntities.IsNull() ? 0 : theEntities->Length());
}

occ::handle<IGESData_IGESEntity> IGESBasic_Group::Entity(const int Index) const
{
  return theEntities->Value(Index);
}

occ::handle<Standard_Transient> IGESBasic_Group::Value(const int Index) const
{
  return occ::handle<Standard_Transient>(theEntities->Value(Index));
}

void IGESBasic_Group::SetValue(const int Index, const occ::handle<IGESData_IGESEntity>& ent)
{
  theEntities->SetValue(Index, ent);
}
