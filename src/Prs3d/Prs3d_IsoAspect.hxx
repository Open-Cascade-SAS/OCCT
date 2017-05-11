// Created on: 1993-04-26
// Created by: Jean-Louis Frenkel
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

#ifndef _Prs3d_IsoAspect_HeaderFile
#define _Prs3d_IsoAspect_HeaderFile

#include <Prs3d_LineAspect.hxx>

//! A framework to define the display attributes of isoparameters.
//! This framework can be used to modify the default
//! setting for isoparameters in Prs3d_Drawer.
class Prs3d_IsoAspect : public Prs3d_LineAspect
{
  DEFINE_STANDARD_RTTIEXT(Prs3d_IsoAspect, Prs3d_LineAspect)
public:

  //! Constructs a framework to define display attributes of isoparameters.
  //! These include:
  //! -   the color attribute aColor
  //! -   the type of line aType
  //! -   the width value aWidth
  //! -   aNumber, the number of isoparameters to be   displayed.
  Prs3d_IsoAspect (const Quantity_Color& theColor,
                   const Aspect_TypeOfLine theType,
                   const Standard_Real theWidth,
                   const Standard_Integer theNumber)
  : Prs3d_LineAspect (theColor, theType, theWidth),
    myNumber (theNumber) {}   

  //! defines the number of U or V isoparametric curves
  //! to be drawn for a single face.
  //! Default value: 10
  void SetNumber (const Standard_Integer theNumber) { myNumber = theNumber; }

  //! returns the number of U or V isoparametric curves drawn for a single face.
  Standard_Integer Number() const { return myNumber; }

protected:

  Standard_Integer myNumber;

};

DEFINE_STANDARD_HANDLE(Prs3d_IsoAspect, Prs3d_LineAspect)

#endif // _Prs3d_IsoAspect_HeaderFile
