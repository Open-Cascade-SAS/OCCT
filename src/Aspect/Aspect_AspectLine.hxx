// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Aspect_AspectLine_HeaderFile
#define _Aspect_AspectLine_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Quantity_Color.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Standard_Real.hxx>
#include <MMgt_TShared.hxx>
class Aspect_AspectLineDefinitionError;
class Quantity_Color;


class Aspect_AspectLine;
DEFINE_STANDARD_HANDLE(Aspect_AspectLine, MMgt_TShared)

//! This class allows the definition of a group
//! of attributes for the LINE primitive
//! The attributes are:
//! * Colour
//! * Type
//! * Thickness
//! When any value of the group is modified
//! all graphic objects using the group are modified.
class Aspect_AspectLine : public MMgt_TShared
{

public:

  
  //! Modifies the colour of <me>.
  Standard_EXPORT void SetColor (const Quantity_Color& AColor);
  
  //! Modifies the type of <me>.
  Standard_EXPORT void SetType (const Aspect_TypeOfLine AType);
  
  //! Modifies the thickness of <me>.
  //! Category: Methods to modify the class definition
  //! Warning: Raises AspectLineDefinitionError if the
  //! width is a negative value.
  Standard_EXPORT void SetWidth (const Standard_Real AWidth);
  
  //! Returns the current values of the group <me>.
  Standard_EXPORT void Values (Quantity_Color& AColor, Aspect_TypeOfLine& AType, Standard_Real& AWidth) const;




  DEFINE_STANDARD_RTTIEXT(Aspect_AspectLine,MMgt_TShared)

protected:

  
  //! Initialise the constructor for Graphic3d_AspectLine3d.
  //!
  //! Default values :
  //! Color = Quantity_NOC_YELLOW;
  //! Type  = Aspect_TOL_SOLID;
  //! Width = 1.0;
  Standard_EXPORT Aspect_AspectLine();
  
  //! Initialise the values for the
  //! constructor of Graphic3d_AspectLine3d.
  Standard_EXPORT Aspect_AspectLine(const Quantity_Color& AColor, const Aspect_TypeOfLine AType, const Standard_Real AWidth);



private:


  Quantity_Color MyColor;
  Aspect_TypeOfLine MyType;
  Standard_Real MyWidth;


};







#endif // _Aspect_AspectLine_HeaderFile
