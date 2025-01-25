// Created on: 2013-01-28
// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef Font_FTLibrary_HeaderFile
#define Font_FTLibrary_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

// forward declarations to avoid including of FreeType headers
typedef struct FT_LibraryRec_* FT_Library;

//! Wrapper over FT_Library. Provides access to FreeType library.
class Font_FTLibrary : public Standard_Transient
{

public:
  //! Initialize new FT_Library instance.
  Standard_EXPORT Font_FTLibrary();

  //! Release FT_Library instance.
  Standard_EXPORT ~Font_FTLibrary();

  //! This method should always return true.
  //! @return true if FT_Library instance is valid.
  bool IsValid() const { return myFTLib != NULL; }

  //! Access FT_Library instance.
  FT_Library Instance() const { return myFTLib; }

private:
  FT_Library myFTLib;

private:
  Font_FTLibrary(const Font_FTLibrary&);
  Font_FTLibrary& operator=(const Font_FTLibrary&);

public:
  DEFINE_STANDARD_RTTIEXT(Font_FTLibrary, Standard_Transient) // Type definition
};

DEFINE_STANDARD_HANDLE(Font_FTLibrary, Standard_Transient)

#endif // _Font_FTLibrary_H__
