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
#include <TColStd_HArray1OfInteger.hxx>

DEFINE_STANDARD_HANDLE(StepVisual_TessellatedConnectingEdge, StepVisual_TessellatedEdge)

//! Representation of STEP entity TessellatedConnectingEdge
class StepVisual_TessellatedConnectingEdge : public StepVisual_TessellatedEdge
{

public :

  //! default constructor
  Standard_EXPORT StepVisual_TessellatedConnectingEdge();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                            const Handle(StepVisual_CoordinatesList)& theTessellatedEdge_Coordinates,
                            const Standard_Boolean theHasTessellatedEdge_GeometricLink,
                            const StepVisual_EdgeOrCurve& theTessellatedEdge_GeometricLink,
                            const Handle(TColStd_HArray1OfInteger)& theTessellatedEdge_LineStrip,
                            const StepData_Logical theSmooth,
                            const Handle(StepVisual_TessellatedFace)& theFace1,
                            const Handle(StepVisual_TessellatedFace)& theFace2,
                            const Handle(TColStd_HArray1OfInteger)& theLineStripFace1,
                            const Handle(TColStd_HArray1OfInteger)& theLineStripFace2);

  //! Returns field Smooth
  Standard_EXPORT StepData_Logical Smooth() const;

  //! Sets field Smooth
  Standard_EXPORT void SetSmooth (const StepData_Logical theSmooth);

  //! Returns field Face1
  Standard_EXPORT Handle(StepVisual_TessellatedFace) Face1() const;

  //! Sets field Face1
  Standard_EXPORT void SetFace1 (const Handle(StepVisual_TessellatedFace)& theFace1);

  //! Returns field Face2
  Standard_EXPORT Handle(StepVisual_TessellatedFace) Face2() const;

  //! Sets field Face2
  Standard_EXPORT void SetFace2 (const Handle(StepVisual_TessellatedFace)& theFace2);

  //! Returns field LineStripFace1
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) LineStripFace1() const;

  //! Sets field LineStripFace1
  Standard_EXPORT void SetLineStripFace1 (const Handle(TColStd_HArray1OfInteger)& theLineStripFace1);

  //! Returns number of LineStripFace1
  Standard_EXPORT Standard_Integer NbLineStripFace1() const;

  //! Returns value of LineStripFace1 by its num
  Standard_EXPORT Standard_Integer LineStripFace1Value(const Standard_Integer theNum) const;

  //! Returns field LineStripFace2
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) LineStripFace2() const;

  //! Sets field LineStripFace2
  Standard_EXPORT void SetLineStripFace2 (const Handle(TColStd_HArray1OfInteger)& theLineStripFace2);

  //! Returns number of LineStripFace2
  Standard_EXPORT Standard_Integer NbLineStripFace2() const;

  //! Returns value of LineStripFace2 by its num
  Standard_EXPORT Standard_Integer LineStripFace2Value(const Standard_Integer theNum) const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedConnectingEdge, StepVisual_TessellatedEdge)

private:

  StepData_Logical mySmooth;
  Handle(StepVisual_TessellatedFace) myFace1;
  Handle(StepVisual_TessellatedFace) myFace2;
  Handle(TColStd_HArray1OfInteger) myLineStripFace1;
  Handle(TColStd_HArray1OfInteger) myLineStripFace2;

};

#endif // _StepVisual_TessellatedConnectingEdge_HeaderFile_
