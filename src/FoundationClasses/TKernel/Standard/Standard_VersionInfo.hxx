// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _Standard_VersionInfo_HeaderFile
#define _Standard_VersionInfo_HeaderFile

#include <Standard_Macro.hxx>

#ifdef __cplusplus
extern "C"
{
#endif
  //! Returns development version of Open CASCADE Technology.
  //! "" - in case of official release,
  //! "dev" - in case of development version between releases,
  //! "beta..." or "rc..." - in case of beta releases or release candidates,
  //! "project..." - in case of version containing project-specific fixes.
  Standard_EXPORT const char* OCCT_DevelopmentVersion();

  //! Returns version of Open CASCADE Technology as a double "major.minor"
  Standard_EXPORT double OCCT_Version_Double();

  //! Returns version of Open CASCADE Technology as a string "major.minor"
  Standard_EXPORT const char* OCCT_Version_String();

  //! Returns complete version of Open CASCADE Technology as a string "major.minor.maintenance"
  Standard_EXPORT const char* OCCT_Version_String_Complete();

  //! Returns extended version of Open CASCADE Technology as a string
  //! "major.minor.maintenance.devext". In case if no development version is defined, returns the
  //! same as OCCT_Version_String_Complete().
  Standard_EXPORT const char* OCCT_Version_String_Extended();

#ifdef __cplusplus
}
#endif

#endif /* _Standard_VersionInfo_HeaderFile */
