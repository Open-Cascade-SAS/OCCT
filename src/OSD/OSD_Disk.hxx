// Created on: 1992-05-27
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

#ifndef _OSD_Disk_HeaderFile
#define _OSD_Disk_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <OSD_Error.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
class OSD_OSDError;
class OSD_Path;


//! Disk management (a set of disk oriented tools)
class OSD_Disk 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a disk object.
  //! This is used only when a class contains a Disk field.
  //! By default, its name is initialized to current working disk.
  Standard_EXPORT OSD_Disk();
  
  //! Initializes the object Disk with the disk name
  //! associated to the OSD_Path.
  Standard_EXPORT OSD_Disk(const OSD_Path& Name);
  
  //! Initializes the object Disk with <PathName>.
  //! <PathName> specifies any file within the mounted
  //! file system.
  //! Example : OSD_Disk myDisk ("/tmp")
  //! Initializes a disk object with the mounted
  //! file associated to /tmp.
  Standard_EXPORT OSD_Disk(const Standard_CString PathName);
  
  //! Returns disk name of <me>.
  Standard_EXPORT OSD_Path Name() const;
  
  //! Instantiates <me> with <Name>.
  Standard_EXPORT void SetName (const OSD_Path& Name);
  
  //! Returns total disk capacity in 512 bytes blocks.
  Standard_EXPORT Standard_Integer DiskSize();
  
  //! Returns free available 512 bytes blocks on disk.
  Standard_EXPORT Standard_Integer DiskFree();
  
  //! Returns user's disk quota (in Bytes).
  Standard_EXPORT Standard_Integer DiskQuota();
  
  //! Sets user's disk quota (in Bytes).
  //! Warning: Needs system administrator privilege.
  Standard_EXPORT void SetDiskQuota (const Standard_Integer QuotaSize);
  
  //! Activates user's disk quota
  //! Warning: Needs system administrator privilege.
  Standard_EXPORT void SetQuotaOn();
  
  //! Deactivates user's disk quota
  //! Warning: Needs system administrator privilege.
  Standard_EXPORT void SetQuotaOff();
  
  //! Returns TRUE if an error occurs
  Standard_EXPORT Standard_Boolean Failed() const;
  
  //! Resets error counter to zero
  Standard_EXPORT void Reset();
  
  //! Raises OSD_Error
  Standard_EXPORT void Perror();
  
  //! Returns error number if 'Failed' is TRUE.
  Standard_EXPORT Standard_Integer Error() const;




protected:





private:



  TCollection_AsciiString DiskName;
  Standard_Integer myQuotaSize;
  OSD_Error myError;


};







#endif // _OSD_Disk_HeaderFile
