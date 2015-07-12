// Created on: 2008-08-28
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

#ifndef _Voxel_Reader_HeaderFile
#define _Voxel_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <Standard_Boolean.hxx>
class TCollection_ExtendedString;


//! Reads a cube of voxels from disk.
//! Beware, a caller of the reader is responsible for deletion of the read voxels.
class Voxel_Reader 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! An empty constructor.
  Standard_EXPORT Voxel_Reader();
  
  //! Reads the voxels from disk
  Standard_EXPORT Standard_Boolean Read (const TCollection_ExtendedString& file);
  
  //! Informs the user about the type of voxels he has read.
  Standard_EXPORT Standard_Boolean IsBoolVoxels() const;
  
  //! Informs the user about the type of voxels he has read.
  Standard_EXPORT Standard_Boolean IsColorVoxels() const;
  
  //! Informs the user about the type of voxels he has read.
  Standard_EXPORT Standard_Boolean IsFloatVoxels() const;
  
  //! Returns a pointer to the read 1bit voxels.
  Standard_EXPORT Standard_Address GetBoolVoxels() const;
  
  //! Returns a pointer to the read 4bit voxels.
  Standard_EXPORT Standard_Address GetColorVoxels() const;
  
  //! Returns a pointer to the read 4bytes voxels.
  Standard_EXPORT Standard_Address GetFloatVoxels() const;




protected:





private:

  
  //! Reads 1bit voxels from disk in ASCII format.
  Standard_EXPORT Standard_Boolean ReadBoolAsciiVoxels (const TCollection_ExtendedString& file);
  
  //! Reads 4bit voxels from disk in ASCII format.
  Standard_EXPORT Standard_Boolean ReadColorAsciiVoxels (const TCollection_ExtendedString& file);
  
  //! Reads 4bytes voxels from disk in ASCII format.
  Standard_EXPORT Standard_Boolean ReadFloatAsciiVoxels (const TCollection_ExtendedString& file);
  
  //! Reads 1bit voxels from disk in BINARY format.
  Standard_EXPORT Standard_Boolean ReadBoolBinaryVoxels (const TCollection_ExtendedString& file);
  
  //! Reads 4bit voxels from disk in BINARY format.
  Standard_EXPORT Standard_Boolean ReadColorBinaryVoxels (const TCollection_ExtendedString& file);
  
  //! Reads 4bytes voxels from disk in BINARY format.
  Standard_EXPORT Standard_Boolean ReadFloatBinaryVoxels (const TCollection_ExtendedString& file);


  Standard_Address myBoolVoxels;
  Standard_Address myColorVoxels;
  Standard_Address myFloatVoxels;


};







#endif // _Voxel_Reader_HeaderFile
