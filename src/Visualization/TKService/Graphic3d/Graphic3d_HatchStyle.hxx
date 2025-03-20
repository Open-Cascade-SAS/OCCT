// Created on: 2016-11-14
// Created by: Varvara POSKONINA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _Graphic3d_HatchStyle_HeaderFile
#define _Graphic3d_HatchStyle_HeaderFile

#include <Aspect_HatchStyle.hxx>
#include <Image_PixMap.hxx>
#include <NCollection_Buffer.hxx>

//! A class that provides an API to use standard OCCT hatch styles
//! defined in Aspect_HatchStyle enum or to create custom styles
//! from a user-defined bitmap
class Graphic3d_HatchStyle : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_HatchStyle, Standard_Transient) // Type definition

public:
  //! Creates a new custom hatch style with the given pattern and unique style id
  //! @warning Raises a program error if given pattern image is not a valid 32*32 bitmap
  Standard_EXPORT Graphic3d_HatchStyle(const Handle(Image_PixMap)& thePattern);

  //! Creates a new predefined hatch style with the given id in Aspect_HatchStyle enum.
  //! GPU memory for the pattern will not be allocated.
  Graphic3d_HatchStyle(const Aspect_HatchStyle theType)
      : myHatchType(theType)
  {
  }

  //! Returns the pattern of custom hatch style
  Standard_EXPORT const Standard_Byte* Pattern() const;

  //! In case if predefined OCCT style is used, returns
  //! index in Aspect_HatchStyle enumeration. If the style
  //! is custom, returns unique index of the style
  Standard_Integer HatchType() const { return myHatchType; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:
  Handle(NCollection_Buffer) myPattern;   //!< Image bitmap with custom hatch pattern
  Standard_Integer           myHatchType; //!< Index of used style
};

DEFINE_STANDARD_HANDLE(Graphic3d_HatchStyle, Standard_Transient)

#endif // _Graphic3d_HatchStyle_HeaderFile
