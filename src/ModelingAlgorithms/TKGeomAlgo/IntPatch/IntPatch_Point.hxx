// Created on: 1992-05-06
// Created by: Jacques GOUSSARD
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

#ifndef _IntPatch_Point_HeaderFile
#define _IntPatch_Point_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntSurf_PntOn2S.hxx>
#include <IntSurf_Transition.hxx>
#include <Adaptor2d_Curve2d.hxx>

class Adaptor3d_HVertex;
class gp_Pnt;

//! Definition of an intersection point between two surfaces.
//! Such a point is contains geometrical information (see
//! the Value method) and logical information.
class IntPatch_Point
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  IntPatch_Point();

  //! Sets the values of a point which is on no domain,
  //! when both surfaces are implicit ones.
  //! If Tangent is True, the point is a point of tangency
  //! between the surfaces.
  Standard_EXPORT void SetValue(const gp_Pnt& Pt, const double Tol, const bool Tangent);

  void SetValue(const gp_Pnt& Pt);

  //! Sets the value of <pt> member
  void SetValue(const IntSurf_PntOn2S& thePOn2S);

  void SetTolerance(const double Tol);

  //! Sets the values of the parameters of the point
  //! on each surface.
  void SetParameters(const double U1, const double V1, const double U2, const double V2);

  //! Set the value of the parameter on the intersection line.
  void SetParameter(const double Para);

  //! Sets the values of a point which is a vertex on
  //! the initial facet of restriction of one
  //! of the surface.
  //! If OnFirst is True, the point is on the domain of the
  //! first patch, otherwise the point is on the domain of the
  //! second surface.
  Standard_EXPORT void SetVertex(const bool OnFirst, const occ::handle<Adaptor3d_HVertex>& V);

  //! Sets the values of a point which is on one of the domain,
  //! when both surfaces are implicit ones.
  //! If OnFirst is True, the point is on the domain of the
  //! first patch, otherwise the point is on the domain of the
  //! second surface.
  Standard_EXPORT void SetArc(const bool                            OnFirst,
                              const occ::handle<Adaptor2d_Curve2d>& A,
                              const double                          Param,
                              const IntSurf_Transition&             TLine,
                              const IntSurf_Transition&             TArc);

  //! Sets (or unsets) the point as a point on several
  //! intersection line.
  void SetMultiple(const bool IsMult);

  //! Returns the intersection point (geometric information).
  const gp_Pnt& Value() const;

  //! This method returns the parameter of the point
  //! on the intersection line.
  //! If the points does not belong to an intersection line,
  //! the value returned does not have any sens.
  double ParameterOnLine() const;

  //! This method returns the fuzziness on the point.
  double Tolerance() const;

  //! Returns True if the Point is a tangency point between
  //! the surfaces.
  //! If the Point is on one of the domain (IsOnDomS1 returns
  //! True or IsOnDomS2 returns True), an exception is raised.
  bool IsTangencyPoint() const;

  //! Returns the parameters on the first surface of the point.
  void ParametersOnS1(double& U1, double& V1) const;

  //! Returns the parameters on the second surface of the point.
  void ParametersOnS2(double& U2, double& V2) const;

  //! Returns True if the point belongs to several intersection
  //! lines.
  bool IsMultiple() const;

  //! Returns TRUE if the point is on a boundary of the domain
  //! of the first patch.
  bool IsOnDomS1() const;

  //! Returns TRUE if the point is a vertex on the initial
  //! restriction facet of the first surface.
  bool IsVertexOnS1() const;

  //! Returns the information about the point when it is
  //! on the domain of the first patch, i-e when the function
  //! IsVertexOnS1 returns True.
  //! Otherwise, an exception is raised.
  const occ::handle<Adaptor3d_HVertex>& VertexOnS1() const;

  //! Returns the arc of restriction containing the
  //! vertex.
  //! The exception DomainError is raised if
  //! IsOnDomS1 returns False.
  const occ::handle<Adaptor2d_Curve2d>& ArcOnS1() const;

  //! Returns the transition of the point on the
  //! intersection line with the arc on S1.
  //! The exception DomainError is raised if IsOnDomS1
  //! returns False.
  const IntSurf_Transition& TransitionLineArc1() const;

  //! Returns the transition between the intersection line
  //! returned by the method Line and the arc on S1 returned
  //! by ArcOnS1().
  //! The exception DomainError is raised if
  //! IsOnDomS1 returns False.
  const IntSurf_Transition& TransitionOnS1() const;

  //! Returns the parameter of the point on the
  //! arc returned by the method ArcOnS2.
  //! The exception DomainError is raised if
  //! IsOnDomS1 returns False.
  double ParameterOnArc1() const;

  //! Returns TRUE if the point is on a boundary of the domain
  //! of the second patch.
  bool IsOnDomS2() const;

  //! Returns TRUE if the point is a vertex on the initial
  //! restriction facet of the first surface.
  bool IsVertexOnS2() const;

  //! Returns the information about the point when it is
  //! on the domain of the second patch, i-e when the function
  //! IsVertexOnS2 returns True.
  //! Otherwise, an exception is raised.
  const occ::handle<Adaptor3d_HVertex>& VertexOnS2() const;

  //! Returns the arc of restriction containing the
  //! vertex.
  //! The exception DomainError is raised if
  //! IsOnDomS2 returns False.
  const occ::handle<Adaptor2d_Curve2d>& ArcOnS2() const;

  //! Returns the transition of the point on the
  //! intersection line with the arc on S2.
  //! The exception DomainError is raised if IsOnDomS2
  //! returns False.
  const IntSurf_Transition& TransitionLineArc2() const;

  //! Returns the transition between the intersection line
  //! returned by the method Line and the arc on S2 returned
  //! by ArcOnS2.
  //! The exception DomainError is raised if
  //! IsOnDomS2 returns False.
  const IntSurf_Transition& TransitionOnS2() const;

  //! Returns the parameter of the point on the
  //! arc returned by the method ArcOnS2.
  //! The exception DomainError is raised if
  //! IsOnDomS2 returns False.
  double ParameterOnArc2() const;

  //! Returns the PntOn2S
  //! (geometric Point and the parameters)
  const IntSurf_PntOn2S& PntOn2S() const;

  //! Returns the parameters on the first and on the
  //! second surface of the point.
  void Parameters(double& U1, double& V1, double& U2, double& V2) const;

  Standard_EXPORT void ReverseTransition();

  Standard_EXPORT void Dump() const;

private:
  IntSurf_PntOn2S pt;
  double          para;
  double          tol;
  bool            tgt;

  // TRUE if the point is in several intersection lines
  bool mult;

  bool                           onS1;
  bool                           vtxonS1;
  occ::handle<Adaptor3d_HVertex> vS1;
  occ::handle<Adaptor2d_Curve2d> arcS1;
  IntSurf_Transition             traline1;
  IntSurf_Transition             tra1;
  double                         prm1;
  bool                           onS2;
  bool                           vtxonS2;
  occ::handle<Adaptor3d_HVertex> vS2;
  occ::handle<Adaptor2d_Curve2d> arcS2;
  IntSurf_Transition             traline2;
  IntSurf_Transition             tra2;
  double                         prm2;
};

#include <IntPatch_Point.lxx>

#endif // _IntPatch_Point_HeaderFile
