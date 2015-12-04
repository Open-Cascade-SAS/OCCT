// Created on: 1993-06-11
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

#ifndef _Prs3d_ArrowAspect_HeaderFile
#define _Prs3d_ArrowAspect_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Quantity_PlaneAngle.hxx>
#include <Quantity_Length.hxx>
#include <Prs3d_BasicAspect.hxx>
#include <Quantity_NameOfColor.hxx>
class Graphic3d_AspectLine3d;
class Prs3d_InvalidAngle;
class Quantity_Color;


class Prs3d_ArrowAspect;
DEFINE_STANDARD_HANDLE(Prs3d_ArrowAspect, Prs3d_BasicAspect)

//! A framework for displaying arrows in representations
//! of dimensions and relations.
class Prs3d_ArrowAspect : public Prs3d_BasicAspect
{

public:

  
  //! Constructs an empty framework for displaying arrows
  //! in representations of lengths. The lengths displayed
  //! are either on their own or in chamfers, fillets,
  //! diameters and radii.
  Standard_EXPORT Prs3d_ArrowAspect();
  
  //! Constructs a framework to display an arrow with a
  //! shaft of the length aLength and having a head with
  //! sides at the angle anAngle from each other.
  Standard_EXPORT Prs3d_ArrowAspect(const Quantity_PlaneAngle anAngle, const Quantity_Length aLength);
  
  Standard_EXPORT Prs3d_ArrowAspect(const Handle(Graphic3d_AspectLine3d)& theAspect);
  
  //! defines the angle of the arrows.
  Standard_EXPORT void SetAngle (const Quantity_PlaneAngle anAngle);
  
  //! returns the current value of the angle used when drawing an arrow.
  Standard_EXPORT Quantity_PlaneAngle Angle() const;
  
  //! defines the length of the arrows.
  Standard_EXPORT void SetLength (const Quantity_Length aLength);
  
  //! returns the current value of the length used when drawing an arrow.
  Standard_EXPORT Quantity_Length Length() const;
  
  Standard_EXPORT void SetColor (const Quantity_Color& aColor);
  
  Standard_EXPORT void SetColor (const Quantity_NameOfColor aColor);
  
  Standard_EXPORT Handle(Graphic3d_AspectLine3d) Aspect() const;
  
  Standard_EXPORT void SetAspect (const Handle(Graphic3d_AspectLine3d)& theAspect);




  DEFINE_STANDARD_RTTIEXT(Prs3d_ArrowAspect,Prs3d_BasicAspect)

protected:




private:


  Handle(Graphic3d_AspectLine3d) myArrowAspect;
  Quantity_PlaneAngle myAngle;
  Quantity_Length myLength;


};







#endif // _Prs3d_ArrowAspect_HeaderFile
