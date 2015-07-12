// Created on: 2008-05-15
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

#ifndef _Voxel_FloatDS_HeaderFile
#define _Voxel_FloatDS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Voxel_DS.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_ShortReal.hxx>


//! A 3D voxel model keeping a foating-point
//! value for each voxel.
class Voxel_FloatDS  : public Voxel_DS
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! An empty constructor.
  Standard_EXPORT Voxel_FloatDS();
  
  //! A constructor initializing the voxel model.
  //! (x, y, z) - the start point of the box.
  //! (x_len, y_len, z_len) - lengths in x, y and z directions along axes of a co-ordinate system.
  //! (nb_x, nb_y, nb_z) - number of splits (voxels) along x, y and z directions.
  Standard_EXPORT Voxel_FloatDS(const Standard_Real x, const Standard_Real y, const Standard_Real z, const Standard_Real x_len, const Standard_Real y_len, const Standard_Real z_len, const Standard_Integer nb_x, const Standard_Integer nb_y, const Standard_Integer nb_z);
  
  //! Initialization of the voxel model.
  //! (x, y, z) - the start point of the box.
  //! (x_len, y_len, z_len) - lengths in x, y and z directions along axes of a co-ordinate system.
  //! (nb_x, nb_y, nb_z) - number of splits (voxels) along x, y and z directions.
  Standard_EXPORT virtual void Init (const Standard_Real x, const Standard_Real y, const Standard_Real z, const Standard_Real x_len, const Standard_Real y_len, const Standard_Real z_len, const Standard_Integer nb_x, const Standard_Integer nb_y, const Standard_Integer nb_z) Standard_OVERRIDE;
  
  //! A destructor of the voxel model.
  Standard_EXPORT void Destroy();
~Voxel_FloatDS()
{
  Destroy();
}
  
  //! The method sets all values equal to 0 (false) and
  //! releases the memory.
  Standard_EXPORT void SetZero();
  
  //! Defines a value for voxel with co-ordinates (ix, iy, iz).
  //! Initial state of the model is so that all voxels have value 0.0f,
  //! and this data doesn't occupy memory.
  //! Memory for data is allocating during setting non-zero values.
  Standard_EXPORT void Set (const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, const Standard_ShortReal data);
  
  //! Returns the value of voxel with co-ordinates (ix, iy, iz).
  Standard_EXPORT Standard_ShortReal Get (const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz) const;




protected:





private:





};







#endif // _Voxel_FloatDS_HeaderFile
