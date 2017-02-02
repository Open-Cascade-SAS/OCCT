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

#include <IGESData_LineFontEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESDraw_SegmentedViewsVisible.hxx>
#include <IGESGraph_Color.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_SegmentedViewsVisible,IGESData_ViewKindEntity)

IGESDraw_SegmentedViewsVisible::IGESDraw_SegmentedViewsVisible ()    {  }


    void IGESDraw_SegmentedViewsVisible::Init
  (const Handle(IGESDraw_HArray1OfViewKindEntity)&  allViews,
   const Handle(TColStd_HArray1OfReal)&             allBreakpointParameters,
   const Handle(TColStd_HArray1OfInteger)&          allDisplayFlags,
   const Handle(TColStd_HArray1OfInteger)&          allColorValues,
   const Handle(IGESGraph_HArray1OfColor)&          allColorDefinitions,
   const Handle(TColStd_HArray1OfInteger)&          allLineFontValues,
   const Handle(IGESBasic_HArray1OfLineFontEntity)& allLineFontDefinitions,
   const Handle(TColStd_HArray1OfInteger)&          allLineWeights)
{
  Standard_Integer Len = allViews->Length();
  if ( allViews->Lower() != 1 ||
      (allBreakpointParameters->Lower() != 1 || allBreakpointParameters->Length() != Len) ||
      (allDisplayFlags->Lower() != 1 || allDisplayFlags->Length() != Len) ||
      (allColorValues->Lower()  != 1 || allColorValues->Length()  != Len) ||
      (allColorDefinitions->Lower() != 1 || allColorDefinitions->Length() != Len) ||
      (allLineFontValues->Lower()   != 1 || allLineFontValues->Length()   != Len) ||
      (allLineFontDefinitions->Lower() != 1 || allLineFontDefinitions->Length()  != Len) ||
      (allLineWeights->Lower()  != 1 || allLineWeights->Length()  != Len) )
    throw Standard_DimensionMismatch("IGESDraw_SegmentedViewsVisible : Init");

  theViews                = allViews;
  theBreakpointParameters = allBreakpointParameters;
  theDisplayFlags         = allDisplayFlags;
  theColorValues          = allColorValues;
  theColorDefinitions     = allColorDefinitions;
  theLineFontValues       = allLineFontValues;
  theLineFontDefinitions  = allLineFontDefinitions;
  theLineWeights          = allLineWeights;
  InitTypeAndForm(402,19);
}

    Standard_Boolean IGESDraw_SegmentedViewsVisible::IsSingle () const
{
  return Standard_False;
}

    Standard_Integer IGESDraw_SegmentedViewsVisible::NbViews () const
{
  return theViews->Length();
}

    Standard_Integer IGESDraw_SegmentedViewsVisible::NbSegmentBlocks () const
{
  return theViews->Length();
}

    Handle(IGESData_ViewKindEntity) IGESDraw_SegmentedViewsVisible::ViewItem
  (const Standard_Integer ViewIndex) const
{
  return theViews->Value(ViewIndex);
}

    Standard_Real IGESDraw_SegmentedViewsVisible::BreakpointParameter
  (const Standard_Integer BreakpointIndex) const
{
  return theBreakpointParameters->Value(BreakpointIndex);
}

    Standard_Integer IGESDraw_SegmentedViewsVisible::DisplayFlag
  (const Standard_Integer FlagIndex) const
{
  return theDisplayFlags->Value(FlagIndex);
}

    Standard_Boolean IGESDraw_SegmentedViewsVisible::IsColorDefinition
  (const Standard_Integer ColorIndex) const
{
  return ( !theColorDefinitions->Value(ColorIndex).IsNull() );
}

    Standard_Integer IGESDraw_SegmentedViewsVisible::ColorValue
  (const Standard_Integer ColorIndex) const
{
  return theColorValues->Value(ColorIndex);
}

    Handle(IGESGraph_Color) IGESDraw_SegmentedViewsVisible::ColorDefinition
  (const Standard_Integer ColorIndex) const
{
  return theColorDefinitions->Value(ColorIndex);
}

    Standard_Boolean IGESDraw_SegmentedViewsVisible::IsFontDefinition
  (const Standard_Integer FontIndex) const
{
  return ( !theLineFontDefinitions->Value(FontIndex).IsNull() );
}

    Standard_Integer IGESDraw_SegmentedViewsVisible::LineFontValue
  (const Standard_Integer FontIndex) const
{
  return theLineFontValues->Value(FontIndex);
}

    Handle(IGESData_LineFontEntity) 
    IGESDraw_SegmentedViewsVisible::LineFontDefinition
  (const Standard_Integer FontIndex) const
{
  return theLineFontDefinitions->Value(FontIndex);
}

    Standard_Integer IGESDraw_SegmentedViewsVisible::LineWeightItem
  (const Standard_Integer WeightIndex) const
{
  return theLineWeights->Value(WeightIndex);
}
