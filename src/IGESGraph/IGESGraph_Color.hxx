// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Niraj RANGWALA )
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

#ifndef _IGESGraph_Color_HeaderFile
#define _IGESGraph_Color_HeaderFile

#include <Standard.hxx>

#include <Standard_Real.hxx>
#include <IGESData_ColorEntity.hxx>
class TCollection_HAsciiString;


class IGESGraph_Color;
DEFINE_STANDARD_HANDLE(IGESGraph_Color, IGESData_ColorEntity)

//! defines IGESColor, Type <314> Form <0>
//! in package IGESGraph
//!
//! The Color Definition Entity is used to communicate the
//! relationship of primary colors to the intensity level of
//! the respective graphics devices as a percent of full
//! intensity range.
class IGESGraph_Color : public IGESData_ColorEntity
{

public:

  
  Standard_EXPORT IGESGraph_Color();
  
  //! This method is used to set the fields of the class Color
  //! - red        : Red   color intensity (range 0.0 to 100.0)
  //! - green      : Green color intensity (range 0.0 to 100.0)
  //! - blue       : Blue  color intensity (range 0.0 to 100.0)
  //! - aColorName : Name of the color (optional)
  Standard_EXPORT void Init (const Standard_Real red, const Standard_Real green, const Standard_Real blue, const Handle(TCollection_HAsciiString)& aColorName);
  
  Standard_EXPORT void RGBIntensity (Standard_Real& Red, Standard_Real& Green, Standard_Real& Blue) const;
  
  Standard_EXPORT void CMYIntensity (Standard_Real& Cyan, Standard_Real& Magenta, Standard_Real& Yellow) const;
  
  Standard_EXPORT void HLSPercentage (Standard_Real& Hue, Standard_Real& Lightness, Standard_Real& Saturation) const;
  
  //! returns True if optional character string is assigned,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean HasColorName() const;
  
  //! if HasColorName() is True  returns the Verbal description of
  //! the Color.
  Standard_EXPORT Handle(TCollection_HAsciiString) ColorName() const;




  DEFINE_STANDARD_RTTIEXT(IGESGraph_Color,IGESData_ColorEntity)

protected:




private:


  Standard_Real theRed;
  Standard_Real theGreen;
  Standard_Real theBlue;
  Handle(TCollection_HAsciiString) theColorName;


};







#endif // _IGESGraph_Color_HeaderFile
