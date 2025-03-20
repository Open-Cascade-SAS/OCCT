// Created on: 1997-02-10
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _AIS_GraphicTool_HeaderFile
#define _AIS_GraphicTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Drawer.hxx>
#include <AIS_TypeOfAttribute.hxx>
#include <Standard_Real.hxx>
#include <Aspect_TypeOfLine.hxx>
class Quantity_Color;
class Graphic3d_MaterialAspect;

class AIS_GraphicTool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static Quantity_NameOfColor GetLineColor(
    const Handle(Prs3d_Drawer)& aDrawer,
    const AIS_TypeOfAttribute   TheTypeOfAttributes);

  Standard_EXPORT static void GetLineColor(const Handle(Prs3d_Drawer)& aDrawer,
                                           const AIS_TypeOfAttribute   TheTypeOfAttributes,
                                           Quantity_Color&             TheLineColor);

  Standard_EXPORT static Standard_Real GetLineWidth(const Handle(Prs3d_Drawer)& aDrawer,
                                                    const AIS_TypeOfAttribute TheTypeOfAttributes);

  Standard_EXPORT static Aspect_TypeOfLine GetLineType(
    const Handle(Prs3d_Drawer)& aDrawer,
    const AIS_TypeOfAttribute   TheTypeOfAttributes);

  Standard_EXPORT static void GetLineAtt(const Handle(Prs3d_Drawer)& aDrawer,
                                         const AIS_TypeOfAttribute   TheTypeOfAttributes,
                                         Quantity_NameOfColor&       aCol,
                                         Standard_Real&              aWidth,
                                         Aspect_TypeOfLine&          aTyp);

  Standard_EXPORT static Quantity_NameOfColor GetInteriorColor(const Handle(Prs3d_Drawer)& aDrawer);

  Standard_EXPORT static void GetInteriorColor(const Handle(Prs3d_Drawer)& aDrawer,
                                               Quantity_Color&             aColor);

  Standard_EXPORT static Graphic3d_MaterialAspect GetMaterial(const Handle(Prs3d_Drawer)& aDrawer);

protected:
private:
};

#endif // _AIS_GraphicTool_HeaderFile
