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

#ifndef _StepVisual_TessellatedConnectingEdge_HeaderFile_
#define _StepVisual_TessellatedConnectingEdge_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepVisual_TessellatedEdge.hxx>

#include <StepData_Logical.hxx>
#include <StepVisual_TessellatedFace.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//! Representation of STEP entity TessellatedConnectingEdge
class StepVisual_TessellatedConnectingEdge : public StepVisual_TessellatedEdge
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_TessellatedConnectingEdge();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&   theRepresentationItem_Name,
    const occ::handle<StepVisual_CoordinatesList>& theTessellatedEdge_Coordinates,
    const bool                                     theHasTessellatedEdge_GeometricLink,
    const StepVisual_EdgeOrCurve&                  theTessellatedEdge_GeometricLink,
    const occ::handle<NCollection_HArray1<int>>&   theTessellatedEdge_LineStrip,
    const StepData_Logical                         theSmooth,
    const occ::handle<StepVisual_TessellatedFace>& theFace1,
    const occ::handle<StepVisual_TessellatedFace>& theFace2,
    const occ::handle<NCollection_HArray1<int>>&   theLineStripFace1,
    const occ::handle<NCollection_HArray1<int>>&   theLineStripFace2);

  //! Returns field Smooth
  Standard_EXPORT StepData_Logical Smooth() const;

  //! Sets field Smooth
  Standard_EXPORT void SetSmooth(const StepData_Logical theSmooth);

  //! Returns field Face1
  Standard_EXPORT occ::handle<StepVisual_TessellatedFace> Face1() const;

  //! Sets field Face1
  Standard_EXPORT void SetFace1(const occ::handle<StepVisual_TessellatedFace>& theFace1);

  //! Returns field Face2
  Standard_EXPORT occ::handle<StepVisual_TessellatedFace> Face2() const;

  //! Sets field Face2
  Standard_EXPORT void SetFace2(const occ::handle<StepVisual_TessellatedFace>& theFace2);

  //! Returns field LineStripFace1
  Standard_EXPORT occ::handle<NCollection_HArray1<int>> LineStripFace1() const;

  //! Sets field LineStripFace1
  Standard_EXPORT void SetLineStripFace1(
    const occ::handle<NCollection_HArray1<int>>& theLineStripFace1);

  //! Returns number of LineStripFace1
  Standard_EXPORT int NbLineStripFace1() const;

  //! Returns value of LineStripFace1 by its num
  Standard_EXPORT int LineStripFace1Value(const int theNum) const;

  //! Returns field LineStripFace2
  Standard_EXPORT occ::handle<NCollection_HArray1<int>> LineStripFace2() const;

  //! Sets field LineStripFace2
  Standard_EXPORT void SetLineStripFace2(
    const occ::handle<NCollection_HArray1<int>>& theLineStripFace2);

  //! Returns number of LineStripFace2
  Standard_EXPORT int NbLineStripFace2() const;

  //! Returns value of LineStripFace2 by its num
  Standard_EXPORT int LineStripFace2Value(const int theNum) const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedConnectingEdge, StepVisual_TessellatedEdge)

private:
  StepData_Logical                        mySmooth;
  occ::handle<StepVisual_TessellatedFace> myFace1;
  occ::handle<StepVisual_TessellatedFace> myFace2;
  occ::handle<NCollection_HArray1<int>>   myLineStripFace1;
  occ::handle<NCollection_HArray1<int>>   myLineStripFace2;
};

#endif // _StepVisual_TessellatedConnectingEdge_HeaderFile_
