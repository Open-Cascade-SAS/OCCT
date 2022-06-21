// Created on: 2014-11-13
// Created by: Maxim YAKUNIN
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _ShapeFix_FixSmallSolid_HeaderFile
#define _ShapeFix_FixSmallSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <ShapeFix_Root.hxx>
class TopoDS_Shape;
class ShapeBuild_ReShape;


class ShapeFix_FixSmallSolid;
DEFINE_STANDARD_HANDLE(ShapeFix_FixSmallSolid, ShapeFix_Root)

//! Fixing solids with small size
class ShapeFix_FixSmallSolid : public ShapeFix_Root
{

public:

  
  //! Construct
  Standard_EXPORT ShapeFix_FixSmallSolid();
  
  //! Set working mode for operator:
  //! - theMode = 0 use both WidthFactorThreshold and VolumeThreshold parameters
  //! - theMode = 1 use only WidthFactorThreshold parameter
  //! - theMode = 2 use only VolumeThreshold parameter
  Standard_EXPORT void SetFixMode (const Standard_Integer theMode);
  
  //! Set or clear volume threshold for small solids
  Standard_EXPORT void SetVolumeThreshold (const Standard_Real theThreshold = -1.0);
  
  //! Set or clear width factor threshold for small solids
  Standard_EXPORT void SetWidthFactorThreshold (const Standard_Real theThreshold = -1.0);
  
  //! Remove small solids from the given shape
  Standard_EXPORT TopoDS_Shape Remove (const TopoDS_Shape& theShape, const Handle(ShapeBuild_ReShape)& theContext) const;
  
  //! Merge small solids in the given shape to adjacent non-small ones
  Standard_EXPORT TopoDS_Shape Merge (const TopoDS_Shape& theShape, const Handle(ShapeBuild_ReShape)& theContext) const;




  DEFINE_STANDARD_RTTIEXT(ShapeFix_FixSmallSolid,ShapeFix_Root)

protected:




private:

  
  Standard_EXPORT Standard_Boolean IsThresholdsSet() const;
  
  Standard_EXPORT Standard_Boolean IsSmall (const TopoDS_Shape& theSolid) const;
  
  Standard_EXPORT Standard_Boolean IsUsedWidthFactorThreshold() const;
  
  Standard_EXPORT Standard_Boolean IsUsedVolumeThreshold() const;

  Standard_Integer myFixMode;
  Standard_Real myVolumeThreshold;
  Standard_Real myWidthFactorThreshold;


};







#endif // _ShapeFix_FixSmallSolid_HeaderFile
