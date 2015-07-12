// Created on: 2009-03-20
// Created by: ABD
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _V3d_ColorScaleLayerItem_HeaderFile
#define _V3d_ColorScaleLayerItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Visual3d_LayerItem.hxx>
class V3d_ColorScale;


class V3d_ColorScaleLayerItem;
DEFINE_STANDARD_HANDLE(V3d_ColorScaleLayerItem, Visual3d_LayerItem)

//! This class is drawable unit of ColorScale of 2d scene
class V3d_ColorScaleLayerItem : public Visual3d_LayerItem
{

public:

  
  //! Creates a layer item
  Standard_EXPORT V3d_ColorScaleLayerItem(const Handle(V3d_ColorScale)& aColorScale);
  
  //! virtual function for recompute 2D
  //! presentation (empty by default)
  Standard_EXPORT virtual void ComputeLayerPrs() Standard_OVERRIDE;
  
  //! virtual function for recompute 2D
  //! presentation (empty by default)
  Standard_EXPORT virtual void RedrawLayerPrs() Standard_OVERRIDE;




  DEFINE_STANDARD_RTTI(V3d_ColorScaleLayerItem,Visual3d_LayerItem)

protected:




private:


  Handle(V3d_ColorScale) MyColorScale;


};







#endif // _V3d_ColorScaleLayerItem_HeaderFile
