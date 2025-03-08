// Created on: 1991-10-02
// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Aspect_Background_HeaderFile
#define _Aspect_Background_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Quantity_Color.hxx>

//! This class allows the definition of
//! a window background.
class Aspect_Background
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a window background.
  //! Default color : NOC_MATRAGRAY.
  Standard_EXPORT Aspect_Background();

  //! Creates a window background with the colour <AColor>.
  Standard_EXPORT Aspect_Background(const Quantity_Color& AColor);

  //! Modifies the colour of the window background <me>.
  Standard_EXPORT void SetColor(const Quantity_Color& AColor);

  //! Returns the colour of the window background <me>.
  Standard_EXPORT Quantity_Color Color() const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

protected:
private:
  Quantity_Color MyColor;
};

#endif // _Aspect_Background_HeaderFile
