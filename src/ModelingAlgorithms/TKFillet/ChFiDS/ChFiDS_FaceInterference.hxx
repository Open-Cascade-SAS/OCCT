// Created on: 1993-11-16
// Created by: Laurent BOURESCHE
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

#ifndef _ChFiDS_FaceInterference_HeaderFile
#define _ChFiDS_FaceInterference_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Boolean.hxx>
class Geom2d_Curve;

//! interference face/fillet
class ChFiDS_FaceInterference
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT ChFiDS_FaceInterference();

  void SetInterference(const Standard_Integer      LineIndex,
                       const TopAbs_Orientation    Trans,
                       const Handle(Geom2d_Curve)& PCurv1,
                       const Handle(Geom2d_Curve)& PCurv2);

  Standard_EXPORT void SetTransition(const TopAbs_Orientation Trans);

  void SetFirstParameter(const Standard_Real U1);

  void SetLastParameter(const Standard_Real U1);

  Standard_EXPORT void SetParameter(const Standard_Real U1, const Standard_Boolean IsFirst);

  Standard_Integer LineIndex() const;

  void SetLineIndex(const Standard_Integer I);

  TopAbs_Orientation Transition() const;

  const Handle(Geom2d_Curve)& PCurveOnFace() const;

  const Handle(Geom2d_Curve)& PCurveOnSurf() const;

  Handle(Geom2d_Curve)& ChangePCurveOnFace();

  Handle(Geom2d_Curve)& ChangePCurveOnSurf();

  Standard_Real FirstParameter() const;

  Standard_Real LastParameter() const;

  Standard_EXPORT Standard_Real Parameter(const Standard_Boolean IsFirst) const;

protected:
private:
  Standard_Real        firstParam;
  Standard_Real        lastParam;
  Handle(Geom2d_Curve) pCurveOnFace;
  Handle(Geom2d_Curve) pCurveOnSurf;
  Standard_Integer     lineindex;
  TopAbs_Orientation   LineTransition;
};

#include <ChFiDS_FaceInterference.lxx>

#endif // _ChFiDS_FaceInterference_HeaderFile
