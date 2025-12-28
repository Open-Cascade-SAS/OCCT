// Created on: 1993-02-03
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

#ifndef _IntPatch_Polyhedron_HeaderFile
#define _IntPatch_Polyhedron_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//! This class provides a linear approximation of the PSurface.
//! preview a constructor on a zone of a surface
class IntPatch_Polyhedron
{
public:
  DEFINE_STANDARD_ALLOC

  //! MaTriangle constructor with an double array of pnt for the
  //! representation of a double array of triangles.
  Standard_EXPORT IntPatch_Polyhedron(const occ::handle<Adaptor3d_Surface>& Surface,
                                      const int                             nbdU,
                                      const int                             nbdV);

  Standard_EXPORT IntPatch_Polyhedron(const occ::handle<Adaptor3d_Surface>& Surface);

  Standard_EXPORT void Destroy();

  ~IntPatch_Polyhedron() { Destroy(); }

  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>& Surface,
                               const int                             nbdU,
                               const int                             nbdV);

  Standard_EXPORT void DeflectionOverEstimation(const double flec);

  Standard_EXPORT void UMinSingularity(const bool Sing);

  Standard_EXPORT void UMaxSingularity(const bool Sing);

  Standard_EXPORT void VMinSingularity(const bool Sing);

  Standard_EXPORT void VMaxSingularity(const bool Sing);

  //! Get the size of the MaTriangle.
  Standard_EXPORT void Size(int& nbdu, int& nbdv) const;

  //! Give the number of triangles in this double array of
  //! triangles (nbdu*nbdv*2).
  Standard_EXPORT int NbTriangles() const;

  //! Give the 3 points of the triangle of address Index in
  //! the double array of triangles.
  Standard_EXPORT void Triangle(const int Index, int& P1, int& P2, int& P3) const;

  //! Give the address Tricon of the triangle connexe to the
  //! triangle of address Triang by the edge Pivot Pedge and
  //! the third point of this connexe triangle. When we are
  //! on a free edge TriCon==0 but the function return the
  //! value of the triangle in the other side of Pivot on
  //! the free edge. Used to turn around a vertex.
  Standard_EXPORT int TriConnex(const int Triang,
                                const int Pivot,
                                const int Pedge,
                                int&      TriCon,
                                int&      OtherP) const;

  //! Give the number of point in the double array of
  //! triangles ((nbdu+1)*(nbdv+1)).
  Standard_EXPORT int NbPoints() const;

  //! Set the value of a field of the double array of
  //! points.
  Standard_EXPORT void Point(const gp_Pnt& thePnt,
                             const int     lig,
                             const int     col,
                             const double  U,
                             const double  V);

  //! Give the point of index i in the MaTriangle.
  Standard_EXPORT const gp_Pnt& Point(const int Index, double& U, double& V) const;

  //! Give the point of index i in the MaTriangle.
  Standard_EXPORT const gp_Pnt& Point(const int Index) const;

  //! Give the point of index i in the MaTriangle.
  Standard_EXPORT void Point(const int Index, gp_Pnt& P) const;

  //! Give the bounding box of the MaTriangle.
  Standard_EXPORT const Bnd_Box& Bounding() const;

  //! Compute the array of boxes. The box <n> corresponding
  //! to the triangle <n>.
  Standard_EXPORT void FillBounding();

  //! Give the array of boxes. The box <n> corresponding
  //! to the triangle <n>.
  Standard_EXPORT const occ::handle<NCollection_HArray1<Bnd_Box>>& ComponentsBounding() const;

  Standard_EXPORT double DeflectionOverEstimation() const;

  Standard_EXPORT bool HasUMinSingularity() const;

  Standard_EXPORT bool HasUMaxSingularity() const;

  Standard_EXPORT bool HasVMinSingularity() const;

  Standard_EXPORT bool HasVMaxSingularity() const;

  //! Give the plane equation of the triangle of address Triang.
  Standard_EXPORT void PlaneEquation(const int Triang,
                                     gp_XYZ&   NormalVector,
                                     double&   PolarDistance) const;

  //! Give the plane equation of the triangle of address Triang.
  Standard_EXPORT bool Contain(const int Triang, const gp_Pnt& ThePnt) const;

  Standard_EXPORT void Parameters(const int Index, double& U, double& V) const;

  Standard_EXPORT void Dump() const;

private:
  Bnd_Box                                   TheBnd;
  occ::handle<NCollection_HArray1<Bnd_Box>> TheComponentsBnd;
  double                                    TheDeflection;
  int                                       nbdeltaU;
  int                                       nbdeltaV;
  void*                                     C_MyPnts;
  void*                                     C_MyU;
  void*                                     C_MyV;
  bool                                      UMinSingular;
  bool                                      UMaxSingular;
  bool                                      VMinSingular;
  bool                                      VMaxSingular;
};

#endif // _IntPatch_Polyhedron_HeaderFile
