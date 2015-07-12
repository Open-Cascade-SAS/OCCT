// Created on: 1992-05-18
// Created by: Stephan GARNAUD (ARM)
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _OSD_Directory_HeaderFile
#define _OSD_Directory_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <OSD_FileNode.hxx>
class OSD_Path;
class OSD_Protection;


//! Management of directories (a set of directory oriented tools)
class OSD_Directory  : public OSD_FileNode
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates Directory object.
  //! It is initiliazed to an empty name.
  Standard_EXPORT OSD_Directory();
  
  //! Creates Directory object initialized with Name.
  Standard_EXPORT OSD_Directory(const OSD_Path& Name);
  
  //! Creates (physically) a directory.
  //! When a directory of the same name already exists, no error is
  //! returned, and only <Protect> is applied to the existing directory.
  //!
  //! If Build is used and <me> is instantiated without a name,
  //! OSDError is raised.
  Standard_EXPORT void Build (const OSD_Protection& Protect);
  
  //! Creates a temporary Directory in current directory.
  //! This directory is automatically removed when object dies.
  Standard_EXPORT static OSD_Directory BuildTemporary();




protected:





private:





};







#endif // _OSD_Directory_HeaderFile
