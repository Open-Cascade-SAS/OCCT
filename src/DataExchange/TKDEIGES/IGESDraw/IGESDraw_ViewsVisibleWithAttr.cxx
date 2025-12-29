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

#include <IGESDraw_ViewsVisibleWithAttr.hxx>
#include <IGESGraph_Color.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_ViewsVisibleWithAttr, IGESData_ViewKindEntity)

IGESDraw_ViewsVisibleWithAttr::IGESDraw_ViewsVisibleWithAttr() = default;

// This class inherits from IGESData_ViewKindEntity

void IGESDraw_ViewsVisibleWithAttr::Init(
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>>& allViewEntities,
  const occ::handle<NCollection_HArray1<int>>&                                  allLineFonts,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_LineFontEntity>>>& allLineDefinitions,
  const occ::handle<NCollection_HArray1<int>>&                                  allColorValues,
  const occ::handle<NCollection_HArray1<occ::handle<IGESGraph_Color>>>&         allColorDefinitions,
  const occ::handle<NCollection_HArray1<int>>&                                  allLineWeights,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&     allDisplayEntities)
{
  if (!allViewEntities.IsNull())
  {
    int Exp = allViewEntities->Length();
    if (allViewEntities->Lower() != 1
        || (allLineFonts->Lower() != 1 || allLineFonts->Length() != Exp)
        || (allColorValues->Lower() != 1 || allColorValues->Length() != Exp)
        || (allLineWeights->Lower() != 1 || allLineWeights->Length() != Exp)
        || (allLineDefinitions->Lower() != 1 || allLineDefinitions->Length() != Exp)
        || (allColorDefinitions->Lower() != 1 || allColorDefinitions->Length() != Exp))
      throw Standard_DimensionMismatch("IGESDraw_ViewsVisibleWithAttr : Init");
  }
  if (!allDisplayEntities.IsNull())
    if (allDisplayEntities->Lower() != 1)
      throw Standard_DimensionMismatch("IGESDraw_ViewsVisibleWithAttr : Init");

  theViewEntities     = allViewEntities;
  theLineFonts        = allLineFonts;
  theLineDefinitions  = allLineDefinitions;
  theColorValues      = allColorValues;
  theColorDefinitions = allColorDefinitions;
  theLineWeights      = allLineWeights;
  theDisplayEntities  = allDisplayEntities;
  InitTypeAndForm(402, 4);
}

void IGESDraw_ViewsVisibleWithAttr::InitImplied(
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& allDisplayEntity)
{
  if (!allDisplayEntity.IsNull())
    if (allDisplayEntity->Lower() != 1)
      throw Standard_DimensionMismatch("IGESDraw_ViewsVisibleWithAttr : InitImplied");
  theDisplayEntities = allDisplayEntity;
}

bool IGESDraw_ViewsVisibleWithAttr::IsSingle() const
{
  return false;
  // Is redefined to return Standard_Flase, since <me> contains list of Views
}

int IGESDraw_ViewsVisibleWithAttr::NbViews() const
{
  return (theViewEntities.IsNull() ? 0 : theViewEntities->Length());
}

int IGESDraw_ViewsVisibleWithAttr::NbDisplayedEntities() const
{
  return (theDisplayEntities.IsNull() ? 0 : theDisplayEntities->Length());
  // Return 0 if HArray1 theDisplyEntities id NULL Handle
}

occ::handle<IGESData_ViewKindEntity> IGESDraw_ViewsVisibleWithAttr::ViewItem(const int Index) const
{
  return theViewEntities->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

int IGESDraw_ViewsVisibleWithAttr::LineFontValue(const int Index) const
{
  return theLineFonts->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

bool IGESDraw_ViewsVisibleWithAttr::IsFontDefinition(const int Index) const
{
  return (!(theLineDefinitions->Value(Index)).IsNull());
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

occ::handle<IGESData_LineFontEntity> IGESDraw_ViewsVisibleWithAttr::FontDefinition(
  const int Index) const
{
  return theLineDefinitions->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

int IGESDraw_ViewsVisibleWithAttr::ColorValue(const int Index) const
{
  return theColorValues->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

bool IGESDraw_ViewsVisibleWithAttr::IsColorDefinition(const int Index) const
{
  return (!(theColorDefinitions->Value(Index)).IsNull());
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

occ::handle<IGESGraph_Color> IGESDraw_ViewsVisibleWithAttr::ColorDefinition(const int Index) const
{
  return theColorDefinitions->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

int IGESDraw_ViewsVisibleWithAttr::LineWeightItem(const int Index) const
{
  return theLineWeights->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

occ::handle<IGESData_IGESEntity> IGESDraw_ViewsVisibleWithAttr::DisplayedEntity(
  const int Index) const
{
  return theDisplayEntities->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
  // if (theDisplayEntity.IsNull()) then NoSuchObject Exception is raised
}
