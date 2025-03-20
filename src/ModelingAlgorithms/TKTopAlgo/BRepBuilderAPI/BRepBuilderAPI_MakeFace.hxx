// Created on: 1993-07-12
// Created by: Remi LEQUETTE
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

#ifndef _BRepBuilderAPI_MakeFace_HeaderFile
#define _BRepBuilderAPI_MakeFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepLib_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <Standard_Real.hxx>
#include <BRepBuilderAPI_FaceError.hxx>
class TopoDS_Face;
class gp_Pln;
class gp_Cylinder;
class gp_Cone;
class gp_Sphere;
class gp_Torus;
class Geom_Surface;
class TopoDS_Wire;

//! Provides methods to build faces.
//!
//! A face may be built :
//!
//! * From a surface.
//!
//! - Elementary surface from gp.
//!
//! - Surface from Geom.
//!
//! * From a surface and U,V values.
//!
//! * From a wire.
//!
//! - Find the surface automatically if possible.
//!
//! * From a surface and a wire.
//!
//! - A flag Inside is given, when this flag is True
//! the  wire is  oriented to bound a finite area on
//! the surface.
//!
//! * From a face and a wire.
//!
//! - The new wire is a perforation.
class BRepBuilderAPI_MakeFace : public BRepBuilderAPI_MakeShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Not done.
  Standard_EXPORT BRepBuilderAPI_MakeFace();

  //! Load a face. useful to add wires.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const TopoDS_Face& F);

  //! Make a face from a plane.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Pln& P);

  //! Make a face from a cylinder.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Cylinder& C);

  //! Make a face from a cone.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Cone& C);

  //! Make a face from a sphere.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Sphere& S);

  //! Make a face from a torus.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Torus& C);

  //! Make a face from a Surface. Accepts tolerance value (TolDegen)
  //! for resolution of degenerated edges.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const Handle(Geom_Surface)& S,
                                          const Standard_Real         TolDegen);

  //! Make a face from a plane.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Pln&       P,
                                          const Standard_Real UMin,
                                          const Standard_Real UMax,
                                          const Standard_Real VMin,
                                          const Standard_Real VMax);

  //! Make a face from a cylinder.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Cylinder&  C,
                                          const Standard_Real UMin,
                                          const Standard_Real UMax,
                                          const Standard_Real VMin,
                                          const Standard_Real VMax);

  //! Make a face from a cone.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Cone&      C,
                                          const Standard_Real UMin,
                                          const Standard_Real UMax,
                                          const Standard_Real VMin,
                                          const Standard_Real VMax);

  //! Make a face from a sphere.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Sphere&    S,
                                          const Standard_Real UMin,
                                          const Standard_Real UMax,
                                          const Standard_Real VMin,
                                          const Standard_Real VMax);

  //! Make a face from a torus.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Torus&     C,
                                          const Standard_Real UMin,
                                          const Standard_Real UMax,
                                          const Standard_Real VMin,
                                          const Standard_Real VMax);

  //! Make a face from a Surface. Accepts tolerance value (TolDegen)
  //! for resolution of degenerated edges.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const Handle(Geom_Surface)& S,
                                          const Standard_Real         UMin,
                                          const Standard_Real         UMax,
                                          const Standard_Real         VMin,
                                          const Standard_Real         VMax,
                                          const Standard_Real         TolDegen);

  //! Find a surface from the wire and make a face.
  //! if <OnlyPlane> is true, the computed surface will be
  //! a plane. If it is not possible to find a plane, the
  //! flag NotDone will be set.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const TopoDS_Wire&     W,
                                          const Standard_Boolean OnlyPlane = Standard_False);

  //! Make a face from a plane and a wire.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Pln&          P,
                                          const TopoDS_Wire&     W,
                                          const Standard_Boolean Inside = Standard_True);

  //! Make a face from a cylinder and a wire.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Cylinder&     C,
                                          const TopoDS_Wire&     W,
                                          const Standard_Boolean Inside = Standard_True);

  //! Make a face from a cone and a wire.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Cone&         C,
                                          const TopoDS_Wire&     W,
                                          const Standard_Boolean Inside = Standard_True);

  //! Make a face from a sphere and a wire.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Sphere&       S,
                                          const TopoDS_Wire&     W,
                                          const Standard_Boolean Inside = Standard_True);

  //! Make a face from a torus and a wire.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const gp_Torus&        C,
                                          const TopoDS_Wire&     W,
                                          const Standard_Boolean Inside = Standard_True);

  //! Make a face from a Surface and a wire.
  //! If the surface S is not plane,
  //! it must contain pcurves for all edges in W,
  //! otherwise the wrong shape will be created.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const Handle(Geom_Surface)& S,
                                          const TopoDS_Wire&          W,
                                          const Standard_Boolean      Inside = Standard_True);

  //! Adds the wire <W> in the face <F>
  //! A general method to create a face is to give
  //! -      a surface S as the support (the geometric domain) of the face,
  //! -      and a wire W to bound it.
  //! The bounds of the face can also be defined by four parameter values
  //! umin, umax, vmin, vmax which determine isoparametric limitations on
  //! the parametric space of the surface. In this way, a patch is
  //! defined. The parameter values are optional. If they are omitted, the
  //! natural bounds of the surface are used. A wire is automatically
  //! built using the defined bounds. Up to four edges and four vertices
  //! are created with this wire (no edge is created when the
  //! corresponding parameter value is infinite).
  //! Wires can then be added using the function Add to define other
  //! restrictions on the face. These restrictions represent holes. More
  //! than one wire may be added by this way, provided that the wires do
  //! not cross each other and that they define only one area on the
  //! surface. (Be careful, however, as this is not checked).
  //! Forbidden addition of wires
  //! Note that in this schema, the third case is valid if edges of the
  //! wire W are declared internal to the face. As a result, these edges
  //! are no longer bounds of the face.
  //! A default tolerance (Precision::Confusion()) is given to the face,
  //! this tolerance may be increased during construction of the face
  //! using various algorithms.
  //! Rules applied to the arguments
  //! For the surface:
  //! -      The surface must not be a 'null handle'.
  //! -      If the surface is a trimmed surface, the basis surface is used.
  //! -      For the wire: the wire is composed of connected edges, each
  //! edge having a parametric curve description in the parametric
  //! domain of the surface; in other words, as a pcurve.
  //! For the parameters:
  //! -      The parameter values must be in the parametric range of the
  //! surface (or the basis surface, if the surface is trimmed). If this
  //! condition is not satisfied, the face is not built, and the Error
  //! function will return BRepBuilderAPI_ParametersOutOfRange.
  //! -      The bounding parameters p1 and p2 are adjusted on a periodic
  //! surface in a given parametric direction by adding or subtracting
  //! the period to obtain p1 in the parametric range of the surface and
  //! such p2, that p2 - p1 <= Period, where Period is the period of the
  //! surface in this parametric direction.
  //! -      A parameter value may be infinite. There will be no edge and
  //! no vertex in the corresponding direction.
  Standard_EXPORT BRepBuilderAPI_MakeFace(const TopoDS_Face& F, const TopoDS_Wire& W);

  //! Initializes (or reinitializes) the
  //! construction of a face by creating a new object which is a copy of
  //! the face F, in order to add wires to it, using the function Add.
  //! Note: this complete copy of the geometry is only required if you
  //! want to work on the geometries of the two faces independently.
  Standard_EXPORT void Init(const TopoDS_Face& F);

  //! Initializes (or reinitializes) the construction of a face on
  //! the surface S. If Bound is true, a wire is
  //! automatically created from the natural bounds of the
  //! surface S and added to the face in order to bound it. If
  //! Bound is false, no wire is added. This option is used
  //! when real bounds are known. These will be added to
  //! the face after this initialization, using the function Add.
  //! TolDegen parameter is used for resolution of degenerated edges
  //! if calculation of natural bounds is turned on.
  Standard_EXPORT void Init(const Handle(Geom_Surface)& S,
                            const Standard_Boolean      Bound,
                            const Standard_Real         TolDegen);

  //! Initializes (or reinitializes) the construction of a face on
  //! the surface S, limited in the u parametric direction by
  //! the two parameter values UMin and UMax and in the
  //! v parametric direction by the two parameter values VMin and VMax.
  //! Warning
  //! Error returns:
  //! -      BRepBuilderAPI_ParametersOutOfRange
  //! when the parameters given are outside the bounds of the
  //! surface or the basis surface of a trimmed surface.
  //! TolDegen parameter is used for resolution of degenerated edges.
  Standard_EXPORT void Init(const Handle(Geom_Surface)& S,
                            const Standard_Real         UMin,
                            const Standard_Real         UMax,
                            const Standard_Real         VMin,
                            const Standard_Real         VMax,
                            const Standard_Real         TolDegen);

  //! Adds the wire W to the constructed face as a hole.
  //! Warning
  //! W must not cross the other bounds of the face, and all
  //! the bounds must define only one area on the surface.
  //! (Be careful, however, as this is not checked.)
  //! Example
  //! // a cylinder
  //! gp_Cylinder C = ..;
  //! // a wire
  //! TopoDS_Wire W = ...;
  //! BRepBuilderAPI_MakeFace MF(C);
  //! MF.Add(W);
  //! TopoDS_Face F = MF;
  Standard_EXPORT void Add(const TopoDS_Wire& W);

  //! Returns true if this algorithm has a valid face.
  Standard_EXPORT virtual Standard_Boolean IsDone() const Standard_OVERRIDE;

  //! Returns the construction status
  //! BRepBuilderAPI_FaceDone if the face is built, or
  //! -   another value of the BRepBuilderAPI_FaceError
  //! enumeration indicating why the construction failed, in
  //! particular when the given parameters are outside the
  //! bounds of the surface.
  Standard_EXPORT BRepBuilderAPI_FaceError Error() const;

  //! Returns the constructed face.
  //! Exceptions
  //! StdFail_NotDone if no face is built.
  Standard_EXPORT const TopoDS_Face& Face() const;
  Standard_EXPORT                    operator TopoDS_Face() const;

protected:
private:
  BRepLib_MakeFace myMakeFace;
};

#endif // _BRepBuilderAPI_MakeFace_HeaderFile
