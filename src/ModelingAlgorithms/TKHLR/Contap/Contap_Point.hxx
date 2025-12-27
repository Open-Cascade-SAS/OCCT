// Created on: 1993-03-04
// Created by: Jacques GOUSSARD
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

#ifndef _Contap_Point_HeaderFile
#define _Contap_Point_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <IntSurf_Transition.hxx>

class Adaptor3d_HVertex;

//! Definition of a vertex on the contour line.
//! Most of the time, such a point is an intersection
//! between the contour and a restriction of the surface.
//! When it is not the method IsOnArc return False.
//! Such a point is contains geometrical information (see
//! the Value method) and logical information.
class Contap_Point
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT Contap_Point();

  //! Creates a point.
  Standard_EXPORT Contap_Point(const gp_Pnt& Pt, const double U, const double V);

  //! Sets the values for a point.
  void SetValue(const gp_Pnt& Pt, const double U, const double V);

  //! Set the value of the parameter on the intersection line.
  void SetParameter(const double Para);

  //! Sets the values of a point which is a vertex on
  //! the initial facet of restriction of one
  //! of the surface.
  void SetVertex(const occ::handle<Adaptor3d_HVertex>& V);

  //! Sets the value of the arc and of the parameter on
  //! this arc of the point.
  void SetArc(const occ::handle<Adaptor2d_Curve2d>& A,
              const double              Param,
              const IntSurf_Transition&        TLine,
              const IntSurf_Transition&        TArc);

  void SetMultiple();

  void SetInternal();

  //! Returns the intersection point (geometric information).
  const gp_Pnt& Value() const;

  //! This method returns the parameter of the point
  //! on the intersection line.
  //! If the points does not belong to an intersection line,
  //! the value returned does not have any sens.
  double ParameterOnLine() const;

  //! Returns the parameters on the surface of the point.
  void Parameters(double& U1, double& V1) const;

  //! Returns True when the point is an intersection between
  //! the contour and a restriction.
  bool IsOnArc() const;

  //! Returns the arc of restriction containing the
  //! vertex.
  const occ::handle<Adaptor2d_Curve2d>& Arc() const;

  //! Returns the parameter of the point on the
  //! arc returned by the method Arc().
  double ParameterOnArc() const;

  //! Returns the transition of the point on the contour.
  const IntSurf_Transition& TransitionOnLine() const;

  //! Returns the transition of the point on the arc.
  const IntSurf_Transition& TransitionOnArc() const;

  //! Returns TRUE if the point is a vertex on the initial
  //! restriction facet of the surface.
  bool IsVertex() const;

  //! Returns the information about the point when it is
  //! on the domain of the patch, i-e when the function
  //! IsVertex returns True.
  //! Otherwise, an exception is raised.
  const occ::handle<Adaptor3d_HVertex>& Vertex() const;

  //! Returns True if the point belongs to several
  //! lines.
  bool IsMultiple() const;

  //! Returns True if the point is an internal one, i.e
  //! if the tangent to the line on the point and the
  //! eye direction are parallel.
  bool IsInternal() const;

private:
  gp_Pnt                    pt;
  double             uparam;
  double             vparam;
  double             paraline;
  bool          onarc;
  occ::handle<Adaptor2d_Curve2d> arc;
  IntSurf_Transition        traline;
  IntSurf_Transition        traarc;
  double             prmarc;
  bool          isvtx;
  occ::handle<Adaptor3d_HVertex> vtx;
  bool          ismult;
  bool          myInternal;
};

#include <Contap_Point.lxx>

#endif // _Contap_Point_HeaderFile
