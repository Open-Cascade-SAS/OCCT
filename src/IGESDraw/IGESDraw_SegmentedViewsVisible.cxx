// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESDraw_SegmentedViewsVisible.ixx>


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
    Standard_DimensionMismatch::Raise("IGESDraw_SegmentedViewsVisible : Init");

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
