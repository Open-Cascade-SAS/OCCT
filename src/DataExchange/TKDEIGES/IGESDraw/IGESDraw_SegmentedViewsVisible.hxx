// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESDraw_SegmentedViewsVisible_HeaderFile
#define _IGESDraw_SegmentedViewsVisible_HeaderFile

#include <Standard.hxx>

#include <IGESData_ViewKindEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <IGESGraph_Color.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <Standard_Real.hxx>
class IGESGraph_Color;
class IGESData_LineFontEntity;

//! defines IGESSegmentedViewsVisible, Type <402> Form <19>
//! in package IGESDraw
//!
//! Permits the association of display parameters with the
//! segments of curves in a given view
class IGESDraw_SegmentedViewsVisible : public IGESData_ViewKindEntity
{

public:
  Standard_EXPORT IGESDraw_SegmentedViewsVisible();

  //! This method is used to set the fields of the class
  //! SegmentedViewsVisible
  //! - allViews                : Pointers to View Entities
  //! - allBreakpointParameters : Parameters of breakpoints
  //! - allDisplayFlags         : Display flags
  //! - allColorValues          : Color Values
  //! - allColorDefinitions     : Color Definitions
  //! - allLineFontValues       : LineFont values
  //! - allLineFontDefinitions  : LineFont Definitions
  //! - allLineWeights          : Line weights
  //! raises exception if Lengths of allViews, allBreakpointParameters,
  //! allDisplayFlags, allColorValues, allColorDefinitions,
  //! allLineFontValues, allLineFontDefinitions and allLineWeights
  //! are not same.
  Standard_EXPORT void Init(
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>>& allViews,
    const occ::handle<NCollection_HArray1<double>>&                       allBreakpointParameters,
    const occ::handle<NCollection_HArray1<int>>&                          allDisplayFlags,
    const occ::handle<NCollection_HArray1<int>>&                          allColorValues,
    const occ::handle<NCollection_HArray1<occ::handle<IGESGraph_Color>>>& allColorDefinitions,
    const occ::handle<NCollection_HArray1<int>>&                          allLineFontValues,
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_LineFontEntity>>>&
                                                 allLineFontDefinitions,
    const occ::handle<NCollection_HArray1<int>>& allLineWeights);

  //! Returns False (for a complex view)
  Standard_EXPORT bool IsSingle() const override;

  //! Returns the count of Views referenced by <me> (inherited)
  Standard_EXPORT int NbViews() const override;

  //! returns the number of view/segment blocks in <me>
  //! Similar to NbViews but has a more general significance
  Standard_EXPORT int NbSegmentBlocks() const;

  //! returns the View entity indicated by ViewIndex
  //! raises an exception if ViewIndex <= 0 or
  //! ViewIndex > NbSegmentBlocks()
  Standard_EXPORT occ::handle<IGESData_ViewKindEntity> ViewItem(const int ViewIndex) const override;

  //! returns the parameter of the breakpoint indicated by
  //! BreakpointIndex
  //! raises an exception if BreakpointIndex <= 0 or
  //! BreakpointIndex > NbSegmentBlocks().
  Standard_EXPORT double BreakpointParameter(const int BreakpointIndex) const;

  //! returns the Display flag indicated by FlagIndex
  //! raises an exception if FlagIndex <= 0 or
  //! FlagIndex > NbSegmentBlocks().
  Standard_EXPORT int DisplayFlag(const int FlagIndex) const;

  //! returns True if the ColorIndex'th value of the
  //! "theColorDefinitions" field of <me> is a pointer
  //! raises an exception if ColorIndex <= 0 or
  //! ColorIndex > NbSegmentBlocks().
  Standard_EXPORT bool IsColorDefinition(const int ColorIndex) const;

  //! returns the Color value indicated by ColorIndex
  //! raises an exception if ColorIndex <= 0 or
  //! ColorIndex > NbSegmentBlocks().
  Standard_EXPORT int ColorValue(const int ColorIndex) const;

  //! returns the Color definition entity indicated by ColorIndex
  //! raises an exception if ColorIndex <= 0 or
  //! ColorIndex > NbSegmentBlocks().
  Standard_EXPORT occ::handle<IGESGraph_Color> ColorDefinition(const int ColorIndex) const;

  //! returns True if the FontIndex'th value of the
  //! "theLineFontDefinitions" field of <me> is a pointer
  //! raises an exception if FontIndex <= 0 or
  //! FontIndex > NbSegmentBlocks().
  Standard_EXPORT bool IsFontDefinition(const int FontIndex) const;

  //! returns the LineFont value indicated by FontIndex
  //! raises an exception if FontIndex <= 0 or
  //! FontIndex > NbSegmentBlocks().
  Standard_EXPORT int LineFontValue(const int FontIndex) const;

  //! returns the LineFont definition entity indicated by FontIndex
  //! raises an exception if FontIndex <= 0 or
  //! FontIndex > NbSegmentBlocks().
  Standard_EXPORT occ::handle<IGESData_LineFontEntity> LineFontDefinition(
    const int FontIndex) const;

  //! returns the LineWeight value indicated by WeightIndex
  //! raises an exception if WeightIndex <= 0 or
  //! WeightIndex > NbSegmentBlocks().
  Standard_EXPORT int LineWeightItem(const int WeightIndex) const;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_SegmentedViewsVisible, IGESData_ViewKindEntity)

private:
  occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> theViews;
  occ::handle<NCollection_HArray1<double>>                               theBreakpointParameters;
  occ::handle<NCollection_HArray1<int>>                                  theDisplayFlags;
  occ::handle<NCollection_HArray1<int>>                                  theColorValues;
  occ::handle<NCollection_HArray1<occ::handle<IGESGraph_Color>>>         theColorDefinitions;
  occ::handle<NCollection_HArray1<int>>                                  theLineFontValues;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_LineFontEntity>>> theLineFontDefinitions;
  occ::handle<NCollection_HArray1<int>>                                  theLineWeights;
};

#endif // _IGESDraw_SegmentedViewsVisible_HeaderFile
