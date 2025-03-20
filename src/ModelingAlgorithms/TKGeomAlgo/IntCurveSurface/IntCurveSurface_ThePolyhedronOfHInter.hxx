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

#ifndef _IntCurveSurface_ThePolyhedronOfHInter_HeaderFile
#define _IntCurveSurface_ThePolyhedronOfHInter_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <Bnd_HArray1OfBox.hxx>
#include <TColStd_Array1OfReal.hxx>

class Adaptor3d_HSurfaceTool;
class Bnd_Box;

class IntCurveSurface_ThePolyhedronOfHInter
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntCurveSurface_ThePolyhedronOfHInter(const Handle(Adaptor3d_Surface)& Surface,
                                                        const Standard_Integer           nbdU,
                                                        const Standard_Integer           nbdV,
                                                        const Standard_Real              U1,
                                                        const Standard_Real              V1,
                                                        const Standard_Real              U2,
                                                        const Standard_Real              V2);

  Standard_EXPORT IntCurveSurface_ThePolyhedronOfHInter(const Handle(Adaptor3d_Surface)& Surface,
                                                        const TColStd_Array1OfReal&      Upars,
                                                        const TColStd_Array1OfReal&      Vpars);

  Standard_EXPORT void Destroy();

  ~IntCurveSurface_ThePolyhedronOfHInter() { Destroy(); }

  Standard_EXPORT void DeflectionOverEstimation(const Standard_Real flec);

  Standard_EXPORT Standard_Real DeflectionOnTriangle(const Handle(Adaptor3d_Surface)& Surface,
                                                     const Standard_Integer           Index) const;

  Standard_EXPORT void UMinSingularity(const Standard_Boolean Sing);

  Standard_EXPORT void UMaxSingularity(const Standard_Boolean Sing);

  Standard_EXPORT void VMinSingularity(const Standard_Boolean Sing);

  Standard_EXPORT void VMaxSingularity(const Standard_Boolean Sing);

  //! get the size of the discretization.
  Standard_EXPORT void Size(Standard_Integer& nbdu, Standard_Integer& nbdv) const;

  //! Give the number of triangles in this double array of
  Standard_EXPORT Standard_Integer NbTriangles() const;

  //! Give the 3 points of the triangle of address Index in
  //! the double array of triangles.
  Standard_EXPORT void Triangle(const Standard_Integer Index,
                                Standard_Integer&      P1,
                                Standard_Integer&      P2,
                                Standard_Integer&      P3) const;

  //! Give the address Tricon of the triangle connexe to the
  //! triangle of address Triang by the edge Pivot Pedge and
  //! the third point of this connexe triangle. When we are
  //! on a free edge TriCon==0 but the function return the
  //! value of the triangle in the other side of Pivot on
  //! the free edge. Used to turn around a vertex.
  Standard_EXPORT Standard_Integer TriConnex(const Standard_Integer Triang,
                                             const Standard_Integer Pivot,
                                             const Standard_Integer Pedge,
                                             Standard_Integer&      TriCon,
                                             Standard_Integer&      OtherP) const;

  //! Give the number of point in the double array of
  //! triangles ((nbdu+1)*(nbdv+1)).
  Standard_EXPORT Standard_Integer NbPoints() const;

  //! Set the value of a field of the double array of
  //! points.
  Standard_EXPORT void Point(const gp_Pnt&          thePnt,
                             const Standard_Integer lig,
                             const Standard_Integer col,
                             const Standard_Real    U,
                             const Standard_Real    V);

  //! Give the point of index i in the MaTriangle.
  Standard_EXPORT const gp_Pnt& Point(const Standard_Integer Index,
                                      Standard_Real&         U,
                                      Standard_Real&         V) const;

  //! Give the point of index i in the MaTriangle.
  Standard_EXPORT const gp_Pnt& Point(const Standard_Integer Index) const;

  //! Give the point of index i in the MaTriangle.
  Standard_EXPORT void Point(const Standard_Integer Index, gp_Pnt& P) const;

  //! Give the bounding box of the MaTriangle.
  Standard_EXPORT const Bnd_Box& Bounding() const;

  //! Compute the array of boxes. The box <n> corresponding
  //! to the triangle <n>.
  Standard_EXPORT void FillBounding();

  //! Give the array of boxes. The box <n> corresponding
  //! to the triangle <n>.
  Standard_EXPORT const Handle(Bnd_HArray1OfBox)& ComponentsBounding() const;

  Standard_EXPORT Standard_Real DeflectionOverEstimation() const;

  Standard_EXPORT Standard_Boolean HasUMinSingularity() const;

  Standard_EXPORT Standard_Boolean HasUMaxSingularity() const;

  Standard_EXPORT Standard_Boolean HasVMinSingularity() const;

  Standard_EXPORT Standard_Boolean HasVMaxSingularity() const;

  //! Give the plane equation of the triangle of address Triang.
  Standard_EXPORT void PlaneEquation(const Standard_Integer Triang,
                                     gp_XYZ&                NormalVector,
                                     Standard_Real&         PolarDistance) const;

  //! Give the plane equation of the triangle of address Triang.
  Standard_EXPORT Standard_Boolean Contain(const Standard_Integer Triang,
                                           const gp_Pnt&          ThePnt) const;

  Standard_EXPORT void Parameters(const Standard_Integer Index,
                                  Standard_Real&         U,
                                  Standard_Real&         V) const;

  //! This method returns true if the edge based on points with
  //! indices Index1 and Index2 represents a boundary edge. It is
  //! necessary to take into account the boundary deflection for
  //! this edge.
  Standard_EXPORT Standard_Boolean IsOnBound(const Standard_Integer Index1,
                                             const Standard_Integer Index2) const;

  //! This method returns a border deflection.
  Standard_Real GetBorderDeflection() const;

  Standard_EXPORT void Dump() const;

protected:
  Standard_EXPORT void Init(const Handle(Adaptor3d_Surface)& Surface,
                            const Standard_Real              U1,
                            const Standard_Real              V1,
                            const Standard_Real              U2,
                            const Standard_Real              V2);

  Standard_EXPORT void Init(const Handle(Adaptor3d_Surface)& Surface,
                            const TColStd_Array1OfReal&      Upars,
                            const TColStd_Array1OfReal&      Vpars);

private:
  //! This method computes and returns a deflection of isoline
  //! of given parameter on Surface.
  Standard_EXPORT Standard_Real ComputeBorderDeflection(const Handle(Adaptor3d_Surface)& Surface,
                                                        const Standard_Real              Parameter,
                                                        const Standard_Real              PMin,
                                                        const Standard_Real              PMax,
                                                        const Standard_Boolean isUIso) const;

  Standard_Integer         nbdeltaU;
  Standard_Integer         nbdeltaV;
  Bnd_Box                  TheBnd;
  Handle(Bnd_HArray1OfBox) TheComponentsBnd;
  Standard_Real            TheDeflection;
  Standard_Address         C_MyPnts;
  Standard_Address         C_MyU;
  Standard_Address         C_MyV;
  Standard_Boolean         UMinSingular;
  Standard_Boolean         UMaxSingular;
  Standard_Boolean         VMinSingular;
  Standard_Boolean         VMaxSingular;
  Standard_Real            TheBorderDeflection;
  Standard_Address         C_MyIsOnBounds;
};

#define ThePSurface Handle(Adaptor3d_Surface)
#define ThePSurface_hxx <Adaptor3d_Surface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define IntCurveSurface_Polyhedron IntCurveSurface_ThePolyhedronOfHInter
#define IntCurveSurface_Polyhedron_hxx <IntCurveSurface_ThePolyhedronOfHInter.hxx>

#include <IntCurveSurface_Polyhedron.lxx>

#undef ThePSurface
#undef ThePSurface_hxx
#undef ThePSurfaceTool
#undef ThePSurfaceTool_hxx
#undef IntCurveSurface_Polyhedron
#undef IntCurveSurface_Polyhedron_hxx

#endif // _IntCurveSurface_ThePolyhedronOfHInter_HeaderFile
