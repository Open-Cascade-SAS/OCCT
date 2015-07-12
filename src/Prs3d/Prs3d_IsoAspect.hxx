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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Standard_Real.hxx>
class Quantity_Color;


class Prs3d_IsoAspect;
DEFINE_STANDARD_HANDLE(Prs3d_IsoAspect, Prs3d_LineAspect)

//! A framework to define the display attributes of isoparameters.
//! This framework can be used to modify the default
//! setting for isoparameters in Prs3d_Drawer.
class Prs3d_IsoAspect : public Prs3d_LineAspect
{

public:

  
  //! Constructs a framework to define display attributes of isoparameters.
  //! These include:
  //! -   the color attribute aColor
  //! -   the type of line aType
  //! -   the width value aWidth
  //! -   aNumber, the number of isoparameters to be   displayed.
  Standard_EXPORT Prs3d_IsoAspect(const Quantity_NameOfColor aColor, const Aspect_TypeOfLine aType, const Standard_Real aWidth, const Standard_Integer aNumber);
  
  Standard_EXPORT Prs3d_IsoAspect(const Quantity_Color& aColor, const Aspect_TypeOfLine aType, const Standard_Real aWidth, const Standard_Integer aNumber);
  
  //! defines the number of U or V isoparametric curves
  //! to be drawn for a single face.
  //! Default value: 10
  Standard_EXPORT void SetNumber (const Standard_Integer aNumber);
  
  //! returns the number of U or V isoparametric curves drawn for a single face.
  Standard_EXPORT Standard_Integer Number() const;




  DEFINE_STANDARD_RTTI(Prs3d_IsoAspect,Prs3d_LineAspect)

protected:




private:


  Standard_Integer myNumber;


};







#endif // _Prs3d_IsoAspect_HeaderFile
