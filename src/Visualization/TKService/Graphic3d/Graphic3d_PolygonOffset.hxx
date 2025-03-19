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

#ifndef _Graphic3d_PolygonOffset_HeaderFile
#define _Graphic3d_PolygonOffset_HeaderFile

#include <Aspect_PolygonOffsetMode.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Integer.hxx>

//! Polygon offset parameters.
struct Graphic3d_PolygonOffset
{
  Aspect_PolygonOffsetMode Mode;
  Standard_ShortReal       Factor;
  Standard_ShortReal       Units;

  //! Empty constructor.
  Graphic3d_PolygonOffset()
      : Mode(Aspect_POM_Fill),
        Factor(1.0f),
        Units(1.0f)
  {
  }

  //! Equality comparison.
  bool operator==(const Graphic3d_PolygonOffset& theOther) const
  {
    return Mode == theOther.Mode && Factor == theOther.Factor && Units == theOther.Units;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;
};

#endif // _Graphic3d_PolygonOffset_HeaderFile
