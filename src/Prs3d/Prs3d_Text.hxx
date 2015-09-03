// Created on: 1993-09-14
// Created by: Jean-Louis FRENKEL
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

#ifndef _Prs3d_Text_HeaderFile
#define _Prs3d_Text_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_TextAspect.hxx>

#include <gp_Ax2.hxx>

class Prs3d_Presentation;
class TCollection_ExtendedString;
class gp_Pnt;

//! A framework to define the display of texts.
class Prs3d_Text  : public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines the display of the text aText at the point AttachmentPoint.
  //! The drawer aDrawer specifies the display attributes which texts will have.
  //! The presentation object aPresentation stores the
  //! information defined in this framework.
  //! static void Draw (const Handle(Prs3d_Presentation)&
  //! aPresentation, const Handle(Prs3d_TextAspect)&
  //! anAspect, const TCollection_ExtendedString& aText,
  //! const gp_Pnt& AttachmentPoint);
  Standard_EXPORT static void Draw (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_Drawer)& aDrawer, const TCollection_ExtendedString& aText, const gp_Pnt& AttachmentPoint);

  //! Draws theText label with the location and the orientation
  //! specified in the model 3D space through theOrientation argument.
  Standard_EXPORT static void Draw (const Handle(Prs3d_Presentation)& thePresentation,
                                    const Handle(Prs3d_TextAspect)&   theAspect,
                                    const TCollection_ExtendedString& theText,
                                    const gp_Ax2&                     theOrientation);
  
  //! Defines the display of the text aText at the point
  //! AttachmentPoint.
  //! The text aspect object anAspect specifies the display
  //! attributes which texts will have.
  //! The presentation object aPresentation stores the
  //! information defined in this framework.
  //! This syntax could be used if you had not already
  //! defined text display attributes in a drawer or if you
  //! wanted to exceptionally overide the definition
  //! provided in your drawer.
  Standard_EXPORT static void Draw (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_TextAspect)& anAspect, const TCollection_ExtendedString& aText, const gp_Pnt& AttachmentPoint);




protected:





private:





};







#endif // _Prs3d_Text_HeaderFile
