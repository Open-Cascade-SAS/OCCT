// Created on: 1995-08-09
// Created by: Arnaud BOUZY/Odile Olivier
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _AIS_Axis_HeaderFile
#define _AIS_Axis_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <AIS_TypeOfAxis.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <SelectMgr_Selection.hxx>

class Geom_Line;
class Geom_Axis1Placement;
class Geom_Axis2Placement;
class Prs3d_LineAspect;

//! Locates the x, y and z axes in an Interactive Object.
//! These are used to orient it correctly in presentations
//! from different viewpoints, or to construct a revolved
//! shape, for example, from one of the axes. Conversely,
//! an axis can be created to build a revolved shape and
//! then situated relative to one of the axes of the view.
class AIS_Axis : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_Axis, AIS_InteractiveObject)
public:

  //! Initializes the line aComponent
  Standard_EXPORT AIS_Axis(const Handle(Geom_Line)& aComponent);

  //! initializes the axis2 position
  //! aComponent. The coordinate system used is right-handed.
  Standard_EXPORT AIS_Axis(const Handle(Geom_Axis2Placement)& aComponent, const AIS_TypeOfAxis anAxisType);

  //! Initializes the axis1 position anAxis.
  Standard_EXPORT AIS_Axis(const Handle(Geom_Axis1Placement)& anAxis);

  //! Initializes the ray as axis with start point and direction
  //! @param[in] theAxis Start point and direction of the ray
  //! @param[in] theLength Optional length of the ray (ray is infinite by default).
  Standard_EXPORT AIS_Axis (const gp_Ax1& theAxis, const Standard_Real theLength = -1);

  //! Returns the axis entity aComponent and identifies it
  //! as a component of a shape.
  const Handle(Geom_Line)& Component() const { return myComponent; }

  //! Sets the coordinates of the lin aComponent.
  Standard_EXPORT void SetComponent (const Handle(Geom_Line)& aComponent);

  //! Returns the position of axis2 and   positions it by
  //! identifying it as the x, y, or z axis and giving its
  //! direction in 3D space. The coordinate system used is right-handed.
  const Handle(Geom_Axis2Placement)& Axis2Placement() const { return myAx2; }

  //! Allows you to provide settings for aComponent:the
  //! position and direction of an axis in 3D space. The
  //! coordinate system used is right-handed.
  Standard_EXPORT void SetAxis2Placement (const Handle(Geom_Axis2Placement)& aComponent, const AIS_TypeOfAxis anAxisType);
  
  //! Constructs a new line to serve as the axis anAxis in 3D space.
  Standard_EXPORT void SetAxis1Placement (const Handle(Geom_Axis1Placement)& anAxis);
  
  //! Returns the type of axis.
  AIS_TypeOfAxis TypeOfAxis() const { return myTypeOfAxis; }

  //! Constructs the entity theTypeAxis to stock information
  //! concerning type of axis.
  void SetTypeOfAxis (const AIS_TypeOfAxis theTypeAxis) { myTypeOfAxis = theTypeAxis; }

  //! Returns a signature of 2 for axis datums. When you
  //! activate mode 2 by a signature, you pick AIS objects
  //! of type AIS_Axis.
  Standard_Boolean IsXYZAxis() const { return myIsXYZAxis; }

  //! Returns true if the interactive object accepts the display mode aMode.
  Standard_EXPORT Standard_Boolean AcceptDisplayMode (const Standard_Integer aMode) const Standard_OVERRIDE;

  virtual Standard_Integer Signature() const Standard_OVERRIDE { return 2; }

  virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE { return AIS_KindOfInteractive_Datum; }

  Standard_EXPORT void SetColor (const Quantity_Color& aColor) Standard_OVERRIDE;

  Standard_EXPORT void SetWidth (const Standard_Real aValue) Standard_OVERRIDE;

  //! Set required visualization parameters.
  Standard_EXPORT void SetDisplayAspect (const Handle(Prs3d_LineAspect)& theNewDatumAspect);

  Standard_EXPORT void UnsetColor() Standard_OVERRIDE;

  Standard_EXPORT void UnsetWidth() Standard_OVERRIDE;

private:

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT void ComputeFields();

private:

  Handle(Geom_Line) myComponent;
  Handle(Geom_Axis2Placement) myAx2;
  gp_Pnt myPfirst;
  gp_Pnt myPlast;
  AIS_TypeOfAxis myTypeOfAxis;
  Standard_Boolean myIsXYZAxis;
  gp_Dir myDir;
  Standard_Real myVal;
  Standard_CString myText;
  Handle(Prs3d_LineAspect) myLineAspect;

};

DEFINE_STANDARD_HANDLE(AIS_Axis, AIS_InteractiveObject)

#endif // _AIS_Axis_HeaderFile
