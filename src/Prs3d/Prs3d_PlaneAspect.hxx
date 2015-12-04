// Created on: 1994-01-17
// Created by: Modelistation
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Prs3d_PlaneAspect_HeaderFile
#define _Prs3d_PlaneAspect_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Quantity_Length.hxx>
#include <Quantity_PlaneAngle.hxx>
#include <Standard_Boolean.hxx>
#include <Prs3d_BasicAspect.hxx>
class Prs3d_LineAspect;


class Prs3d_PlaneAspect;
DEFINE_STANDARD_HANDLE(Prs3d_PlaneAspect, Prs3d_BasicAspect)

//! A framework to define the display of planes.
class Prs3d_PlaneAspect : public Prs3d_BasicAspect
{

public:

  
  //! Constructs an empty framework for the display of planes.
  Standard_EXPORT Prs3d_PlaneAspect();
  
  //! Returns the attributes of displayed edges involved in the presentation of planes.
  Standard_EXPORT Handle(Prs3d_LineAspect) EdgesAspect() const;
  
  //! Returns the attributes of displayed isoparameters involved in the presentation of planes.
  Standard_EXPORT Handle(Prs3d_LineAspect) IsoAspect() const;
  
  //! Returns the settings for displaying an arrow.
  Standard_EXPORT Handle(Prs3d_LineAspect) ArrowAspect() const;
  
  Standard_EXPORT void SetArrowsLength (const Quantity_Length L);
  
  //! Returns the length of the arrow shaft used in the display of arrows.
  Standard_EXPORT Quantity_Length ArrowsLength() const;
  
  //! Sets the angle of the arrowhead used in the display of planes.
  Standard_EXPORT void SetArrowsSize (const Quantity_Length L);
  
  //! Returns the size of arrows used in the display of planes.
  Standard_EXPORT Quantity_Length ArrowsSize() const;
  
  //! Sets the angle of the arrowhead used in the display
  //! of arrows involved in the presentation of planes.
  Standard_EXPORT void SetArrowsAngle (const Quantity_PlaneAngle ang);
  
  //! Returns the angle of the arrowhead used in the
  //! display of arrows involved in the presentation of planes.
  Standard_EXPORT Quantity_PlaneAngle ArrowsAngle() const;
  
  //! Sets the display attributes defined in DisplayCenterArrow to active.
  Standard_EXPORT void SetDisplayCenterArrow (const Standard_Boolean draw);
  
  //! Returns true if the display of center arrows is allowed.
  Standard_EXPORT Standard_Boolean DisplayCenterArrow() const;
  
  //! Sets the display attributes defined in DisplayEdgesArrows to active.
  Standard_EXPORT void SetDisplayEdgesArrows (const Standard_Boolean draw);
  
  //! Returns true if the display of edge arrows is allowed.
  Standard_EXPORT Standard_Boolean DisplayEdgesArrows() const;
  
  Standard_EXPORT void SetDisplayEdges (const Standard_Boolean draw);
  
  Standard_EXPORT Standard_Boolean DisplayEdges() const;
  
  //! Sets the display attributes defined in DisplayIso to active.
  Standard_EXPORT void SetDisplayIso (const Standard_Boolean draw);
  
  //! Returns true if the display of isoparameters is allowed.
  Standard_EXPORT Standard_Boolean DisplayIso() const;
  
  Standard_EXPORT void SetPlaneLength (const Quantity_Length LX, const Quantity_Length LY);
  
  //! Returns the length of the x axis used in the display of planes.
  Standard_EXPORT Quantity_Length PlaneXLength() const;
  
  //! Returns the length of the y axis used in the display of planes.
  Standard_EXPORT Quantity_Length PlaneYLength() const;
  
  //! Sets the distance L between isoparameters used in the display of planes.
  Standard_EXPORT void SetIsoDistance (const Quantity_Length L);
  
  //! Returns the distance between isoparameters used in the display of planes.
  Standard_EXPORT Quantity_Length IsoDistance() const;




  DEFINE_STANDARD_RTTIEXT(Prs3d_PlaneAspect,Prs3d_BasicAspect)

protected:




private:


  Handle(Prs3d_LineAspect) myEdgesAspect;
  Handle(Prs3d_LineAspect) myIsoAspect;
  Handle(Prs3d_LineAspect) myArrowAspect;
  Quantity_Length myArrowsLength;
  Quantity_Length myArrowsSize;
  Quantity_PlaneAngle myArrowsAngle;
  Standard_Boolean myDrawCenterArrow;
  Standard_Boolean myDrawEdgesArrows;
  Standard_Boolean myDrawEdges;
  Standard_Boolean myDrawIso;
  Quantity_Length myPlaneXLength;
  Quantity_Length myPlaneYLength;
  Quantity_Length myIsoDistance;


};







#endif // _Prs3d_PlaneAspect_HeaderFile
