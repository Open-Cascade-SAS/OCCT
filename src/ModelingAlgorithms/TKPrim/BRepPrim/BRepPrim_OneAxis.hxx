// Created on: 1991-07-23
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

#ifndef _BRepPrim_OneAxis_HeaderFile
#define _BRepPrim_OneAxis_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <BRepPrim_Builder.hxx>
#include <gp_Ax2.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
class gp_Pnt2d;

//! Algorithm to  build  primitives with  one  axis of
//! revolution.
//!
//! The revolution  body is described by :
//!
//! A coordinate  system (Ax2 from  gp). The Z axis is
//! the rotational axis.
//!
//! An Angle around the Axis, When  the Angle  is 2*PI
//! the primitive is not limited  by planar faces. The
//! U parameter range from 0 to Angle.
//!
//! A parameter range VMin, VMax on the meridian.
//!
//! A meridian : The  meridian is a curve described by
//! a set of deferred methods.
//!
//! The  topology consists of  A shell,  Faces,  Wires,
//! Edges and Vertices.  Methods  are provided to build
//! all the elements.  Building an element  implies the
//! automatic building  of  all its  sub-elements.
//!
//! So building the shell builds everything.
//!
//! There are at most 5 faces :
//!
//! - The LateralFace.
//!
//! - The TopFace and the BottomFace.
//!
//! - The StartFace and the EndFace.
class BRepPrim_OneAxis
{
public:
  DEFINE_STANDARD_ALLOC

  //! The MeridianOffset is added  to the  parameters on
  //! the meridian curve and  to  the  V values  of  the
  //! pcurves. This is  used for the sphere for example,
  //! to give a range on the meridian  edge which is not
  //! VMin, VMax.
  Standard_EXPORT void SetMeridianOffset(const Standard_Real MeridianOffset = 0);

  //! Returns the Ax2 from <me>.
  Standard_EXPORT const gp_Ax2& Axes() const;

  Standard_EXPORT void Axes(const gp_Ax2& A);

  Standard_EXPORT Standard_Real Angle() const;

  Standard_EXPORT void Angle(const Standard_Real A);

  Standard_EXPORT Standard_Real VMin() const;

  Standard_EXPORT void VMin(const Standard_Real V);

  Standard_EXPORT Standard_Real VMax() const;

  Standard_EXPORT void VMax(const Standard_Real V);

  //! Returns a face with  no edges.  The surface is the
  //! lateral surface with normals pointing outward. The
  //! U parameter is the angle with the  origin on the X
  //! axis. The  V parameter is   the  parameter of  the
  //! meridian.
  Standard_EXPORT virtual TopoDS_Face MakeEmptyLateralFace() const = 0;

  //! Returns  an  edge with  a 3D curve   made from the
  //! meridian  in the XZ  plane rotated by <Ang> around
  //! the Z-axis. Ang may be 0 or myAngle.
  Standard_EXPORT virtual TopoDS_Edge MakeEmptyMeridianEdge(const Standard_Real Ang) const = 0;

  //! Sets the  parametric curve of the  edge <E> in the
  //! face  <F> to be  the   2d representation  of   the
  //! meridian.
  Standard_EXPORT virtual void SetMeridianPCurve(TopoDS_Edge& E, const TopoDS_Face& F) const = 0;

  //! Returns the meridian point at parameter <V> in the
  //! plane XZ.
  Standard_EXPORT virtual gp_Pnt2d MeridianValue(const Standard_Real V) const = 0;

  //! Returns True if the point of  parameter <V> on the
  //! meridian is on the Axis. Default implementation is
  //! Abs(MeridianValue(V).X()) < Precision::Confusion()
  Standard_EXPORT virtual Standard_Boolean MeridianOnAxis(const Standard_Real V) const;

  //! Returns True  if the  meridian is  closed. Default
  //! implementation                                  is
  //! MeridianValue(VMin).IsEqual(MeridianValue(VMax),
  //! Precision::Confusion())
  Standard_EXPORT virtual Standard_Boolean MeridianClosed() const;

  //! Returns  True   if  VMax    is  infinite.  Default
  //! Precision::IsPositiveInfinite(VMax);
  Standard_EXPORT virtual Standard_Boolean VMaxInfinite() const;

  //! Returns  True   if  VMin    is  infinite.  Default
  //! Precision::IsNegativeInfinite(VMax);
  Standard_EXPORT virtual Standard_Boolean VMinInfinite() const;

  //! Returns True if  there is  a top  face.
  //!
  //! That is neither : VMaxInfinite()
  //! MeridianClosed()
  //! MeridianOnAxis(VMax)
  Standard_EXPORT virtual Standard_Boolean HasTop() const;

  //! Returns   True if there is   a bottom  face.
  //!
  //! That is neither : VMinInfinite()
  //! MeridianClosed()
  //! MeridianOnAxis(VMin)
  Standard_EXPORT virtual Standard_Boolean HasBottom() const;

  //! Returns True if  there are Start   and  End faces.
  //!
  //! That is : 2*PI  - Angle > Precision::Angular()
  Standard_EXPORT virtual Standard_Boolean HasSides() const;

  //! Returns the Shell containing all the  Faces of the
  //! primitive.
  Standard_EXPORT const TopoDS_Shell& Shell();

  //! Returns  the lateral Face.   It is oriented toward
  //! the outside of the primitive.
  Standard_EXPORT const TopoDS_Face& LateralFace();

  //! Returns the   top planar  Face.    It  is Oriented
  //! toward the +Z axis (outside).
  Standard_EXPORT const TopoDS_Face& TopFace();

  //! Returns  the Bottom planar Face.   It is  Oriented
  //! toward the -Z axis (outside).
  Standard_EXPORT const TopoDS_Face& BottomFace();

  //! Returns  the  Face   starting   the slice, it   is
  //! oriented toward the exterior of the primitive.
  Standard_EXPORT const TopoDS_Face& StartFace();

  //! Returns the Face ending the slice, it  is oriented
  //! toward the exterior of the primitive.
  Standard_EXPORT const TopoDS_Face& EndFace();

  //! Returns  the wire in the lateral face.
  Standard_EXPORT const TopoDS_Wire& LateralWire();

  //! Returns the   wire in the   lateral  face with the
  //! start edge.
  Standard_EXPORT const TopoDS_Wire& LateralStartWire();

  //! Returns the wire with in lateral face with the end
  //! edge.
  Standard_EXPORT const TopoDS_Wire& LateralEndWire();

  //! Returns the wire in the top face.
  Standard_EXPORT const TopoDS_Wire& TopWire();

  //! Returns the wire in the bottom face.
  Standard_EXPORT const TopoDS_Wire& BottomWire();

  //! Returns the wire  in the  start face.
  Standard_EXPORT const TopoDS_Wire& StartWire();

  //! Returns  the wire   in the  start   face  with the
  //! AxisEdge.
  Standard_EXPORT const TopoDS_Wire& AxisStartWire();

  //! Returns the Wire in   the end face.
  Standard_EXPORT const TopoDS_Wire& EndWire();

  //! Returns  the Wire  in  the   end   face  with  the
  //! AxisEdge.
  Standard_EXPORT const TopoDS_Wire& AxisEndWire();

  //! Returns the Edge built along the Axis and oriented
  //! on +Z of the Axis.
  Standard_EXPORT const TopoDS_Edge& AxisEdge();

  //! Returns the   Edge at angle 0.
  Standard_EXPORT const TopoDS_Edge& StartEdge();

  //! Returns the  Edge at  angle Angle.  If !HasSides()
  //! the StartEdge and the EndEdge are the same edge.
  Standard_EXPORT const TopoDS_Edge& EndEdge();

  //! Returns the linear Edge between start Face and top
  //! Face.
  Standard_EXPORT const TopoDS_Edge& StartTopEdge();

  //! Returns the linear  Edge between  start  Face  and
  //! bottom Face.
  Standard_EXPORT const TopoDS_Edge& StartBottomEdge();

  //! Returns the linear Edge  between end Face and  top
  //! Face.
  Standard_EXPORT const TopoDS_Edge& EndTopEdge();

  //! Returns  the  linear  Edge  between end  Face  and
  //! bottom Face.
  Standard_EXPORT const TopoDS_Edge& EndBottomEdge();

  //! Returns the edge at VMax. If  MeridianClosed() the
  //! TopEdge and the BottomEdge are the same edge.
  Standard_EXPORT const TopoDS_Edge& TopEdge();

  //! Returns the edge  at VMin. If MeridianClosed() the
  //! TopEdge and the BottomEdge are the same edge.
  Standard_EXPORT const TopoDS_Edge& BottomEdge();

  //! Returns the Vertex at the Top altitude on the axis.
  Standard_EXPORT const TopoDS_Vertex& AxisTopVertex();

  //! Returns the Vertex  at the Bottom  altitude on the
  //! axis.
  Standard_EXPORT const TopoDS_Vertex& AxisBottomVertex();

  //! Returns the vertex (0,VMax)
  Standard_EXPORT const TopoDS_Vertex& TopStartVertex();

  //! Returns the vertex (angle,VMax)
  Standard_EXPORT const TopoDS_Vertex& TopEndVertex();

  //! Returns the vertex (0,VMin)
  Standard_EXPORT const TopoDS_Vertex& BottomStartVertex();

  //! Returns the vertex (angle,VMax)
  Standard_EXPORT const TopoDS_Vertex& BottomEndVertex();
  Standard_EXPORT virtual ~BRepPrim_OneAxis();

protected:
  //! Creates a OneAxis algorithm.  <B> is used to build
  //! the Topology. The angle defaults to 2*PI.
  Standard_EXPORT BRepPrim_OneAxis(const BRepPrim_Builder& B,
                                   const gp_Ax2&           A,
                                   const Standard_Real     VMin,
                                   const Standard_Real     VMax);

  BRepPrim_Builder myBuilder;

private:
  gp_Ax2           myAxes;
  Standard_Real    myAngle;
  Standard_Real    myVMin;
  Standard_Real    myVMax;
  Standard_Real    myMeridianOffset;
  TopoDS_Shell     myShell;
  Standard_Boolean ShellBuilt;
  TopoDS_Vertex    myVertices[6];
  Standard_Boolean VerticesBuilt[6];
  TopoDS_Edge      myEdges[9];
  Standard_Boolean EdgesBuilt[9];
  TopoDS_Wire      myWires[9];
  Standard_Boolean WiresBuilt[9];
  TopoDS_Face      myFaces[5];
  Standard_Boolean FacesBuilt[5];
};

#endif // _BRepPrim_OneAxis_HeaderFile
