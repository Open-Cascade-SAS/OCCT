// Created on: 1993-01-26
// Created by: Jean-Louis FRENKEL
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

#ifndef _StdPrs_ToolRFace_HeaderFile
#define _StdPrs_ToolRFace_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>

class TopoDS_Edge;

//! Iterator over 2D curves restricting a face (skipping internal/external edges).
//! In addition, the algorithm skips NULL curves - IsInvalidGeometry() can be checked if this should
//! be handled within algorithm.
class StdPrs_ToolRFace
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT StdPrs_ToolRFace();

  //! Constructor with initialization.
  Standard_EXPORT StdPrs_ToolRFace(const Handle(BRepAdaptor_Surface)& aSurface);

  //! Return TRUE indicating that iterator looks only for oriented edges.
  Standard_Boolean IsOriented() const { return Standard_True; }

  //! Move iterator to the first element.
  void Init()
  {
    myExplorer.Init(myFace, TopAbs_EDGE);
    next();
  }

  //! Return TRUE if iterator points to the curve.
  Standard_Boolean More() const { return myExplorer.More(); }

  //! Go to the next curve in the face.
  void Next()
  {
    myExplorer.Next();
    next();
  }

  //! Return current curve.
  const Adaptor2d_Curve2d& Value() const { return myCurve; }

  //! Return current edge.
  Standard_EXPORT const TopoDS_Edge& Edge() const;

  //! Return current edge orientation.
  TopAbs_Orientation Orientation() const { return myExplorer.Current().Orientation(); }

  //! Return TRUE if NULL curves have been skipped.
  Standard_Boolean IsInvalidGeometry() const { return myHasNullCurves; }

private:
  //! Find nearest valid item for the iterator.
  Standard_EXPORT void next();

private:
  TopoDS_Face         myFace;
  TopExp_Explorer     myExplorer;
  Geom2dAdaptor_Curve myCurve;
  Standard_Boolean    myHasNullCurves;
};

#endif // _StdPrs_ToolRFace_HeaderFile
