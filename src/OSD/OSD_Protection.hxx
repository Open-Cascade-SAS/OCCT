// Created on: 2018-03-15
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

#ifndef _OSD_Protection_HeaderFile
#define _OSD_Protection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <OSD_SingleProtection.hxx>
#include <Standard_Integer.hxx>
class OSD_FileNode;
class OSD_File;
class OSD_Directory;


//! This  class provides data to manage file protection
//! Example:These rights are treated in a system dependent manner :
//! On UNIX you have User,Group and Other rights
//! On VMS  you have Owner,Group,World and System rights
//! An automatic conversion is done between OSD and UNIX/VMS.
//!
//! OSD	VMS	UNIX
//! User     Owner   User
//! Group    Group   Group
//! World    World   Other
//! System   System  (combined with Other)
//!
//! When you use System protection on UNIX you must know that
//! Other rights and System rights are inclusively "ORed".
//! So Other with only READ access and System with WRITE access
//! will produce on UNIX Other with READ and WRITE access.
//!
//! This choice comes from the fact that ROOT can't be considered
//! as member of the group nor as user. So it is considered as Other.
class OSD_Protection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes global access rights as follows
  //!
  //! User   : Read Write
  //! System : Read Write
  //! Group  : Read
  //! World  : Read
  Standard_EXPORT OSD_Protection();
  
  //! Sets values of fields
  Standard_EXPORT OSD_Protection(const OSD_SingleProtection System, const OSD_SingleProtection User, const OSD_SingleProtection Group, const OSD_SingleProtection World);
  
  //! Retrieves values of fields
  Standard_EXPORT void Values (OSD_SingleProtection& System, OSD_SingleProtection& User, OSD_SingleProtection& Group, OSD_SingleProtection& World);
  
  //! Sets values of fields
  Standard_EXPORT void SetValues (const OSD_SingleProtection System, const OSD_SingleProtection User, const OSD_SingleProtection Group, const OSD_SingleProtection World);
  
  //! Sets protection of 'System'
  Standard_EXPORT void SetSystem (const OSD_SingleProtection priv);
  
  //! Sets protection of 'User'
  Standard_EXPORT void SetUser (const OSD_SingleProtection priv);
  
  //! Sets protection of 'Group'
  Standard_EXPORT void SetGroup (const OSD_SingleProtection priv);
  
  //! Sets protection of 'World'
  Standard_EXPORT void SetWorld (const OSD_SingleProtection priv);
  
  //! Gets protection of 'System'
  Standard_EXPORT OSD_SingleProtection System() const;
  
  //! Gets protection of 'User'
  Standard_EXPORT OSD_SingleProtection User() const;
  
  //! Gets protection of 'Group'
  Standard_EXPORT OSD_SingleProtection Group() const;
  
  //! Gets protection of 'World'
  Standard_EXPORT OSD_SingleProtection World() const;
  
  //! Add a right to a single protection.
  //! ex: aProt = RWD
  //! me.Add(aProt,X)  ->  aProt = RWXD
  Standard_EXPORT void Add (OSD_SingleProtection& aProt, const OSD_SingleProtection aRight);
  
  //! Subtract a right to a single protection.
  //! ex: aProt = RWD
  //! me.Sub(aProt,RW) ->  aProt = D
  //! But me.Sub(aProt,RWX) is also valid and gives same result.
  Standard_EXPORT void Sub (OSD_SingleProtection& aProt, const OSD_SingleProtection aRight);


friend class OSD_FileNode;
friend class OSD_File;
friend class OSD_Directory;


protected:





private:

  
  //! Returns System dependent access rights
  //! this is a private method.
  Standard_EXPORT Standard_Integer Internal() const;


  OSD_SingleProtection s;
  OSD_SingleProtection u;
  OSD_SingleProtection g;
  OSD_SingleProtection w;


};







#endif // _OSD_Protection_HeaderFile
