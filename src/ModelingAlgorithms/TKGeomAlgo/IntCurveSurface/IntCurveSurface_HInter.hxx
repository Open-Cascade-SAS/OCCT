// Created on: 1993-04-07
// Created by: Laurent BUCHARD
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

#ifndef _IntCurveSurface_HInter_HeaderFile
#define _IntCurveSurface_HInter_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <IntCurveSurface_Intersection.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>

class IntCurveSurface_TheHCurveTool;
class Adaptor3d_HSurfaceTool;
class IntCurveSurface_ThePolygonOfHInter;
class IntCurveSurface_ThePolygonToolOfHInter;
class IntCurveSurface_ThePolyhedronOfHInter;
class IntCurveSurface_ThePolyhedronToolOfHInter;
class IntCurveSurface_TheInterferenceOfHInter;
class IntCurveSurface_TheCSFunctionOfHInter;
class IntCurveSurface_TheExactHInter;
class IntCurveSurface_TheQuadCurvExactHInter;
class IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter;
class Bnd_BoundSortBox;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Parab;
class gp_Hypr;
class IntAna_IntConicQuad;
class Bnd_Box;

class IntCurveSurface_HInter : public IntCurveSurface_Intersection
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty Constructor
  Standard_EXPORT IntCurveSurface_HInter();

  //! Compute the Intersection between the curve and the
  //! surface
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Curve>&   Curve,
                               const occ::handle<Adaptor3d_Surface>& Surface);

  //! Compute the Intersection between the curve and
  //! the surface. The Curve is already sampled and
  //! its polygon : <Polygon> is given.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Curve>&            Curve,
                               const IntCurveSurface_ThePolygonOfHInter& Polygon,
                               const occ::handle<Adaptor3d_Surface>&          Surface);

  //! Compute the Intersection between the curve and
  //! the surface. The Curve is already sampled and
  //! its polygon : <Polygon> is given. The Surface is
  //! also sampled and <Polyhedron> is given.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Curve>&               Curve,
                               const IntCurveSurface_ThePolygonOfHInter&    ThePolygon,
                               const occ::handle<Adaptor3d_Surface>&             Surface,
                               const IntCurveSurface_ThePolyhedronOfHInter& Polyhedron);

  //! Compute the Intersection between the curve and
  //! the surface. The Curve is already sampled and
  //! its polygon : <Polygon> is given. The Surface is
  //! also sampled and <Polyhedron> is given.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Curve>&               Curve,
                               const IntCurveSurface_ThePolygonOfHInter&    ThePolygon,
                               const occ::handle<Adaptor3d_Surface>&             Surface,
                               const IntCurveSurface_ThePolyhedronOfHInter& Polyhedron,
                               Bnd_BoundSortBox&                            BndBSB);

  //! Compute the Intersection between the curve and
  //! the surface. The Surface is already sampled and
  //! its polyhedron : <Polyhedron> is given.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Curve>&               Curve,
                               const occ::handle<Adaptor3d_Surface>&             Surface,
                               const IntCurveSurface_ThePolyhedronOfHInter& Polyhedron);

protected:
  //! Compute the Intersection between the curve and the
  //! surface
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Curve>&   Curve,
                               const occ::handle<Adaptor3d_Surface>& Surface,
                               const double              U0,
                               const double              V0,
                               const double              U1,
                               const double              V1);

  Standard_EXPORT void InternalPerformCurveQuadric(const occ::handle<Adaptor3d_Curve>&   Curve,
                                                   const occ::handle<Adaptor3d_Surface>& Surface);

  Standard_EXPORT void InternalPerform(const occ::handle<Adaptor3d_Curve>&               Curve,
                                       const IntCurveSurface_ThePolygonOfHInter&    Polygon,
                                       const occ::handle<Adaptor3d_Surface>&             Surface,
                                       const IntCurveSurface_ThePolyhedronOfHInter& Polyhedron,
                                       const double                          U1,
                                       const double                          V1,
                                       const double                          U2,
                                       const double                          V2);

  Standard_EXPORT void InternalPerform(const occ::handle<Adaptor3d_Curve>&               Curve,
                                       const IntCurveSurface_ThePolygonOfHInter&    Polygon,
                                       const occ::handle<Adaptor3d_Surface>&             Surface,
                                       const IntCurveSurface_ThePolyhedronOfHInter& Polyhedron,
                                       const double                          U1,
                                       const double                          V1,
                                       const double                          U2,
                                       const double                          V2,
                                       Bnd_BoundSortBox&                            BSB);

  Standard_EXPORT void InternalPerform(const occ::handle<Adaptor3d_Curve>&            Curve,
                                       const IntCurveSurface_ThePolygonOfHInter& Polygon,
                                       const occ::handle<Adaptor3d_Surface>&          Surface,
                                       const double                       U1,
                                       const double                       V1,
                                       const double                       U2,
                                       const double                       V2);

  Standard_EXPORT void PerformConicSurf(const gp_Lin&                    Line,
                                        const occ::handle<Adaptor3d_Curve>&   Curve,
                                        const occ::handle<Adaptor3d_Surface>& Surface,
                                        const double              U1,
                                        const double              V1,
                                        const double              U2,
                                        const double              V2);

  Standard_EXPORT void PerformConicSurf(const gp_Circ&                   Circle,
                                        const occ::handle<Adaptor3d_Curve>&   Curve,
                                        const occ::handle<Adaptor3d_Surface>& Surface,
                                        const double              U1,
                                        const double              V1,
                                        const double              U2,
                                        const double              V2);

  Standard_EXPORT void PerformConicSurf(const gp_Elips&                  Ellipse,
                                        const occ::handle<Adaptor3d_Curve>&   Curve,
                                        const occ::handle<Adaptor3d_Surface>& Surface,
                                        const double              U1,
                                        const double              V1,
                                        const double              U2,
                                        const double              V2);

  Standard_EXPORT void PerformConicSurf(const gp_Parab&                  Parab,
                                        const occ::handle<Adaptor3d_Curve>&   Curve,
                                        const occ::handle<Adaptor3d_Surface>& Surface,
                                        const double              U1,
                                        const double              V1,
                                        const double              U2,
                                        const double              V2);

  Standard_EXPORT void PerformConicSurf(const gp_Hypr&                   Hyper,
                                        const occ::handle<Adaptor3d_Curve>&   Curve,
                                        const occ::handle<Adaptor3d_Surface>& Surface,
                                        const double              U1,
                                        const double              V1,
                                        const double              U2,
                                        const double              V2);

  Standard_EXPORT void AppendIntAna(const occ::handle<Adaptor3d_Curve>&   Curve,
                                    const occ::handle<Adaptor3d_Surface>& Surface,
                                    const IntAna_IntConicQuad&       InterAna);

  Standard_EXPORT void AppendPoint(const occ::handle<Adaptor3d_Curve>&   Curve,
                                   const double              w,
                                   const occ::handle<Adaptor3d_Surface>& Surface,
                                   const double              u,
                                   const double              v);

  Standard_EXPORT void AppendSegment(const occ::handle<Adaptor3d_Curve>&   Curve,
                                     const double              u0,
                                     const double              u1,
                                     const occ::handle<Adaptor3d_Surface>& Surface);

private:
  Standard_EXPORT void DoSurface(const occ::handle<Adaptor3d_Surface>& surface,
                                 const double              u0,
                                 const double              u1,
                                 const double              v0,
                                 const double              v1,
                                 NCollection_Array2<gp_Pnt>&              pntsOnSurface,
                                 Bnd_Box&                         boxSurface,
                                 double&                   gap);

  Standard_EXPORT void DoNewBounds(const occ::handle<Adaptor3d_Surface>& surface,
                                   const double              u0,
                                   const double              u1,
                                   const double              v0,
                                   const double              v1,
                                   const NCollection_Array2<gp_Pnt>&        pntsOnSurface,
                                   const NCollection_Array1<double>&      X,
                                   const NCollection_Array1<double>&      Y,
                                   const NCollection_Array1<double>&      Z,
                                   NCollection_Array1<double>&            Bounds);
};

#endif // _IntCurveSurface_HInter_HeaderFile
