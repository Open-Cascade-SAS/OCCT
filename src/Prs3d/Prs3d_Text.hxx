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

#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_TextAspect.hxx>

class TCollection_ExtendedString;
class gp_Pnt;

//! A framework to define the display of texts.
class Prs3d_Text
{
public:

  DEFINE_STANDARD_ALLOC

  //! Defines the display of the text.
  //! @param theGroup  group to add primitives
  //! @param theAspect presentation attributes
  //! @param theText   text to draw
  //! @param theAttachmentPoint attachment point
  //! @return text to draw
  Standard_EXPORT static Handle(Graphic3d_Text) Draw (const Handle(Graphic3d_Group)& theGroup,
                                                      const Handle(Prs3d_TextAspect)& theAspect,
                                                      const TCollection_ExtendedString& theText,
                                                      const gp_Pnt& theAttachmentPoint);

  //! Draws the text label.
  //! @param theGroup       group to add primitives
  //! @param theAspect      presentation attributes
  //! @param theText        text to draw
  //! @param theOrientation location and orientation specified in the model 3D space
  //! @param theHasOwnAnchor 
  //! @return text to draw
  Standard_EXPORT static Handle(Graphic3d_Text) Draw (const Handle(Graphic3d_Group)&    theGroup,
                                                      const Handle(Prs3d_TextAspect)&   theAspect,
                                                      const TCollection_ExtendedString& theText,
                                                      const gp_Ax2&                     theOrientation,
                                                      const Standard_Boolean            theHasOwnAnchor = Standard_True);

};

#endif // _Prs3d_Text_HeaderFile
