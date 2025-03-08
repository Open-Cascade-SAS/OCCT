// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRepClass_Edge_HeaderFile
#define _BRepClass_Edge_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//! This class  is used to send  the  description of an
//! Edge to the classifier. It  contains  an Edge and a
//! Face. So the PCurve of the Edge can be found.
class BRepClass_Edge
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepClass_Edge();

  Standard_EXPORT BRepClass_Edge(const TopoDS_Edge& E, const TopoDS_Face& F);

  //! Returns the current Edge
  TopoDS_Edge&       Edge();
  const TopoDS_Edge& Edge() const;

  //! Returns the Face for the current Edge
  TopoDS_Face&       Face();
  const TopoDS_Face& Face() const;

  //! Returns the next Edge
  const TopoDS_Edge& NextEdge() const { return myNextEdge; }

  //! Finds and sets the next Edge for the current
  Standard_EXPORT void SetNextEdge(const TopTools_IndexedDataMapOfShapeListOfShape& theMapVE);

  //! Returns the maximum tolerance
  Standard_Real MaxTolerance() const { return myMaxTolerance; }

  //! Sets the maximum tolerance at
  //! which to start checking in the intersector
  void SetMaxTolerance(const Standard_Real theValue) { myMaxTolerance = theValue; }

  //! Returns true if we are using boxes
  //! in the intersector
  Standard_Boolean UseBndBox() const { return myUseBndBox; }

  //! Sets the status of whether we are
  //! using boxes or not
  void SetUseBndBox(const Standard_Boolean theValue) { myUseBndBox = theValue; }

protected:
private:
  TopoDS_Edge      myEdge;
  TopoDS_Face      myFace;
  TopoDS_Edge      myNextEdge;
  Standard_Real    myMaxTolerance;
  Standard_Boolean myUseBndBox;
};

#include <BRepClass_Edge.lxx>

#endif // _BRepClass_Edge_HeaderFile
