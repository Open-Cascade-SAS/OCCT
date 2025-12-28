// Created on: 2001-03-06
// Created by: Christian CAILLET
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _IGESSelect_SignColor_HeaderFile
#define _IGESSelect_SignColor_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IFSelect_Signature.hxx>
#include <Standard_CString.hxx>
class Standard_Transient;
class Interface_InterfaceModel;

//! Gives Color attached to an entity
//! Several forms are possible, according to <mode>
//! 1 : number : "Dnn" for entity, "Snn" for standard, "(none)" for 0
//! 2 : name : Of standard color, or of the color entity, or "(none)"
//! (if the color entity has no name, its label is taken)
//! 3 : RGB values, form R:nn,G:nn,B:nn
//! 4 : RED value   : an integer
//! 5 : GREEN value : an integer
//! 6 : BLUE value  : an integer
//! Other computable values can be added if needed :
//! CMY values, Percentages for Hue, Lightness, Saturation
class IGESSelect_SignColor : public IFSelect_Signature
{

public:
  //! Creates a SignColor
  //! mode : see above for the meaning
  //! modes 4,5,6 give a numeric integer value
  //! Name is initialised according to the mode
  Standard_EXPORT IGESSelect_SignColor(const int mode);

  //! Returns the value (see above)
  Standard_EXPORT const char* Value(
    const occ::handle<Standard_Transient>&       ent,
    const occ::handle<Interface_InterfaceModel>& model) const override;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_SignColor, IFSelect_Signature)

private:
  int themode;
};

#endif // _IGESSelect_SignColor_HeaderFile
