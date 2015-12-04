// Created on: 1991-11-04
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

#ifndef _Aspect_AspectFillArea_HeaderFile
#define _Aspect_AspectFillArea_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Aspect_InteriorStyle.hxx>
#include <Quantity_Color.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Standard_Real.hxx>
#include <Aspect_HatchStyle.hxx>
#include <MMgt_TShared.hxx>
class Aspect_AspectFillAreaDefinitionError;
class Quantity_Color;


class Aspect_AspectFillArea;
DEFINE_STANDARD_HANDLE(Aspect_AspectFillArea, MMgt_TShared)

//! Group of attributes for the FACE primitives.
//! The attributes are:
//! * type of interior
//! * type of hatch
//! * interior colour
//! * border colour
//! * type of border
//! * thickness of border
//! when the value of the group is modified, all graphic
//! objects using this group are modified.
class Aspect_AspectFillArea : public MMgt_TShared
{

public:

  
  //! Modifies the colour of the edge of the face
  Standard_EXPORT void SetEdgeColor (const Quantity_Color& AColor);
  
  //! Modifies the edge line type
  Standard_EXPORT void SetEdgeLineType (const Aspect_TypeOfLine AType);
  
  //! Modifies the edge thickness
  //!
  //! Category: Methods to modify the class definition
  //!
  //! Warning: Raises AspectFillAreaDefinitionError if the
  //! width is a negative value.
  Standard_EXPORT void SetEdgeWidth (const Standard_Real AWidth);
  
  //! Modifies the hatch type used when InteriorStyle
  //! is IS_HATCH
  Standard_EXPORT void SetHatchStyle (const Aspect_HatchStyle AStyle);
  
  //! Modifies the colour of the interior of the face
  Standard_EXPORT void SetInteriorColor (const Quantity_Color& AColor);
  
  //! Modifies the colour of the interior of the back face
  Standard_EXPORT void SetBackInteriorColor (const Quantity_Color& color);
  
  //! Modifies the interior type used for rendering
  //!
  //! InteriorStyle : IS_EMPTY	no interior
  //! IS_HOLLOW	display the boundaries of the surface
  //! IS_HATCH	display hatching
  //! IS_SOLID	display interior entirely filled
  Standard_EXPORT void SetInteriorStyle (const Aspect_InteriorStyle AStyle);
  
  //! Returns the hatch type used when InteriorStyle
  //! is IS_HATCH
  Standard_EXPORT Aspect_HatchStyle HatchStyle() const;
  
  Standard_EXPORT void Values (Aspect_InteriorStyle& AStyle, Quantity_Color& AIntColor, Quantity_Color& AEdgeColor, Aspect_TypeOfLine& AType, Standard_Real& AWidth) const;
  
  //! Returns the current values of the <me> group.
  Standard_EXPORT void Values (Aspect_InteriorStyle& AStyle, Quantity_Color& AIntColor, Quantity_Color& BackIntColor, Quantity_Color& AEdgeColor, Aspect_TypeOfLine& AType, Standard_Real& AWidth) const;




  DEFINE_STANDARD_RTTIEXT(Aspect_AspectFillArea,MMgt_TShared)

protected:

  
  //! Initialise the constructor
  //! of Graphic3d_AspectFillArea3d.
  //!
  //! default values :
  //!
  //! InteriorStyle	= Aspect_IS_EMPTY;
  //! InteriorColor	= Quantity_NOC_CYAN1;
  //! EdgeColor	= Quantity_NOC_WHITE;
  //! EdgeType	= Aspect_TOL_SOLID;
  //! EdgeWidth	= 1.0;
  //! HatchStyle	= Aspect_HS_VERTICAL;
  Standard_EXPORT Aspect_AspectFillArea();
  
  //! Initialise the values for the constructor of
  //! Graphic3d_AspectFillArea3d.
  //!
  //! InteriorStyle :
  //! IS_EMPTY	no interior.
  //! IS_HOLLOW	display the boundaries of the surface.
  //! IS_HATCH	display hatched with a hatch style.
  //! IS_SOLID	display the interior entirely filled.
  //!
  //! EdgeLineType :
  //! TOL_SOLID	continuous
  //! TOL_DASH	dashed
  //! TOL_DOT	dotted
  //! TOL_DOTDASH	mixed
  //!
  //! default values :
  //! HatchStyle	= Aspect_HS_VERTICAL;
  //!
  //! Warning: Raises AspectFillAreaDefinitionError if the
  //! width is a negative value.
  Standard_EXPORT Aspect_AspectFillArea(const Aspect_InteriorStyle InteriorStyle, const Quantity_Color& InteriorColor, const Quantity_Color& EdgeColor, const Aspect_TypeOfLine EdgeLineType, const Standard_Real EdgeLineWidth);



private:


  Aspect_InteriorStyle MyInteriorStyle;
  Quantity_Color MyInteriorColor;
  Quantity_Color MyBackInteriorColor;
  Quantity_Color MyEdgeColor;
  Aspect_TypeOfLine MyEdgeType;
  Standard_Real MyEdgeWidth;
  Aspect_HatchStyle MyHatchStyle;


};







#endif // _Aspect_AspectFillArea_HeaderFile
