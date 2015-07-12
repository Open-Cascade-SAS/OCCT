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

#ifndef _Visual3d_LayerItem_HeaderFile
#define _Visual3d_LayerItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <MMgt_TShared.hxx>


class Visual3d_LayerItem;
DEFINE_STANDARD_HANDLE(Visual3d_LayerItem, MMgt_TShared)

//! This class is drawable unit of 2d scene
class Visual3d_LayerItem : public MMgt_TShared
{

public:

  
  //! Creates a layer item
  Standard_EXPORT Visual3d_LayerItem();
  
  //! virtual function for recompute 2D
  //! presentation (empty by default)
  Standard_EXPORT virtual void ComputeLayerPrs();
  
  //! virtual function for recompute 2D
  //! presentation (empty by default)
  Standard_EXPORT virtual void RedrawLayerPrs();
  
  Standard_EXPORT Standard_Boolean IsNeedToRecompute() const;
  
  Standard_EXPORT void SetNeedToRecompute (const Standard_Boolean NeedToRecompute = Standard_True);




  DEFINE_STANDARD_RTTI(Visual3d_LayerItem,MMgt_TShared)

protected:




private:


  Standard_Boolean MyIsNeedToRecompute;


};







#endif // _Visual3d_LayerItem_HeaderFile
