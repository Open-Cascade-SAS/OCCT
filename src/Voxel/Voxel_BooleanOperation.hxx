// Created on: 2008-05-21
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

#ifndef _Voxel_BooleanOperation_HeaderFile
#define _Voxel_BooleanOperation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class Voxel_BoolDS;
class Voxel_ColorDS;
class Voxel_FloatDS;
class Voxel_DS;


//! Boolean operations (fuse, cut)
//! for voxels of the same dimension.
class Voxel_BooleanOperation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! An empty constructor.
  Standard_EXPORT Voxel_BooleanOperation();
  
  //! Fuses two cubes of voxels.
  //! It modifies the first cube of voxels.
  //! It returns false in case of different dimension of the cube,
  //! different number of voxels.
  Standard_EXPORT Standard_Boolean Fuse (Voxel_BoolDS& theVoxels1, const Voxel_BoolDS& theVoxels2) const;
  
  //! Fuses two cubes of voxels.
  //! It modifies the first cube of voxels.
  //! It returns false in case of different dimension of the cube,
  //! different number of voxels.
  //! It summerizes the value of corresponding voxels and puts the result to theVoxels1.
  //! If the result exceeds 15 or becomes greater, it keeps 15.
  Standard_EXPORT Standard_Boolean Fuse (Voxel_ColorDS& theVoxels1, const Voxel_ColorDS& theVoxels2) const;
  
  //! Fuses two cubes of voxels.
  //! It modifies the first cube of voxels.
  //! It returns false in case of different dimension of the cube,
  //! different number of voxels.
  //! It summerizes the value of corresponding voxels and puts the result to theVoxels1.
  Standard_EXPORT Standard_Boolean Fuse (Voxel_FloatDS& theVoxels1, const Voxel_FloatDS& theVoxels2) const;
  
  //! Cuts two cubes of voxels.
  //! It modifies the first cube of voxels.
  //! It returns false in case of different dimension of the cube,
  //! different number of voxels.
  Standard_EXPORT Standard_Boolean Cut (Voxel_BoolDS& theVoxels1, const Voxel_BoolDS& theVoxels2) const;
  
  //! Cuts two cubes of voxels.
  //! It modifies the first cube of voxels.
  //! It returns false in case of different dimension of the cube,
  //! different number of voxels.
  //! It subtracts the value of corresponding voxels and puts the result to theVoxels1.
  Standard_EXPORT Standard_Boolean Cut (Voxel_ColorDS& theVoxels1, const Voxel_ColorDS& theVoxels2) const;
  
  //! Cuts two cubes of voxels.
  //! It modifies the first cube of voxels.
  //! It returns false in case of different dimension of the cube,
  //! different number of voxels.
  //! It subtracts the value of corresponding voxels and puts the result to theVoxels1.
  Standard_EXPORT Standard_Boolean Cut (Voxel_FloatDS& theVoxels1, const Voxel_FloatDS& theVoxels2) const;




protected:





private:

  
  Standard_EXPORT Standard_Boolean Check (const Voxel_DS& theVoxels1, const Voxel_DS& theVoxels2) const;




};







#endif // _Voxel_BooleanOperation_HeaderFile
