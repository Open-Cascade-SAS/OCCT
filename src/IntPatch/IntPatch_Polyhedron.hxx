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
#include <Bnd_HArray1OfBox.hxx>

//! This class provides a linear approximation of the PSurface.
//! preview a constructor on a zone of  a surface
class IntPatch_Polyhedron 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! MaTriangle constructor with an double array of pnt for the
  //! representation of a double array of triangles.
  Standard_EXPORT IntPatch_Polyhedron(const Handle(Adaptor3d_Surface)& Surface, const Standard_Integer nbdU, const Standard_Integer nbdV);
  
  Standard_EXPORT IntPatch_Polyhedron(const Handle(Adaptor3d_Surface)& Surface);
  
  Standard_EXPORT void Destroy();
~IntPatch_Polyhedron()
{
  Destroy();
}
  
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& Surface, const Standard_Integer nbdU, const Standard_Integer nbdV);
  
  Standard_EXPORT void DeflectionOverEstimation (const Standard_Real flec);
  
  Standard_EXPORT Standard_Real DeflectionOnTriangle (const Handle(Adaptor3d_Surface)& Surface, const Standard_Integer Index) const;
  
  Standard_EXPORT void UMinSingularity (const Standard_Boolean Sing);
  
  Standard_EXPORT void UMaxSingularity (const Standard_Boolean Sing);
  
  Standard_EXPORT void VMinSingularity (const Standard_Boolean Sing);
  
  Standard_EXPORT void VMaxSingularity (const Standard_Boolean Sing);
  
  //! Get the size of the MaTriangle.
  Standard_EXPORT void Size (Standard_Integer& nbdu, Standard_Integer& nbdv) const;
  
  //! Give the number of triangles in this double array of
  //! triangles (nbdu*nbdv*2).
  Standard_EXPORT Standard_Integer NbTriangles() const;
  
  //! Give the 3 points of the triangle of address Index in
  //! the double array of triangles.
  Standard_EXPORT void Triangle (const Standard_Integer Index, Standard_Integer& P1, Standard_Integer& P2, Standard_Integer& P3) const;
  
  //! Give the address Tricon of the triangle connexe to the
  //! triangle of address Triang by the edge Pivot Pedge and
  //! the third point of this connexe triangle. When we are
  //! on a free edge TriCon==0 but the function return the
  //! value of the triangle in the other side of Pivot on
  //! the free edge. Used to turn around a vertex.
  Standard_EXPORT Standard_Integer TriConnex (const Standard_Integer Triang, const Standard_Integer Pivot, const Standard_Integer Pedge, Standard_Integer& TriCon, Standard_Integer& OtherP) const;
  
  //! Give the number of point in the double array of
  //! triangles ((nbdu+1)*(nbdv+1)).
  Standard_EXPORT Standard_Integer NbPoints() const;
  
  //! Set the value of a field of the double array of
  //! points.
  Standard_EXPORT void Point (const gp_Pnt& thePnt, const Standard_Integer lig, const Standard_Integer col, const Standard_Real U, const Standard_Real V);
  
  //! Give the point of index i in the MaTriangle.
  Standard_EXPORT const gp_Pnt& Point (const Standard_Integer Index, Standard_Real& U, Standard_Real& V) const;
  
  //! Give the point of index i in the MaTriangle.
  Standard_EXPORT const gp_Pnt& Point (const Standard_Integer Index) const;
  
  //! Give the point of index i in the MaTriangle.
  Standard_EXPORT void Point (const Standard_Integer Index, gp_Pnt& P) const;
  
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
  Standard_EXPORT void PlaneEquation (const Standard_Integer Triang, gp_XYZ& NormalVector, Standard_Real& PolarDistance) const;
  
  //! Give the plane equation of the triangle of address Triang.
  Standard_EXPORT Standard_Boolean Contain (const Standard_Integer Triang, const gp_Pnt& ThePnt) const;
  
  Standard_EXPORT void Parameters (const Standard_Integer Index, Standard_Real& U, Standard_Real& V) const;
  
  Standard_EXPORT void Dump() const;




protected:





private:



  Bnd_Box TheBnd;
  Handle(Bnd_HArray1OfBox) TheComponentsBnd;
  Standard_Real TheDeflection;
  Standard_Integer nbdeltaU;
  Standard_Integer nbdeltaV;
  Standard_Address C_MyPnts;
  Standard_Address C_MyU;
  Standard_Address C_MyV;
  Standard_Boolean UMinSingular;
  Standard_Boolean UMaxSingular;
  Standard_Boolean VMinSingular;
  Standard_Boolean VMaxSingular;


};







#endif // _IntPatch_Polyhedron_HeaderFile
