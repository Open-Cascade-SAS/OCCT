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

#include <IGESDraw_SegmentedViewsVisible.hxx>
#include <IGESGraph_Color.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_SegmentedViewsVisible, IGESData_ViewKindEntity)

IGESDraw_SegmentedViewsVisible::IGESDraw_SegmentedViewsVisible() {}

void IGESDraw_SegmentedViewsVisible::Init(
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>>& allViews,
  const occ::handle<NCollection_HArray1<double>>&                       allBreakpointParameters,
  const occ::handle<NCollection_HArray1<int>>&                          allDisplayFlags,
  const occ::handle<NCollection_HArray1<int>>&                          allColorValues,
  const occ::handle<NCollection_HArray1<occ::handle<IGESGraph_Color>>>& allColorDefinitions,
  const occ::handle<NCollection_HArray1<int>>&                          allLineFontValues,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_LineFontEntity>>>&
                                               allLineFontDefinitions,
  const occ::handle<NCollection_HArray1<int>>& allLineWeights)
{
  int Len = allViews->Length();
  if (allViews->Lower() != 1
      || (allBreakpointParameters->Lower() != 1 || allBreakpointParameters->Length() != Len)
      || (allDisplayFlags->Lower() != 1 || allDisplayFlags->Length() != Len)
      || (allColorValues->Lower() != 1 || allColorValues->Length() != Len)
      || (allColorDefinitions->Lower() != 1 || allColorDefinitions->Length() != Len)
      || (allLineFontValues->Lower() != 1 || allLineFontValues->Length() != Len)
      || (allLineFontDefinitions->Lower() != 1 || allLineFontDefinitions->Length() != Len)
      || (allLineWeights->Lower() != 1 || allLineWeights->Length() != Len))
    throw Standard_DimensionMismatch("IGESDraw_SegmentedViewsVisible : Init");

  theViews                = allViews;
  theBreakpointParameters = allBreakpointParameters;
  theDisplayFlags         = allDisplayFlags;
  theColorValues          = allColorValues;
  theColorDefinitions     = allColorDefinitions;
  theLineFontValues       = allLineFontValues;
  theLineFontDefinitions  = allLineFontDefinitions;
  theLineWeights          = allLineWeights;
  InitTypeAndForm(402, 19);
}

bool IGESDraw_SegmentedViewsVisible::IsSingle() const
{
  return false;
}

int IGESDraw_SegmentedViewsVisible::NbViews() const
{
  return theViews->Length();
}

int IGESDraw_SegmentedViewsVisible::NbSegmentBlocks() const
{
  return theViews->Length();
}

occ::handle<IGESData_ViewKindEntity> IGESDraw_SegmentedViewsVisible::ViewItem(
  const int ViewIndex) const
{
  return theViews->Value(ViewIndex);
}

double IGESDraw_SegmentedViewsVisible::BreakpointParameter(const int BreakpointIndex) const
{
  return theBreakpointParameters->Value(BreakpointIndex);
}

int IGESDraw_SegmentedViewsVisible::DisplayFlag(const int FlagIndex) const
{
  return theDisplayFlags->Value(FlagIndex);
}

bool IGESDraw_SegmentedViewsVisible::IsColorDefinition(const int ColorIndex) const
{
  return (!theColorDefinitions->Value(ColorIndex).IsNull());
}

int IGESDraw_SegmentedViewsVisible::ColorValue(const int ColorIndex) const
{
  return theColorValues->Value(ColorIndex);
}

occ::handle<IGESGraph_Color> IGESDraw_SegmentedViewsVisible::ColorDefinition(
  const int ColorIndex) const
{
  return theColorDefinitions->Value(ColorIndex);
}

bool IGESDraw_SegmentedViewsVisible::IsFontDefinition(const int FontIndex) const
{
  return (!theLineFontDefinitions->Value(FontIndex).IsNull());
}

int IGESDraw_SegmentedViewsVisible::LineFontValue(const int FontIndex) const
{
  return theLineFontValues->Value(FontIndex);
}

occ::handle<IGESData_LineFontEntity> IGESDraw_SegmentedViewsVisible::LineFontDefinition(
  const int FontIndex) const
{
  return theLineFontDefinitions->Value(FontIndex);
}

int IGESDraw_SegmentedViewsVisible::LineWeightItem(const int WeightIndex) const
{
  return theLineWeights->Value(WeightIndex);
}
