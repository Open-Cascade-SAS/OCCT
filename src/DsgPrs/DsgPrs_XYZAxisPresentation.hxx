// Created on: 1997-02-10
// Created by: Odile Olivier
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _DsgPrs_XYZAxisPresentation_HeaderFile
#define _DsgPrs_XYZAxisPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_CString.hxx>
class Prs3d_Presentation;
class Prs3d_LineAspect;
class gp_Dir;
class gp_Pnt;
class Prs3d_ArrowAspect;
class Prs3d_TextAspect;


//! A framework for displaying the axes of an XYZ trihedron.
class DsgPrs_XYZAxisPresentation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Draws each axis of a trihedron displayed in the
  //! presentation aPresentation and with lines shown by
  //! the values of aLineAspect. Each axis is defined by:
  //! -   the first and last points aPfirst and aPlast
  //! -   the direction aDir and
  //! -   the value aVal which provides a value for length.
  //! The value for length is provided so that the trihedron
  //! can vary in length relative to the scale of shape display.
  //! Each axis will be identified as X, Y, or Z by the text aText.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_LineAspect)& anLineAspect, const gp_Dir& aDir, const Standard_Real aVal, const Standard_CString aText, const gp_Pnt& aPfirst, const gp_Pnt& aPlast);
  
  //! draws the presentation X ,Y ,Z axis
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Prs3d_LineAspect)& aLineAspect, const Handle(Prs3d_ArrowAspect)& anArrowAspect, const Handle(Prs3d_TextAspect)& aTextAspect, const gp_Dir& aDir, const Standard_Real aVal, const Standard_CString aText, const gp_Pnt& aPfirst, const gp_Pnt& aPlast);




protected:





private:





};







#endif // _DsgPrs_XYZAxisPresentation_HeaderFile
