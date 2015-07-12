// Created on: 2008-07-30
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

#ifndef _Voxel_Selector_HeaderFile
#define _Voxel_Selector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class V3d_View;
class Voxel_BoolDS;
class Voxel_ColorDS;
class Voxel_ROctBoolDS;


//! Detects voxels in the viewer 3d under the mouse cursor.
class Voxel_Selector 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! An empty constructor.
  Standard_EXPORT Voxel_Selector();
  
  //! A constructor of the selector,
  //! which initializes the classes
  //! by a view, where the user selects the voxels.
  Standard_EXPORT Voxel_Selector(const Handle(V3d_View)& view);
  
  //! Initializes the selector by a view,
  //! where the user selects the voxels.
  Standard_EXPORT void Init (const Handle(V3d_View)& view);
  
  //! Defines the voxels (1bit).
  Standard_EXPORT void SetVoxels (const Voxel_BoolDS& voxels);
  
  //! Defines the voxels (4bit).
  Standard_EXPORT void SetVoxels (const Voxel_ColorDS& voxels);
  
  //! Defines the voxels (1bit recursive splitting).
  Standard_EXPORT void SetVoxels (const Voxel_ROctBoolDS& voxels);
  
  //! Detects a voxel under the mouse cursor.
  Standard_EXPORT Standard_Boolean Detect (const Standard_Integer winx, const Standard_Integer winy, Standard_Integer& ix, Standard_Integer& iy, Standard_Integer& iz);




protected:





private:



  Handle(V3d_View) myView;
  Standard_Address myVoxels;
  Standard_Integer myIsBool;


};







#endif // _Voxel_Selector_HeaderFile
