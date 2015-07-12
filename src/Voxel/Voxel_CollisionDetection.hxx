// Created on: 2008-07-14
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

#ifndef _Voxel_CollisionDetection_HeaderFile
#define _Voxel_CollisionDetection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_ListOfShape.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Address.hxx>
#include <Voxel_BoolDS.hxx>
class TopoDS_Shape;
class Bnd_Box;
class Voxel_BoolDS;


//! Detects collisions between shapes.
class Voxel_CollisionDetection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! An empty constructor.
  Standard_EXPORT Voxel_CollisionDetection();
  
  //! A constructor.
  //! It defines deflection of triangulation for the shapes.
  //! As lower the deflection is, as proper the triangulation is generated.
  //! Also, it defines number of splits along X, Y and Z axes for generation of voxels.
  //! As greater the numbers are, as greater number of voxels is used for detection of collision.
  Standard_EXPORT Voxel_CollisionDetection(const Standard_Real deflection, const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz);
  
  //! Adds a shape.
  //! Returns an index of the shape.
  Standard_EXPORT Standard_Integer AddShape (const TopoDS_Shape& shape);
  
  //! Replaces a shape by another one.
  //! <ishape> is an index of the shape.
  //! This method is useful for moving shape, for example.
  Standard_EXPORT Standard_Boolean ReplaceShape (const Standard_Integer ishape, const TopoDS_Shape& shape);
  
  //! Defines the deflection of triangulation of shapes.
  Standard_EXPORT void SetDeflection (const Standard_Real deflection);
  
  //! Defines the number of voxels along X, Y and Z axes.
  Standard_EXPORT void SetNbVoxels (const Standard_Integer nbx, const Standard_Integer nby, const Standard_Integer nbz);
  
  //! Defines a user-defined boundary box for generation of voxels.
  //! If this method is not called, the algorithm calculates the boundary box itself.
  Standard_EXPORT void SetBoundaryBox (const Bnd_Box& box);
  
  //! Defines usage of volume of shapes in collision detection algorithm.
  //! Beware, usage of volume a little bit decreases the speed of algorithm.
  Standard_EXPORT void SetUsageOfVolume (const Standard_Boolean usage);
  
  //! Doesn't clean the collision points on new call to the method Compute().
  //! It allows to see the collisions for a moving shape.
  Standard_EXPORT void KeepCollisions (const Standard_Boolean keep);
  
  //! Prepares data for computation of collisions.
  //! It checks the inner parameters (number of voxels along X, Y and Z axes) and
  //! voxelizes the shapes.
  //! If the shape is not changed since the last call to this method,
  //! this method may be not called for this shape.
  //! <ishape> - is the index of the shape for processing by this method.
  //! If it is equal to -1, all shapes will be processed.
  Standard_EXPORT Standard_Boolean Voxelize (const Standard_Integer ishape = -1);
  
  //! Computes the collisions.
  //! This method may be called many times if, for example, the shapes are being moved.
  Standard_EXPORT Standard_Boolean Compute();
  
  //! Returns true if a collision is detected.
  Standard_EXPORT Standard_Boolean HasCollisions() const;
  
  //! Returns the collided voxels.
  Standard_EXPORT const Voxel_BoolDS& GetCollisions() const;
  
  //! A destructor.
  Standard_EXPORT void Destroy();
~Voxel_CollisionDetection()
{
  Destroy();
}




protected:





private:

  
  //! An internal method for cleaning the intermediate data.
  Standard_EXPORT void Clear();
  
  //! An internal method, which checks correspondance
  //! of voxels to the parameters defined by user.
  Standard_EXPORT Standard_Boolean CheckVoxels (const Voxel_BoolDS& voxels) const;


  TopTools_ListOfShape myShapes;
  Standard_Real myDeflection;
  Standard_Integer myNbX;
  Standard_Integer myNbY;
  Standard_Integer myNbZ;
  Standard_Boolean myUsageOfVolume;
  Standard_Boolean myKeepCollisions;
  Standard_Real myX;
  Standard_Real myY;
  Standard_Real myZ;
  Standard_Real myXLen;
  Standard_Real myYLen;
  Standard_Real myZLen;
  Standard_Address myVoxels;
  Voxel_BoolDS myCollisions;
  Standard_Boolean myHasCollisions;


};







#endif // _Voxel_CollisionDetection_HeaderFile
