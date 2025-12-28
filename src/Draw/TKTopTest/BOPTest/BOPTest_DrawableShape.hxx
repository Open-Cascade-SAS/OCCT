// Created on: 2000-05-25
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _BOPTest_DrawableShape_HeaderFile
#define _BOPTest_DrawableShape_HeaderFile

#include <Standard.hxx>

#include <DBRep_DrawableShape.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class Draw_Text3D;
class TopoDS_Shape;
class gp_Pnt;
class Draw_Display;

class BOPTest_DrawableShape : public DBRep_DrawableShape
{

public:
  Standard_EXPORT BOPTest_DrawableShape(const TopoDS_Shape& S,
                                        const Draw_Color&   FreeCol,
                                        const Draw_Color&   ConnCol,
                                        const Draw_Color&   EdgeCol,
                                        const Draw_Color&   IsosCol,
                                        const double        size,
                                        const int           nbisos,
                                        const int           discret,
                                        const char*         Text,
                                        const Draw_Color&   TextColor);

  Standard_EXPORT BOPTest_DrawableShape(const TopoDS_Shape& S,
                                        const char*         Text,
                                        const Draw_Color&   TextColor);

  Standard_EXPORT void DrawOn(Draw_Display& dis) const override;

  DEFINE_STANDARD_RTTIEXT(BOPTest_DrawableShape, DBRep_DrawableShape)

private:
  Standard_EXPORT gp_Pnt Pnt() const;

  occ::handle<Draw_Text3D> myText;
  Draw_Color               myTextColor;
};

#endif // _BOPTest_DrawableShape_HeaderFile
