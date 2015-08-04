// Created on: 2008-04-17
// Created by: Customer Support
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _V3d_LayerMgr_HeaderFile
#define _V3d_LayerMgr_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <V3d_ViewPointer.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Boolean.hxx>
class Visual3d_Layer;
class V3d_ColorScale;
class V3d_ColorScaleLayerItem;
class V3d_View;
class Aspect_ColorScale;


class V3d_LayerMgr;
DEFINE_STANDARD_HANDLE(V3d_LayerMgr, MMgt_TShared)

//! Class to manage layers
class V3d_LayerMgr : public MMgt_TShared
{

public:

  
  Standard_EXPORT V3d_LayerMgr(const Handle(V3d_View)& aView);
  
    const Handle(Visual3d_Layer)& Overlay() const;
  
    Handle(V3d_View) View() const;
  
  //! Recompute layer with objects
  Standard_EXPORT void Compute();
  
  Standard_EXPORT void Resized();




  DEFINE_STANDARD_RTTI(V3d_LayerMgr,MMgt_TShared)

protected:

  
  //! Begin layers recomputation
  Standard_EXPORT virtual Standard_Boolean Begin();
  
  //! Perform layers recomputation
  Standard_EXPORT virtual void Redraw();
  
  //! End layers recomputation
  Standard_EXPORT virtual void End();

  V3d_ViewPointer myView;
  Handle(Visual3d_Layer) myOverlay;


private:




};


#include <V3d_LayerMgr.lxx>





#endif // _V3d_LayerMgr_HeaderFile
