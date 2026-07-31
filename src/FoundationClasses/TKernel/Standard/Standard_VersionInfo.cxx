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

#include <Standard_VersionInfo.hxx>

#include <Standard_Version.hxx>

//=================================================================================================

const char* OCCT_DevelopmentVersion()
{
#ifdef OCC_VERSION_DEVELOPMENT
  return OCC_VERSION_DEVELOPMENT;
#else
  return "";
#endif
}

//=================================================================================================

double OCCT_Version_Double()
{
  return OCC_VERSION;
}

//=================================================================================================

const char* OCCT_Version_String()
{
  return OCC_VERSION_STRING;
}

//=================================================================================================

const char* OCCT_Version_String_Complete()
{
  return OCC_VERSION_COMPLETE;
}

//=================================================================================================

const char* OCCT_Version_String_Extended()
{
  return OCC_VERSION_STRING_EXT;
}
