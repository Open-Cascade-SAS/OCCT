// Created on: 1991-09-18
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _BRepPrim_GWedge_HeaderFile
#define _BRepPrim_GWedge_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepPrim_Builder.hxx>
#include <gp_Ax2.hxx>
#include <TopoDS_Shell.hxx>
#include <Standard_Boolean.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <BRepPrim_Direction.hxx>
class gp_Pln;
class gp_Lin;
class gp_Pnt;

//! A wedge is defined by:
//!
//! Axes: an Axis2 (coordinate system)
//!
//! YMin, YMax the coordinates of the ymin and ymax
//! rectangular faces parallel to the ZX plane (of the
//! coordinate systems)
//!
//! ZMin, ZMax, XMin, XMax the rectangular
//! left (YMin) face parallel to the Z and X axes.
//!
//! Z2Min, Z2Max, X2Min, X2Max the rectangular
//! right (YMax) face parallel to the Z and X axes.
//!
//! For a box Z2Min = ZMin, Z2Max = ZMax,
//! X2Min = XMin, X2Max = XMax
//!
//! The wedge can be open in the corresponding direction
//! of its Boolean myInfinite
class BRepPrim_GWedge
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor
  Standard_EXPORT BRepPrim_GWedge();

  //! Creates a GWedge algorithm. <Axes> is the axis
  //! system for the primitive.
  //!
  //! XMin, YMin, ZMin are set to 0
  //! XMax, YMax, ZMax are set to dx, dy, dz
  //! Z2Min = ZMin
  //! Z2Max = ZMax
  //! X2Min = XMin
  //! X2Max = XMax
  //! The result is a box
  //! dx,dy,dz should be positive
  Standard_EXPORT BRepPrim_GWedge(const BRepPrim_Builder& B,
                                  const gp_Ax2&           Axes,
                                  const double     dx,
                                  const double     dy,
                                  const double     dz);

  //! Creates a GWedge primitive. <Axes> is the axis
  //! system for the primitive.
  //!
  //! XMin, YMin, ZMin are set to 0
  //! XMax, YMax, ZMax are set to dx, dy, dz
  //! Z2Min = ZMin
  //! Z2Max = ZMax
  //! X2Min = ltx
  //! X2Max = ltx
  //! The result is a STEP right angular wedge
  //! dx,dy,dz should be positive
  //! ltx should not be negative
  Standard_EXPORT BRepPrim_GWedge(const BRepPrim_Builder& B,
                                  const gp_Ax2&           Axes,
                                  const double     dx,
                                  const double     dy,
                                  const double     dz,
                                  const double     ltx);

  //! Create a GWedge primitive. <Axes> is the axis
  //! system for the primitive.
  //!
  //! all the fields are set to the corresponding value
  //! XYZMax - XYZMin should be positive
  //! ZX2Max - ZX2Min should not be negative
  Standard_EXPORT BRepPrim_GWedge(const BRepPrim_Builder& B,
                                  const gp_Ax2&           Axes,
                                  const double     xmin,
                                  const double     ymin,
                                  const double     zmin,
                                  const double     z2min,
                                  const double     x2min,
                                  const double     xmax,
                                  const double     ymax,
                                  const double     zmax,
                                  const double     z2max,
                                  const double     x2max);

  //! Returns the coordinates system from <me>.
  Standard_EXPORT gp_Ax2 Axes() const;

  //! Returns Xmin value from <me>.
  Standard_EXPORT double GetXMin() const;

  //! Returns YMin value from <me>.
  Standard_EXPORT double GetYMin() const;

  //! Returns ZMin value from <me>.
  Standard_EXPORT double GetZMin() const;

  //! Returns Z2Min value from <me>.
  Standard_EXPORT double GetZ2Min() const;

  //! Returns X2Min value from <me>.
  Standard_EXPORT double GetX2Min() const;

  //! Returns XMax value from <me>.
  Standard_EXPORT double GetXMax() const;

  //! Returns YMax value from <me>.
  Standard_EXPORT double GetYMax() const;

  //! Returns ZMax value from <me>.
  Standard_EXPORT double GetZMax() const;

  //! Returns Z2Max value from <me>.
  Standard_EXPORT double GetZ2Max() const;

  //! Returns X2Max value from <me>.
  Standard_EXPORT double GetX2Max() const;

  //! Opens <me> in <d1> direction. A face and its edges
  //! or vertices are said nonexistent.
  Standard_EXPORT void Open(const BRepPrim_Direction d1);

  //! Closes <me> in <d1> direction. A face and its
  //! edges or vertices are said existent.
  Standard_EXPORT void Close(const BRepPrim_Direction d1);

  //! Returns True if <me> is open in <d1> direction.
  Standard_EXPORT bool IsInfinite(const BRepPrim_Direction d1) const;

  //! Returns the Shell containing the Faces of <me>.
  Standard_EXPORT const TopoDS_Shell& Shell();

  //! Returns True if <me> has a Face in <d1> direction.
  Standard_EXPORT bool HasFace(const BRepPrim_Direction d1) const;

  //! Returns the Face of <me> located in <d1> direction.
  Standard_EXPORT const TopoDS_Face& Face(const BRepPrim_Direction d1);

  //! Returns the plane of the Face of <me> located in
  //! <d1> direction.
  Standard_EXPORT gp_Pln Plane(const BRepPrim_Direction d1);

  //! Returns True if <me> has a Wire in <d1> direction.
  Standard_EXPORT bool HasWire(const BRepPrim_Direction d1) const;

  //! Returns the Wire of <me> located in <d1> direction.
  Standard_EXPORT const TopoDS_Wire& Wire(const BRepPrim_Direction d1);

  //! Returns True if <me> has an Edge in <d1><d2> direction.
  Standard_EXPORT bool HasEdge(const BRepPrim_Direction d1,
                                           const BRepPrim_Direction d2) const;

  //! Returns the Edge of <me> located in <d1><d2> direction.
  Standard_EXPORT const TopoDS_Edge& Edge(const BRepPrim_Direction d1, const BRepPrim_Direction d2);

  //! Returns the line of the Edge of <me> located in
  //! <d1><d2> direction.
  Standard_EXPORT gp_Lin Line(const BRepPrim_Direction d1, const BRepPrim_Direction d2);

  //! Returns True if <me> has a Vertex in <d1><d2><d3>
  //! direction.
  Standard_EXPORT bool HasVertex(const BRepPrim_Direction d1,
                                             const BRepPrim_Direction d2,
                                             const BRepPrim_Direction d3) const;

  //! Returns the Vertex of <me> located in <d1><d2><d3>
  //! direction.
  Standard_EXPORT const TopoDS_Vertex& Vertex(const BRepPrim_Direction d1,
                                              const BRepPrim_Direction d2,
                                              const BRepPrim_Direction d3);

  //! Returns the point of the Vertex of <me> located in
  //! <d1><d2><d3> direction.
  Standard_EXPORT gp_Pnt Point(const BRepPrim_Direction d1,
                               const BRepPrim_Direction d2,
                               const BRepPrim_Direction d3);

  //! Checks a shape on degeneracy
  //! @return TRUE if a shape is degenerated
  Standard_EXPORT bool IsDegeneratedShape();

private:
  BRepPrim_Builder myBuilder;
  gp_Ax2           myAxes;
  double    XMin;
  double    XMax;
  double    YMin;
  double    YMax;
  double    ZMin;
  double    ZMax;
  double    Z2Min;
  double    Z2Max;
  double    X2Min;
  double    X2Max;
  TopoDS_Shell     myShell;
  bool ShellBuilt;
  TopoDS_Vertex    myVertices[8];
  bool VerticesBuilt[8];
  TopoDS_Edge      myEdges[12];
  bool EdgesBuilt[12];
  TopoDS_Wire      myWires[6];
  bool WiresBuilt[6];
  TopoDS_Face      myFaces[6];
  bool FacesBuilt[6];
  bool myInfinite[6];
};

#endif // _BRepPrim_GWedge_HeaderFile
