// Created on : Thu Mar 24 18:30:12 2022
// Created by: snn
// Generator: Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2022
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

#ifndef _StepVisual_TessellatedPointSet_HeaderFile_
#define _StepVisual_TessellatedPointSet_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepVisual_TessellatedItem.hxx>

#include <StepVisual_CoordinatesList.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//! Representation of STEP entity TessellatedPointSet
class StepVisual_TessellatedPointSet : public StepVisual_TessellatedItem
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_TessellatedPointSet();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&   theRepresentationItem_Name,
                            const occ::handle<StepVisual_CoordinatesList>& theCoordinates,
                            const occ::handle<NCollection_HArray1<int>>&   thePointList);

  //! Returns field Coordinates
  Standard_EXPORT occ::handle<StepVisual_CoordinatesList> Coordinates() const;

  //! Sets field Coordinates
  Standard_EXPORT void SetCoordinates(const occ::handle<StepVisual_CoordinatesList>& theCoordinates);

  //! Returns field PointList
  Standard_EXPORT occ::handle<NCollection_HArray1<int>> PointList() const;

  //! Sets field PointList
  Standard_EXPORT void SetPointList(const occ::handle<NCollection_HArray1<int>>& thePointList);

  //! Returns number of PointList
  Standard_EXPORT int NbPointList() const;

  //! Returns value of PointList by its num
  Standard_EXPORT int PointListValue(const int theNum) const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedPointSet, StepVisual_TessellatedItem)

private:
  occ::handle<StepVisual_CoordinatesList> myCoordinates;
  occ::handle<NCollection_HArray1<int>>   myPointList;
};

#endif // _StepVisual_TessellatedPointSet_HeaderFile_
