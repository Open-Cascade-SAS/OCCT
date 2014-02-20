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

/***********************************************************************


************************************************************************/

// for the class
#include <V3d_ColorScaleLayerItem.ixx>

//
//-Constructors
//
V3d_ColorScaleLayerItem::V3d_ColorScaleLayerItem (
  const Handle(V3d_ColorScale)& aColorScale ):
Visual3d_LayerItem(),
MyColorScale(aColorScale)
{
}

void V3d_ColorScaleLayerItem::ComputeLayerPrs()
{
  Visual3d_LayerItem::ComputeLayerPrs();
}

void V3d_ColorScaleLayerItem::RedrawLayerPrs()
{
  Visual3d_LayerItem::RedrawLayerPrs();
  if ( !MyColorScale.IsNull() )
    MyColorScale->DrawScale();
}
