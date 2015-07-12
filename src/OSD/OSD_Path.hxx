// Created on: 1992-02-18
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

#ifndef _OSD_Path_HeaderFile
#define _OSD_Path_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Boolean.hxx>
#include <OSD_SysType.hxx>
#include <Standard_Integer.hxx>
class Standard_ConstructionError;
class Standard_NullObject;
class OSD_OSDError;
class Standard_NumericError;
class Standard_ProgramError;
class TCollection_AsciiString;



class OSD_Path 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a Path object initialized to an empty string.
  //! i.e. current directory.
  Standard_EXPORT OSD_Path();
  
  //! Creates a Path object initialized by dependant path.
  //! ex: OSD_Path me ("/usr/bin/myprog.sh",OSD_UnixBSD);
  //!
  //! OSD_Path me ("sys$common:[syslib]cc.exe",OSD_OSF) will
  //! raise a ProgramError due to invalid name for this
  //! type of system.
  //! In order to avoid a 'ProgramError' , use IsValid(...)
  //! to ensure you the validity of <aDependentName>.
  //! Raises ConstructionError when the path is either null
  //! or contains characters not in range of ' '...'~'.
  Standard_EXPORT OSD_Path(const TCollection_AsciiString& aDependentName, const OSD_SysType aSysType = OSD_Default);
  
  //! Initializes a system independent path.
  //! By default , the Path conversion will be assumed using
  //! currently used system.
  //! A special syntax is used to specify a "aTrek" in an
  //! independent manner :
  //! a "|" represents directory separator
  //! a "^" means directory above (father)
  //! examples:
  //! "|usr|bin" - On UNIX -> "/usr/bin"
  //! - On VMS  -> "[usr.bin]"
  //! - On MSDOS-> "\usr\bin"
  //! - On MacOs-> ": usr : bin"
  //!
  //! "^|rep"    - On UNIX -> "../rep"
  //! - On VMS  -> "[-.rep]"
  //! - On MSDOS -> "..\rep"
  //! - On MacOS->  ":: rep"
  //!
  //! "subdir|" - On UNIX -> "subdir/"
  //! - On VMS  -> "[.subdir.]"
  Standard_EXPORT OSD_Path(const TCollection_AsciiString& aNode, const TCollection_AsciiString& aUsername, const TCollection_AsciiString& aPassword, const TCollection_AsciiString& aDisk, const TCollection_AsciiString& aTrek, const TCollection_AsciiString& aName, const TCollection_AsciiString& anExtension);
  
  //! Gets each component of a path.
  Standard_EXPORT void Values (TCollection_AsciiString& aNode, TCollection_AsciiString& aUsername, TCollection_AsciiString& aPassword, TCollection_AsciiString& aDisk, TCollection_AsciiString& aTrek, TCollection_AsciiString& aName, TCollection_AsciiString& anExtension) const;
  
  //! Sets each component of a path.
  Standard_EXPORT void SetValues (const TCollection_AsciiString& aNode, const TCollection_AsciiString& aUsername, const TCollection_AsciiString& aPassword, const TCollection_AsciiString& aDisk, const TCollection_AsciiString& aTrek, const TCollection_AsciiString& aName, const TCollection_AsciiString& anExtension);
  
  //! Returns system dependent path
  //! <aType> is one among Unix,VMS ...
  //! This function is not private because you may need to
  //! display system dependent path on a front-end.
  //! It can be useful when communicating with another system.
  //! For instance when you want to communicate between VMS and Unix
  //! to transfer files, or to do a remote procedure call
  //! using files.
  //! example :
  //! OSD_Path myPath ("sparc4", "sga", "secret_passwd",
  //! "$5$dkb100","|users|examples");
  //! Internal ( Dependent_name );
  //! On UNIX  sga"secret_passwd"@sparc4:/users/examples
  //! On VMS   sparc4"sga secret_passwd"::$5$dkb100:[users.examples]
  //! Sets each component of a Path giving its system dependent name.
  Standard_EXPORT void SystemName (TCollection_AsciiString& FullName, const OSD_SysType aType = OSD_Default) const;
  
  //! Returns system dependent path resolving logical symbols.
  Standard_EXPORT void ExpandedName (TCollection_AsciiString& aName);
  
  //! Returns TRUE if <theDependentName> is valid for this SysType.
  Standard_EXPORT static Standard_Boolean IsValid (const TCollection_AsciiString& theDependentName, const OSD_SysType theSysType = OSD_Default);
  
  //! This removes the last directory name in <aTrek>
  //! and returns result.
  //! ex:  me = "|usr|bin|todo.sh"
  //! me.UpTrek() gives me = "|usr|todo.sh"
  //! if <me> contains "|", me.UpTrek() will give again "|"
  //! without any error.
  Standard_EXPORT void UpTrek();
  
  //! This appends a directory name into the Trek.
  //! ex: me = "|usr|todo.sh"
  //! me.DownTrek("bin") gives me = "|usr|bin|todo.sh".
  Standard_EXPORT void DownTrek (const TCollection_AsciiString& aName);
  
  //! Returns number of components in Trek of <me>.
  //! ex: me = "|usr|sys|etc|bin"
  //! me.TrekLength() returns 4.
  Standard_EXPORT Standard_Integer TrekLength() const;
  
  //! This removes a component of Trek in <me> at position <where>.
  //! The first component of Trek is numbered 1.
  //! ex:   me = "|usr|bin|"
  //! me.RemoveATrek(1) gives me = "|bin|"
  //! To avoid a 'NumericError' because of a bad <where>, use
  //! TrekLength() to know number of components of Trek in <me>.
  Standard_EXPORT void RemoveATrek (const Standard_Integer where);
  
  //! This removes <aName> from <me> in Trek.
  //! No error is raised if <aName> is not in <me>.
  //! ex:  me = "|usr|sys|etc|doc"
  //! me.RemoveATrek("sys") gives me = "|usr|etc|doc".
  Standard_EXPORT void RemoveATrek (const TCollection_AsciiString& aName);
  
  //! Returns component of Trek in <me> at position <where>.
  //! ex:  me = "|usr|bin|sys|"
  //! me.TrekValue(2) returns "bin"
  Standard_EXPORT TCollection_AsciiString TrekValue (const Standard_Integer where) const;
  
  //! This inserts <aName> at position <where> into Trek of <me>.
  //! ex:  me = "|usr|etc|"
  //! me.InsertATrek("sys",2) gives me = "|usr|sys|etc"
  Standard_EXPORT void InsertATrek (const TCollection_AsciiString& aName, const Standard_Integer where);
  
  //! Returns Node of <me>.
  Standard_EXPORT TCollection_AsciiString Node() const;
  
  //! Returns UserName of <me>.
  Standard_EXPORT TCollection_AsciiString UserName() const;
  
  //! Returns Password of <me>.
  Standard_EXPORT TCollection_AsciiString Password() const;
  
  //! Returns Disk of <me>.
  Standard_EXPORT TCollection_AsciiString Disk() const;
  
  //! Returns Trek of <me>.
  Standard_EXPORT TCollection_AsciiString Trek() const;
  
  //! Returns file name of <me>.
  //! If <me> hasn't been initialized, it returns an empty AsciiString.
  Standard_EXPORT TCollection_AsciiString Name() const;
  
  //! Returns my extension name.
  //! This returns an empty string if path contains no file name.
  Standard_EXPORT TCollection_AsciiString Extension() const;
  
  //! Sets Node of <me>.
  Standard_EXPORT void SetNode (const TCollection_AsciiString& aName);
  
  //! Sets UserName of <me>.
  Standard_EXPORT void SetUserName (const TCollection_AsciiString& aName);
  
  //! Sets Password of <me>.
  Standard_EXPORT void SetPassword (const TCollection_AsciiString& aName);
  
  //! Sets Disk of <me>.
  Standard_EXPORT void SetDisk (const TCollection_AsciiString& aName);
  
  //! Sets Trek of <me>.
  Standard_EXPORT void SetTrek (const TCollection_AsciiString& aName);
  
  //! Sets file name of <me>.
  //! If <me> hasn't been initialized, it returns an empty AsciiString.
  Standard_EXPORT void SetName (const TCollection_AsciiString& aName);
  
  //! Sets my extension name.
  Standard_EXPORT void SetExtension (const TCollection_AsciiString& aName);
  
  //! Finds the full path of an executable file, like the
  //! "which" Unix utility. Uses the path environment variable.
  //! Returns False if executable file not found.
  Standard_EXPORT Standard_Boolean LocateExecFile (OSD_Path& aPath);
  
  //! Returns the relative file path between the absolute directory
  //! path <DirPath>  and the absolute file path <AbsFilePath>.
  //! If <DirPath> starts with "/", pathes are handled as
  //! on Unix, if it starts with a letter followed by ":", as on
  //! WNT. In particular on WNT directory names are not key sensitive.
  //! If handling fails, an empty string is returned.
  Standard_EXPORT static TCollection_AsciiString RelativePath (const TCollection_AsciiString& DirPath, const TCollection_AsciiString& AbsFilePath);
  
  //! Returns the absolute file path from the absolute directory path
  //! <DirPath> and the relative file path returned by RelativePath().
  //! If the RelFilePath is an absolute path, it is returned and the
  //! directory path is ignored.
  //! If handling fails, an empty string is returned.
  Standard_EXPORT static TCollection_AsciiString AbsolutePath (const TCollection_AsciiString& DirPath, const TCollection_AsciiString& RelFilePath);




protected:





private:



  TCollection_AsciiString myNode;
  TCollection_AsciiString myUserName;
  TCollection_AsciiString myPassword;
  TCollection_AsciiString myDisk;
  TCollection_AsciiString myTrek;
  TCollection_AsciiString myName;
  TCollection_AsciiString myExtension;
  Standard_Boolean myUNCFlag;
  OSD_SysType mySysDep;


};







#endif // _OSD_Path_HeaderFile
