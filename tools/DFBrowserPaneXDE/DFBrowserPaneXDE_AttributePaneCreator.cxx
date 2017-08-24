// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/DFBrowserPaneXDE_AttributePaneCreator.hxx>

#include <inspector/DFBrowserPaneXDE_AttributeCommonPane.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocArea.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocCentroid.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocColor.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocColorTool.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocDatum.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocDimension.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocDimTol.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocDimTolTool.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocDocumentTool.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocGeomTolerance.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocGraphNode.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocLayerTool.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocLocation.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocMaterial.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocMaterialTool.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocShapeMapTool.hxx>
#include <inspector/DFBrowserPaneXDE_XCAFDocShapeTool.hxx>

#include <Standard_Version.hxx>

#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_Color.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_Datum.hxx>
#if OCC_VERSION_HEX > 0x060901
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#endif
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_Location.hxx>
#include <XCAFDoc_Material.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPaneXDE_AttributePaneCreator::DFBrowserPaneXDE_AttributePaneCreator(
                                       DFBrowserPane_AttributePaneCreatorAPI* theStandardPaneCreator)
: myStandardPaneCreator (theStandardPaneCreator)
{
}

// =======================================================================
// function : CreateAttributePane
// purpose :
// =======================================================================
DFBrowserPane_AttributePaneAPI* DFBrowserPaneXDE_AttributePaneCreator::CreateAttributePane (const Standard_CString& theAttributeName)
{
  DFBrowserPane_AttributePaneAPI* aPane = 0;
  if (DFBrowserPaneXDE_AttributeCommonPane::ProcessAttribute (theAttributeName))
  {
    DFBrowserPane_AttributePaneAPI* aStandardPane = myStandardPaneCreator->CreateAttributePane (theAttributeName);
    if (!aStandardPane)
      aStandardPane = createXDEPane (theAttributeName);
    aPane = new DFBrowserPaneXDE_AttributeCommonPane (aStandardPane);
  }
  else
   aPane = createXDEPane (theAttributeName);

  return aPane;
}

// =======================================================================
// function : createXDEPane
// purpose :
// =======================================================================
DFBrowserPane_AttributePaneAPI* DFBrowserPaneXDE_AttributePaneCreator::createXDEPane (const Standard_CString& theAttributeName)
{
  DFBrowserPane_AttributePaneAPI* aPane = 0;
  if (theAttributeName == STANDARD_TYPE (XCAFDoc_ShapeMapTool)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocShapeMapTool();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_Area)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocArea();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_Centroid)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocCentroid();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_Color)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocColor();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_ColorTool)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocColorTool();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_Datum)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocDatum();
#if OCC_VERSION_HEX > 0x060901
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_Dimension)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocDimension();
#endif
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_DimTol)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocDimTol();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_DimTolTool)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocDimTolTool();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_DocumentTool)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocDocumentTool();
#if OCC_VERSION_HEX > 0x060901
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_GeomTolerance)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocGeomTolerance();
#endif
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_GraphNode)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocGraphNode();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_LayerTool)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocLayerTool();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_Location)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocLocation();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_Material)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocMaterial();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_MaterialTool)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocMaterialTool();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_ShapeMapTool)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocShapeMapTool();
  else if (theAttributeName == STANDARD_TYPE (XCAFDoc_ShapeTool)->Name())
    aPane = new DFBrowserPaneXDE_XCAFDocShapeTool();
  return aPane;
}
