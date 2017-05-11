// Created on: 2014-11-10
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

#ifndef _AIS_TextLabel_HeaderFile
#define _AIS_TextLabel_HeaderFile

#include <AIS_InteractiveObject.hxx>

#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Font_FontAspect.hxx>
#include <TCollection_ExtendedString.hxx>

//! Presentation of the text.
class AIS_TextLabel : public AIS_InteractiveObject
{
public:

  //! Default constructor
  Standard_EXPORT AIS_TextLabel();

  //! Setup color of entire text.
  Standard_EXPORT virtual void SetColor (const Quantity_Color& theColor) Standard_OVERRIDE;

  //! Setup transparency within [0, 1] range.
  Standard_EXPORT virtual void SetTransparency (const Standard_Real theValue) Standard_OVERRIDE;

  //! Removes the transparency setting.
  virtual void UnsetTransparency() Standard_OVERRIDE { SetTransparency (0.0); }

  //! Material has no effect for text label.
  virtual void SetMaterial (const Graphic3d_MaterialAspect& ) Standard_OVERRIDE {}

  //! Setup text.
  Standard_EXPORT void SetText (const TCollection_ExtendedString& theText);

  //! Setup position.
  Standard_EXPORT void SetPosition (const gp_Pnt& thePosition);

  //! Setup horizontal justification.
  Standard_EXPORT void SetHJustification (const Graphic3d_HorizontalTextAlignment theHJust);

  //! Setup vertical justification.
  Standard_EXPORT void SetVJustification (const Graphic3d_VerticalTextAlignment theVJust);

  //! Setup angle.
  Standard_EXPORT void SetAngle (const Standard_Real theAngle);

  //! Setup zoomable property.
  Standard_EXPORT void SetZoomable (const Standard_Boolean theIsZoomable);

  //! Setup height.
  Standard_EXPORT void SetHeight (const Standard_Real theHeight);

  //! Setup font aspect.
  Standard_EXPORT void SetFontAspect (const Font_FontAspect theFontAspect);

  //! Setup font.
  Standard_EXPORT void SetFont (Standard_CString theFont);

  //! Setup label orientation in the model 3D space.
  Standard_EXPORT void SetOrientation3D (const gp_Ax2& theOrientation);

  //! Reset label orientation in the model 3D space.
  Standard_EXPORT void UnsetOrientation3D ();

  //! Returns position.
  Standard_EXPORT const gp_Pnt& Position() const;

  //! Returns label orientation in the model 3D space.
  Standard_EXPORT const gp_Ax2& Orientation3D() const;

  //! Returns true if the current text placement mode uses text orientation in the model 3D space.
  Standard_EXPORT Standard_Boolean HasOrientation3D() const;

  Standard_EXPORT void SetFlipping (const Standard_Boolean theIsFlipping);

  Standard_EXPORT Standard_Boolean HasFlipping() const;

  //! Define the display type of the text.
  //!
  //! TODT_NORMAL     Default display. Text only.
  //! TODT_SUBTITLE   There is a subtitle under the text.
  //! TODT_DEKALE     The text is displayed with a 3D style.
  //! TODT_BLEND      The text is displayed in XOR.
  //! TODT_DIMENSION  Dimension line under text will be invisible.
  Standard_EXPORT void SetDisplayType (const Aspect_TypeOfDisplayText theDisplayType);

  //! Modifies the colour of the subtitle for the TODT_SUBTITLE TextDisplayType
  //! and the colour of backgroubd for the TODT_DEKALE TextDisplayType.
  Standard_EXPORT void SetColorSubTitle (const Quantity_Color& theColor);

private:

  //! Compute
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                        const Handle(Prs3d_Presentation)&           thePresentation,
                                        const Standard_Integer                      theMode) Standard_OVERRIDE;

  //! Compute selection
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                 const Standard_Integer             theMode) Standard_OVERRIDE;

protected:

  TCollection_ExtendedString myText;
  TCollection_AsciiString    myFont;
  Font_FontAspect            myFontAspect;
  gp_Ax2                     myOrientation3D;
  Standard_Boolean           myHasOrientation3D;
  Standard_Boolean           myHasFlipping;

public:

  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(AIS_TextLabel,AIS_InteractiveObject)

};

DEFINE_STANDARD_HANDLE(AIS_TextLabel, AIS_InteractiveObject)

#endif // _AIS_TextLabel_HeaderFile
