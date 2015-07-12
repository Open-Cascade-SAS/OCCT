// Created on: 2010-05-21
// Created by: PAUL SUPRYATKIN
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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

#ifndef _Aspect_GradientBackground_HeaderFile
#define _Aspect_GradientBackground_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Quantity_Color.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <Aspect_Background.hxx>
class Quantity_Color;


//! This class allows the definition of
//! a window gradient background.
class Aspect_GradientBackground  : public Aspect_Background
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a window gradient background.
  //! Default colors : Quantity_NOC_BLACK.
  //! Default fill method : Aspect_GFM_NONE
  Standard_EXPORT Aspect_GradientBackground();
  
  //! Creates a window gradient background with colours <AColor1, AColor2>.
  Standard_EXPORT Aspect_GradientBackground(const Quantity_Color& AColor1, const Quantity_Color& AColor2, const Aspect_GradientFillMethod AMethod = Aspect_GFM_HOR);
  
  //! Modifies the colours of the window gradient background <me>.
  Standard_EXPORT void SetColors (const Quantity_Color& AColor1, const Quantity_Color& AColor2, const Aspect_GradientFillMethod AMethod = Aspect_GFM_HOR);
  
  //! Returns colours of the window gradient background <me>.
  Standard_EXPORT void Colors (Quantity_Color& AColor1, Quantity_Color& AColor2) const;
  
  //! Returns the current gradient background fill mode.
  Standard_EXPORT Aspect_GradientFillMethod BgGradientFillMethod() const;




protected:





private:



  Quantity_Color MyColor2;
  Aspect_GradientFillMethod MyGradientMethod;


};







#endif // _Aspect_GradientBackground_HeaderFile
