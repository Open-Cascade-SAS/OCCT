// Created on: 2013-01-28
// Created by: Kirill GAVRILOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _Font_FTLibrary_H__
#define _Font_FTLibrary_H__

#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>
#include <Handle_Standard_Transient.hxx>

// inclusion template for FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

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
  bool IsValid() const
  {
    return myFTLib != NULL;
  }

  //! Access FT_Library instance.
  FT_Library Instance() const
  {
    return myFTLib;
  }

private:

  FT_Library myFTLib;

private:

  Font_FTLibrary            (const Font_FTLibrary& );
  Font_FTLibrary& operator= (const Font_FTLibrary& );

public:

  DEFINE_STANDARD_RTTI(Font_FTLibrary) // Type definition

};

DEFINE_STANDARD_HANDLE(Font_FTLibrary, Standard_Transient)

#endif // _Font_FTLibrary_H__
