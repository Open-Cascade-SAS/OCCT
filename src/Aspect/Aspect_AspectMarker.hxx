// Created on: 1991-09-02
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

#ifndef _Aspect_AspectMarker_HeaderFile
#define _Aspect_AspectMarker_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Quantity_Color.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Standard_Real.hxx>
#include <MMgt_TShared.hxx>
class Aspect_AspectMarkerDefinitionError;
class Quantity_Color;


class Aspect_AspectMarker;
DEFINE_STANDARD_HANDLE(Aspect_AspectMarker, MMgt_TShared)

//! This class allows the definition of a group
//! of attributes for the primitive MARKER.
//! the attributes are:
//! * Colour
//! * Type
//! * Scale factor
//! When any value of the group is modified
//! all graphic objects using the group are modified
class Aspect_AspectMarker : public MMgt_TShared
{

public:

  
  //! Modifies the colour of <me>.
  Standard_EXPORT void SetColor (const Quantity_Color& AColor);
  
  //! Modifies the scale factor of <me>.
  //! Marker type Aspect_TOM_POINT is not affected
  //! by the marker size scale factor. It is always
  //! the smallest displayable dot.
  //! Warning: Raises AspectMarkerDefinitionError if the
  //! scale is a negative value.
  Standard_EXPORT void SetScale (const Standard_Real AScale);
  
  //! Modifies the type of marker <me>.
  Standard_EXPORT void SetType (const Aspect_TypeOfMarker AType);
  
  //! Returns the current values of the group <me>.
  Standard_EXPORT void Values (Quantity_Color& AColor, Aspect_TypeOfMarker& AType, Standard_Real& AScale) const;




  DEFINE_STANDARD_RTTIEXT(Aspect_AspectMarker,MMgt_TShared)

protected:

  
  //! Initialise the constructor for Graphic3d_AspectMarker3d.
  //!
  //! defaults values :
  //!
  //! Color	= Quantity_NOC_YELLOW;
  //! Type	= Aspect_TOM_X;
  //! Scale	= 1.0;
  Standard_EXPORT Aspect_AspectMarker();
  
  //! Initialise the values for the
  //! constructor of Graphic3d_AspectMarker3d.
  //! Warning: Raises AspectMarkerDefinitionError if the
  //! scale is a negative value.
  Standard_EXPORT Aspect_AspectMarker(const Quantity_Color& AColor, const Aspect_TypeOfMarker AType, const Standard_Real AScale);



private:


  Quantity_Color MyColor;
  Aspect_TypeOfMarker MyType;
  Standard_Real MyScale;


};







#endif // _Aspect_AspectMarker_HeaderFile
