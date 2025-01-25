// Created on: 2024-03-15
// Created by: Stephan GARNAUD (ARM)
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _OSD_FileNode_HeaderFile
#define _OSD_FileNode_HeaderFile

#include <Standard.hxx>

#include <OSD_Path.hxx>
#include <OSD_Error.hxx>

class OSD_Protection;
class Quantity_Date;

//! A class for 'File' and 'Directory' grouping common
//! methods (file/directory manipulation tools).
//! The "file oriented" name means files or directories which are
//! in fact hard coded as files.
class OSD_FileNode
{
public:
  DEFINE_STANDARD_ALLOC

  //! Gets file name and path.
  Standard_EXPORT void Path(OSD_Path& Name) const;

  //! Sets file name and path.
  //! If a name is not found, it raises a program error.
  Standard_EXPORT void SetPath(const OSD_Path& Name);

  //! Returns TRUE if <me> exists.
  Standard_EXPORT Standard_Boolean Exists();

  //! Erases the FileNode from directory
  Standard_EXPORT void Remove();

  //! Moves <me> into another directory
  Standard_EXPORT void Move(const OSD_Path& NewPath);

  //! Copies <me> to another FileNode
  Standard_EXPORT void Copy(const OSD_Path& ToPath);

  // None of the existing security APIs are supported in a UWP applications
  //! Returns access mode of <me>.
  Standard_EXPORT OSD_Protection Protection();

  //! Changes protection of the FileNode
  Standard_EXPORT void SetProtection(const OSD_Protection& Prot);

  //! Returns last write access.
  //! On UNIX, AccessMoment and CreationMoment return the
  //! same value.
  Standard_EXPORT Quantity_Date AccessMoment();

  //! Returns creation date.
  //! On UNIX, AccessMoment and CreationMoment return the
  //! same value.
  Standard_EXPORT Quantity_Date CreationMoment();

  //! Returns TRUE if an error occurs
  Standard_EXPORT Standard_Boolean Failed() const;

  //! Resets error counter to zero
  Standard_EXPORT void Reset();

  //! Raises OSD_Error
  Standard_EXPORT void Perror();

  //! Returns error number if 'Failed' is TRUE.
  Standard_EXPORT Standard_Integer Error() const;

protected:
  //! Creates FileNode object
  //! This is to be used with SetPath .
  //! Allocate space for the file name and initializes this
  //! name to an empty name.
  Standard_EXPORT OSD_FileNode();

  //! Instantiates the object FileNode storing its name.
  //! If a name is not found, it raises a program error.
  Standard_EXPORT OSD_FileNode(const OSD_Path& Name);

  //! Destructor is protected for safer inheritance
  ~OSD_FileNode() {}

protected:
  OSD_Path  myPath;
  OSD_Error myError;
};

#endif // _OSD_FileNode_HeaderFile
