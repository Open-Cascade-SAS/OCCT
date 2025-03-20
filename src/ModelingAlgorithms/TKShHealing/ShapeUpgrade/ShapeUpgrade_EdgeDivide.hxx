// Created on: 2000-05-24
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _ShapeUpgrade_EdgeDivide_HeaderFile
#define _ShapeUpgrade_EdgeDivide_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Face.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <ShapeUpgrade_Tool.hxx>
class ShapeUpgrade_SplitCurve3d;
class ShapeUpgrade_SplitCurve2d;
class TopoDS_Edge;

class ShapeUpgrade_EdgeDivide;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_EdgeDivide, ShapeUpgrade_Tool)

class ShapeUpgrade_EdgeDivide : public ShapeUpgrade_Tool
{

public:
  //! Empty constructor
  Standard_EXPORT ShapeUpgrade_EdgeDivide();

  Standard_EXPORT void Clear();

  //! Sets supporting surface by face
  void SetFace(const TopoDS_Face& F);

  Standard_EXPORT virtual Standard_Boolean Compute(const TopoDS_Edge& E);

  Standard_Boolean HasCurve2d() const;

  Standard_Boolean HasCurve3d() const;

  Handle(TColStd_HSequenceOfReal) Knots2d() const;

  Handle(TColStd_HSequenceOfReal) Knots3d() const;

  //! Sets the tool for splitting pcurves.
  Standard_EXPORT void SetSplitCurve2dTool(
    const Handle(ShapeUpgrade_SplitCurve2d)& splitCurve2dTool);

  //! Sets the tool for splitting 3D curves.
  Standard_EXPORT void SetSplitCurve3dTool(
    const Handle(ShapeUpgrade_SplitCurve3d)& splitCurve3dTool);

  //! Returns the tool for splitting pcurves.
  Standard_EXPORT virtual Handle(ShapeUpgrade_SplitCurve2d) GetSplitCurve2dTool() const;

  //! Returns the tool for splitting 3D curves.
  Standard_EXPORT virtual Handle(ShapeUpgrade_SplitCurve3d) GetSplitCurve3dTool() const;

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_EdgeDivide, ShapeUpgrade_Tool)

protected:
  TopoDS_Face                     myFace;
  Standard_Boolean                myHasCurve2d;
  Standard_Boolean                myHasCurve3d;
  Handle(TColStd_HSequenceOfReal) myKnots2d;
  Handle(TColStd_HSequenceOfReal) myKnots3d;

private:
  Handle(ShapeUpgrade_SplitCurve3d) mySplitCurve3dTool;
  Handle(ShapeUpgrade_SplitCurve2d) mySplitCurve2dTool;
};

#include <ShapeUpgrade_EdgeDivide.lxx>

#endif // _ShapeUpgrade_EdgeDivide_HeaderFile
