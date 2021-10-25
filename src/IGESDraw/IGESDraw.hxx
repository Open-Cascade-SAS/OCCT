// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen (Niraj RANGWALA)
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESDraw_HeaderFile
#define _IGESDraw_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IGESDraw_Protocol;


//! This package contains the group of classes necessary for
//! Structure Entities implied in Drawings and Structured
//! Graphics (Sets for drawing, Drawings and Views).
class IGESDraw 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Prepares dynamic data (Protocol, Modules) for this package
  Standard_EXPORT static void Init();
  
  //! Returns the Protocol for this Package
  Standard_EXPORT static Handle(IGESDraw_Protocol) Protocol();




protected:





private:




friend class IGESDraw_ConnectPoint;
friend class IGESDraw_NetworkSubfigureDef;
friend class IGESDraw_ViewsVisible;
friend class IGESDraw_ViewsVisibleWithAttr;
friend class IGESDraw_LabelDisplay;
friend class IGESDraw_Planar;
friend class IGESDraw_SegmentedViewsVisible;
friend class IGESDraw_Drawing;
friend class IGESDraw_DrawingWithRotation;
friend class IGESDraw_View;
friend class IGESDraw_RectArraySubfigure;
friend class IGESDraw_CircArraySubfigure;
friend class IGESDraw_NetworkSubfigure;
friend class IGESDraw_PerspectiveView;
friend class IGESDraw_ToolConnectPoint;
friend class IGESDraw_ToolNetworkSubfigureDef;
friend class IGESDraw_ToolViewsVisible;
friend class IGESDraw_ToolViewsVisibleWithAttr;
friend class IGESDraw_ToolLabelDisplay;
friend class IGESDraw_ToolPlanar;
friend class IGESDraw_ToolSegmentedViewsVisible;
friend class IGESDraw_ToolDrawing;
friend class IGESDraw_ToolDrawingWithRotation;
friend class IGESDraw_ToolView;
friend class IGESDraw_ToolRectArraySubfigure;
friend class IGESDraw_ToolCircArraySubfigure;
friend class IGESDraw_ToolNetworkSubfigure;
friend class IGESDraw_ToolPerspectiveView;
friend class IGESDraw_Protocol;
friend class IGESDraw_ReadWriteModule;
friend class IGESDraw_GeneralModule;
friend class IGESDraw_SpecificModule;

};







#endif // _IGESDraw_HeaderFile
