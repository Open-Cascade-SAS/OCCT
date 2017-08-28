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

#include <inspector/DFBrowserPaneXDE_XCAFDocCentroid.hxx>
#include <inspector/DFBrowserPane_AttributePaneModel.hxx>

#include <XCAFDoc_Centroid.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPaneXDE_XCAFDocCentroid::DFBrowserPaneXDE_XCAFDocCentroid()
: DFBrowserPane_AttributePane()
{
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPaneXDE_XCAFDocCentroid::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(XCAFDoc_Centroid) anAttr = Handle(XCAFDoc_Centroid)::DownCast (theAttribute);
  gp_Pnt aPoint = anAttr->Get();
  theValues << "Get: X" << aPoint.X()
            << "Get: Y" << aPoint.Y()
            << "Get: Z" << aPoint.Z();
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPaneXDE_XCAFDocCentroid::GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                              QList<QVariant>& theValues)
{
  Handle(XCAFDoc_Centroid) anAttr = Handle(XCAFDoc_Centroid)::DownCast (theAttribute);
  gp_Pnt aPoint = anAttr->Get();
  theValues.append (QString ("(%1, %2, %3)").arg (aPoint.X()).arg (aPoint.Y()).arg (aPoint.Z()));
}
