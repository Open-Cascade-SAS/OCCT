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

#include <Bnd_Box2d.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

class Geom2d_Curve;

//! This class is used to send the description of an
//! Edge to the classifier. It contains an Edge and a
//! Face. So the PCurve of the Edge can be found.
class BRepClass_Edge
{
public:
  DEFINE_STANDARD_ALLOC

  //! State of the cached pcurve bounding box.
  enum class BndBoxState
  {
    NotBuilt,
    Ready,
    Unavailable
  };

  Standard_EXPORT BRepClass_Edge();

  Standard_EXPORT BRepClass_Edge(const TopoDS_Edge& E, const TopoDS_Face& F);

  //! Returns the current edge.
  const TopoDS_Edge& Edge() const { return myEdge; }

  //! Returns the face for the current edge.
  const TopoDS_Face& Face() const { return myFace; }

  //! Sets the current edge and invalidates topology-derived data.
  //! @param[in] theEdge new edge
  Standard_EXPORT void SetEdge(const TopoDS_Edge& theEdge);

  //! Sets the face and invalidates topology-derived data.
  //! @param[in] theFace new face
  Standard_EXPORT void SetFace(const TopoDS_Face& theFace);

  //! Returns the next Edge
  const TopoDS_Edge& NextEdge() const { return myNextEdge; }

  //! Sets the next edge at the last vertex of the current edge.
  //! @param[in] theEdge next edge
  void SetNextEdge(const TopoDS_Edge& theEdge) { myNextEdge = theEdge; }

  //! Returns the maximum tolerance
  double MaxTolerance() const { return myMaxTolerance; }

  //! Sets the maximum tolerance at
  //! which to start checking in the intersector
  void SetMaxTolerance(const double theValue) { myMaxTolerance = theValue; }

  //! Returns true if we are using boxes
  //! in the intersector
  bool UseBndBox() const { return myUseBndBox; }

  //! Sets the status of whether we are
  //! using boxes or not
  void SetUseBndBox(const bool theValue) { myUseBndBox = theValue; }

  //! Sets cached 2D geometry and invalidates its bounding box.
  //! @param[in] theCurve pcurve on the associated face
  //! @param[in] theFirst first pcurve parameter
  //! @param[in] theLast last pcurve parameter
  Standard_EXPORT void SetGeometry(const occ::handle<Geom2d_Curve>& theCurve,
                                   double                           theFirst,
                                   double                           theLast);

  //! Sets a successfully computed pcurve bounding box. A void box marks the box unavailable.
  //! @param[in] theBox pcurve bounding box
  Standard_EXPORT void SetBoundingBox(const Bnd_Box2d& theBox);

  //! Marks the pcurve bounding box as unavailable after a failed build.
  Standard_EXPORT void SetBoundingBoxUnavailable();

  //! Returns cached pcurve, or null when it is unavailable.
  const occ::handle<Geom2d_Curve>& Curve() const { return myCurve; }

  //! Returns cached first pcurve parameter.
  double FirstParameter() const { return myFirstParameter; }

  //! Returns cached last pcurve parameter.
  double LastParameter() const { return myLastParameter; }

  //! Returns cached pcurve bounding box.
  const Bnd_Box2d& BoundingBox() const { return myBoundingBox; }

  //! Returns the state of the cached pcurve bounding box.
  BndBoxState BoundingBoxState() const { return myBoundingBoxState; }

private:
  void invalidateDerivedData();

private:
  TopoDS_Edge               myEdge;
  TopoDS_Face               myFace;
  TopoDS_Edge               myNextEdge;
  occ::handle<Geom2d_Curve> myCurve;
  Bnd_Box2d                 myBoundingBox;
  double                    myFirstParameter;
  double                    myLastParameter;
  double                    myMaxTolerance;
  BndBoxState               myBoundingBoxState;
  bool                      myUseBndBox;
};

#endif // _BRepClass_Edge_HeaderFile
