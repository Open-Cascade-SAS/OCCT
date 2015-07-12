// Created on: 2008-05-30
// Created by: Vladislav ROMASHKO
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _Voxel_FastConverter_HeaderFile
#define _Voxel_FastConverter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Address.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Byte.hxx>
class TopoDS_Shape;
class Voxel_BoolDS;
class Voxel_ColorDS;
class Voxel_ROctBoolDS;
class gp_Pnt;
class gp_Pln;


//! Converts a shape to voxel representation.
//! It does it fast, but with less precision.
//! Also, it doesn't fill-in volumic part of the shape.
class Voxel_FastConverter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! A constructor for conversion of a shape into a cube of boolean voxels.
  //! It allocates the voxels in memory.
  //! "nbthreads" defines the number of threads used to convert the shape.
  Standard_EXPORT Voxel_FastConverter(const TopoDS_Shape& shape, Voxel_BoolDS& voxels, const Standard_Real deflection = 0.1, const Standard_Integer nbx = 10, const Standard_Integer nby = 10, const Standard_Integer nbz = 10, const Standard_Integer nbthreads = 1, const Standard_Boolean useExistingTriangulation = Standard_False);
  
  //! A constructor for conversion of a shape into a cube of colored voxels.
  //! It allocates the voxels in memory.
  //! "nbthreads" defines the number of threads used to convert the shape.
  Standard_EXPORT Voxel_FastConverter(const TopoDS_Shape& shape, Voxel_ColorDS& voxels, const Standard_Real deflection = 0.1, const Standard_Integer nbx = 10, const Standard_Integer nby = 10, const Standard_Integer nbz = 10, const Standard_Integer nbthreads = 1, const Standard_Boolean useExistingTriangulation = Standard_False);
  
  //! A constructor for conversion of a shape into a cube of boolean voxels
  //! split into 8 sub-voxels recursively.
  //! It allocates the voxels in memory.
  //! "nbthreads" defines the number of threads used to convert the shape.
  Standard_EXPORT Voxel_FastConverter(const TopoDS_Shape& shape, Voxel_ROctBoolDS& voxels, const Standard_Real deflection = 0.1, const Standard_Integer nbx = 10, const Standard_Integer nby = 10, const Standard_Integer nbz = 10, const Standard_Integer nbthreads = 1, const Standard_Boolean useExistingTriangulation = Standard_False);
  
  //! Converts a shape into a voxel representation.
  //! It sets to 0 the outside volume of the shape and
  //! 1 for surfacic part of the shape.
  //! "ithread" is the index of the thread for current call of ::Convert().
  //! Start numeration of "ithread" with 1, please.
  Standard_EXPORT Standard_Boolean Convert (Standard_Integer& progress, const Standard_Integer ithread = 1);
  
  //! Converts a shape into a voxel representation using separating axis theorem.
  //! It sets to 0 the outside volume of the shape and
  //! 1 for surfacic part of the shape.
  //! "ithread" is the index of the thread for current call of ::Convert().
  //! Start numeration of "ithread" with 1, please.
  Standard_EXPORT Standard_Boolean ConvertUsingSAT (Standard_Integer& progress, const Standard_Integer ithread = 1);
  
  //! Fills-in volume of the shape by a value.
  Standard_EXPORT Standard_Boolean FillInVolume (const Standard_Byte inner, const Standard_Integer ithread = 1);
  
  //! Fills-in volume of the shape by a value.
  //! Uses the topological information from the provided shape
  //! to judge whether points are inside the shape or not
  //! (only when processing vertical faces).
  //! The inner value has to be positive.
  Standard_EXPORT Standard_Boolean FillInVolume (const Standard_Byte inner, const TopoDS_Shape& shape, const Standard_Integer ithread = 1);
  
  //! A destructor.
  Standard_EXPORT void Destroy();
~Voxel_FastConverter()
{
  Destroy();
}




protected:





private:

  
  Standard_EXPORT void Init();
  
  Standard_EXPORT void GetBndBox (const gp_Pnt& p1, const gp_Pnt& p2, const gp_Pnt& p3, Standard_Real& xmin, Standard_Real& ymin, Standard_Real& zmin, Standard_Real& xmax, Standard_Real& ymax, Standard_Real& zmax) const;
  
  Standard_EXPORT void ComputeVoxelsNearTriangle (const gp_Pln& plane, const gp_Pnt& p1, const gp_Pnt& p2, const gp_Pnt& p3, const Standard_Real hdiag, const Standard_Integer ixmin, const Standard_Integer iymin, const Standard_Integer izmin, const Standard_Integer ixmax, const Standard_Integer iymax, const Standard_Integer izmax) const;
  
  Standard_EXPORT void ComputeVoxelsNearTriangle (const gp_Pnt& p1, const gp_Pnt& p2, const gp_Pnt& p3, const gp_Pnt& extents, const gp_Pnt& extents2, const gp_Pnt& extents4, const Standard_Integer ixmin, const Standard_Integer iymin, const Standard_Integer izmin, const Standard_Integer ixmax, const Standard_Integer iymax, const Standard_Integer izmax) const;


  TopoDS_Shape myShape;
  Standard_Address myVoxels;
  Standard_Real myDeflection;
  Standard_Integer myIsBool;
  Standard_Integer myNbX;
  Standard_Integer myNbY;
  Standard_Integer myNbZ;
  Standard_Integer myNbThreads;
  Standard_Integer myNbTriangles;
  Standard_Boolean myUseExistingTriangulation;


};







#endif // _Voxel_FastConverter_HeaderFile
