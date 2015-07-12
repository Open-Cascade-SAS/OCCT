// Created on: 2004-06-24
// Created by: STV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _V3d_ColorScale_HeaderFile
#define _V3d_ColorScale_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <V3d_LayerMgrPointer.hxx>
#include <Standard_Boolean.hxx>
#include <Aspect_ColorScale.hxx>
#include <Standard_Integer.hxx>
class V3d_LayerMgr;
class Quantity_Color;
class TCollection_ExtendedString;


class V3d_ColorScale;
DEFINE_STANDARD_HANDLE(V3d_ColorScale, Aspect_ColorScale)

//! A colorscale class
class V3d_ColorScale : public Aspect_ColorScale
{

public:

  
  //! Returns returns ColorScale from V3d.
  //! Returns View from V3d.
  Standard_EXPORT V3d_ColorScale(const Handle(V3d_LayerMgr)& theMgr);
  
  Standard_EXPORT void Display();
  
  Standard_EXPORT void Erase();
  
  Standard_EXPORT Standard_Boolean IsDisplayed() const;
  
  Standard_EXPORT virtual void PaintRect (const Standard_Integer theX, const Standard_Integer theY, const Standard_Integer theWidth, const Standard_Integer theHeight, const Quantity_Color& theColor, const Standard_Boolean theFilled = Standard_False) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void PaintText (const TCollection_ExtendedString& theText, const Standard_Integer theX, const Standard_Integer theY, const Quantity_Color& theColor) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Integer TextWidth (const TCollection_ExtendedString& theText) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Integer TextHeight (const TCollection_ExtendedString& theText) const Standard_OVERRIDE;
  
  Standard_EXPORT void TextSize (const TCollection_ExtendedString& theText, const Standard_Integer theHeight, Standard_Integer& theWidth, Standard_Integer& theAscent, Standard_Integer& theDescent) const;
  
  Standard_EXPORT void DrawScale();




  DEFINE_STANDARD_RTTI(V3d_ColorScale,Aspect_ColorScale)

protected:

  
  Standard_EXPORT virtual void UpdateColorScale() Standard_OVERRIDE;



private:


  V3d_LayerMgrPointer myLayerMgr;
  Standard_Boolean myDisplay;


};







#endif // _V3d_ColorScale_HeaderFile
