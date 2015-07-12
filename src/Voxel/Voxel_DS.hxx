// Created on: 2008-05-07
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

#ifndef _Voxel_DS_HeaderFile
#define _Voxel_DS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Voxel_Writer;
class Voxel_Reader;


//! A base class for all voxel data structures.
class Voxel_DS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! An empty constructor.
  Standard_EXPORT Voxel_DS();
  
  //! A constructor initializing the voxel model.
  //! (x, y, z) - the start point of the box.
  //! (x_len, y_len, z_len) - lengths in x, y and z directions along axes of a co-ordinate system.
  //! (nb_x, nb_y, nb_z) - number of splits (voxels) along x, y and z directions.
  Standard_EXPORT Voxel_DS(const Standard_Real x, const Standard_Real y, const Standard_Real z, const Standard_Real x_len, const Standard_Real y_len, const Standard_Real z_len, const Standard_Integer nb_x, const Standard_Integer nb_y, const Standard_Integer nb_z);
  
  //! Initialization of the voxel model.
  //! (x, y, z) - the start point of the box.
  //! (x_len, y_len, z_len) - lengths in x, y and z directions along axes of a co-ordinate system.
  //! (nb_x, nb_y, nb_z) - number of splits (voxels) along x, y and z directions.
  //! The methods below return initial data of the voxel model.
  Standard_EXPORT virtual void Init (const Standard_Real x, const Standard_Real y, const Standard_Real z, const Standard_Real x_len, const Standard_Real y_len, const Standard_Real z_len, const Standard_Integer nb_x, const Standard_Integer nb_y, const Standard_Integer nb_z);
  
  Standard_EXPORT Standard_Real GetX() const;
  
  Standard_EXPORT Standard_Real GetY() const;
  
  Standard_EXPORT Standard_Real GetZ() const;
  
  Standard_EXPORT Standard_Real GetXLen() const;
  
  Standard_EXPORT Standard_Real GetYLen() const;
  
  Standard_EXPORT Standard_Real GetZLen() const;
  
  Standard_EXPORT Standard_Integer GetNbX() const;
  
  Standard_EXPORT Standard_Integer GetNbY() const;
  
  Standard_EXPORT Standard_Integer GetNbZ() const;
  
  //! Returns the center point of a voxel with co-ordinates (ix, iy, iz).
  Standard_EXPORT void GetCenter (const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, Standard_Real& xc, Standard_Real& yc, Standard_Real& zc) const;
  
  //! Returns the origin point of a voxel with co-ordinates (ix, iy, iz).
  Standard_EXPORT void GetOrigin (const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz, Standard_Real& x0, Standard_Real& y0, Standard_Real& z0) const;
  
  //! Finds a voxel corresponding to a 3D point.
  //! Returns true if it is found.
  Standard_EXPORT Standard_Boolean GetVoxel (const Standard_Real x, const Standard_Real y, const Standard_Real z, Standard_Integer& ix, Standard_Integer& iy, Standard_Integer& iz) const;
  
  //! Returns x-index of a voxel corresponding to x-coordinate.
  Standard_EXPORT Standard_Boolean GetVoxelX (const Standard_Real x, Standard_Integer& ix) const;
  
  //! Returns y-index of a voxel corresponding to y-coordinate.
  Standard_EXPORT Standard_Boolean GetVoxelY (const Standard_Real y, Standard_Integer& iy) const;
  
  //! Returns z-index of a voxel corresponding to z-coordinate.
  Standard_EXPORT Standard_Boolean GetVoxelZ (const Standard_Real z, Standard_Integer& iz) const;


friend class Voxel_Writer;
friend class Voxel_Reader;


protected:



  Standard_Address myData;
  Standard_Real myX;
  Standard_Real myY;
  Standard_Real myZ;
  Standard_Real myXLen;
  Standard_Real myYLen;
  Standard_Real myZLen;
  Standard_Integer myNbX;
  Standard_Integer myNbY;
  Standard_Integer myNbZ;
  Standard_Integer myNbXY;
  Standard_Real myDX;
  Standard_Real myDY;
  Standard_Real myDZ;
  Standard_Real myHalfDX;
  Standard_Real myHalfDY;
  Standard_Real myHalfDZ;


private:





};







#endif // _Voxel_DS_HeaderFile
