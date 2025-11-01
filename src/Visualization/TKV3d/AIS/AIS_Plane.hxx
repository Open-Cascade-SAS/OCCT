// Created on: 1995-08-02
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

#ifndef _AIS_Plane_HeaderFile
#define _AIS_Plane_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <AIS_TypeOfPlane.hxx>
#include <gp_Pnt.hxx>
#include <Select3D_TypeOfSensitivity.hxx>

class Geom_Plane;
class Geom_Axis2Placement;

//! Constructs plane datums to be used in construction of
//! composite shapes.
class AIS_Plane : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_Plane, AIS_InteractiveObject)
public:
  //! initializes the plane aComponent. If
  //! the mode aCurrentMode equals true, the drawing
  //! tool, "Drawer" is not initialized.
  Standard_EXPORT AIS_Plane(const Handle(Geom_Plane)& aComponent,
                            const Standard_Boolean    aCurrentMode = Standard_False);

  //! initializes the plane aComponent and
  //! the point aCenter. If the mode aCurrentMode
  //! equals true, the drawing tool, "Drawer" is not
  //! initialized. aCurrentMode equals true, the drawing
  //! tool, "Drawer" is not initialized.
  Standard_EXPORT AIS_Plane(const Handle(Geom_Plane)& aComponent,
                            const gp_Pnt&             aCenter,
                            const Standard_Boolean    aCurrentMode = Standard_False);

  //! initializes the plane aComponent, the
  //! point aCenter, and the minimum and maximum
  //! points, aPmin and aPmax. If the mode
  //! aCurrentMode equals true, the drawing tool, "Drawer" is not initialized.
  Standard_EXPORT AIS_Plane(const Handle(Geom_Plane)& aComponent,
                            const gp_Pnt&             aCenter,
                            const gp_Pnt&             aPmin,
                            const gp_Pnt&             aPmax,
                            const Standard_Boolean    aCurrentMode = Standard_False);

  Standard_EXPORT AIS_Plane(const Handle(Geom_Axis2Placement)& aComponent,
                            const AIS_TypeOfPlane              aPlaneType,
                            const Standard_Boolean             aCurrentMode = Standard_False);

  //! Same value for x and y directions
  Standard_EXPORT void SetSize(const Standard_Real aValue);

  //! Sets the size defined by the length along the X axis
  //! XVal and the length along the Y axis YVal.
  Standard_EXPORT void SetSize(const Standard_Real Xval, const Standard_Real YVal);

  Standard_EXPORT void UnsetSize();

  Standard_EXPORT Standard_Boolean Size(Standard_Real& X, Standard_Real& Y) const;

  Standard_Boolean HasOwnSize() const { return myHasOwnSize; }

  //! Sets transform persistence for zoom with value of minimum size
  Standard_EXPORT void SetMinimumSize(const Standard_Real theValue);

  //! Unsets transform persistence zoom
  Standard_EXPORT void UnsetMinimumSize();

  //! Returns true if transform persistence for zoom is set
  Standard_EXPORT Standard_Boolean HasMinimumSize() const;

  virtual Standard_Integer Signature() const Standard_OVERRIDE { return 7; }

  virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE
  {
    return AIS_KindOfInteractive_Datum;
  }

  //! Returns the component specified in SetComponent.
  const Handle(Geom_Plane)& Component() { return myComponent; }

  //! Creates an instance of the plane aComponent.
  Standard_EXPORT void SetComponent(const Handle(Geom_Plane)& aComponent);

  //! Returns the settings for the selected plane
  //! aComponent, provided in SetPlaneAttributes.
  //! These include the points aCenter, aPmin, and aPmax
  Standard_EXPORT Standard_Boolean PlaneAttributes(Handle(Geom_Plane)& aComponent,
                                                   gp_Pnt&             aCenter,
                                                   gp_Pnt&             aPmin,
                                                   gp_Pnt&             aPmax);

  //! Allows you to provide settings other than default ones
  //! for the selected plane. These include: center point
  //! aCenter, maximum aPmax and minimum aPmin.
  Standard_EXPORT void SetPlaneAttributes(const Handle(Geom_Plane)& aComponent,
                                          const gp_Pnt&             aCenter,
                                          const gp_Pnt&             aPmin,
                                          const gp_Pnt&             aPmax);

  //! Returns the coordinates of the center point.
  const gp_Pnt& Center() const { return myCenter; }

  //! Provides settings for the center theCenter other than (0, 0, 0).
  void SetCenter(const gp_Pnt& theCenter) { myCenter = theCenter; }

  //! Allows you to provide settings for the position and
  //! direction of one of the plane's axes, aComponent, in
  //! 3D space. The coordinate system used is
  //! right-handed, and the type of plane aPlaneType is one of:
  //! -   AIS_ TOPL_Unknown
  //! -   AIS_ TOPL_XYPlane
  //! -   AIS_ TOPL_XZPlane
  //! -   AIS_ TOPL_YZPlane}.
  Standard_EXPORT void SetAxis2Placement(const Handle(Geom_Axis2Placement)& aComponent,
                                         const AIS_TypeOfPlane              aPlaneType);

  //! Returns the position of the plane's axis2 system
  //! identifying the x, y, or z axis and giving the plane a
  //! direction in 3D space. An axis2 system is a right-handed coordinate system.
  Standard_EXPORT Handle(Geom_Axis2Placement) Axis2Placement();

  //! Returns the type of plane - xy, yz, xz or unknown.
  AIS_TypeOfPlane TypeOfPlane() { return myTypeOfPlane; }

  //! Returns the type of plane - xy, yz, or xz.
  Standard_Boolean IsXYZPlane() { return myIsXYZPlane; }

  //! Returns the non-default current display mode set by SetCurrentMode.
  Standard_Boolean CurrentMode() { return myCurrentMode; }

  //! Allows you to provide settings for a non-default
  //! current display mode.
  void SetCurrentMode(const Standard_Boolean theCurrentMode) { myCurrentMode = theCurrentMode; }

  //! Returns true if the display mode selected, aMode, is valid for planes.
  Standard_EXPORT virtual Standard_Boolean AcceptDisplayMode(const Standard_Integer aMode) const
    Standard_OVERRIDE;

  //! connection to <aCtx> default drawer implies a recomputation of Frame values.
  Standard_EXPORT virtual void SetContext(const Handle(AIS_InteractiveContext)& aCtx)
    Standard_OVERRIDE;

  //! Returns the type of sensitivity for the plane;
  Select3D_TypeOfSensitivity TypeOfSensitivity() const { return myTypeOfSensitivity; }

  //! Sets the type of sensitivity for the plane.
  void SetTypeOfSensitivity(Select3D_TypeOfSensitivity theTypeOfSensitivity)
  {
    myTypeOfSensitivity = theTypeOfSensitivity;
  }

  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& theSelection,
                                                const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT void SetColor(const Quantity_Color& aColor) Standard_OVERRIDE;

  Standard_EXPORT void UnsetColor() Standard_OVERRIDE;

private:
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                       const Handle(Prs3d_Presentation)&         thePrs,
                                       const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT void ComputeFrame();

  Standard_EXPORT void ComputeFields();

  Standard_EXPORT void InitDrawerAttributes();

private:
  Handle(Geom_Plane)          myComponent;
  Handle(Geom_Axis2Placement) myAx2;
  gp_Pnt                      myCenter;
  gp_Pnt                      myPmin;
  gp_Pnt                      myPmax;
  Standard_Boolean            myCurrentMode;
  Standard_Boolean            myAutomaticPosition;
  AIS_TypeOfPlane             myTypeOfPlane;
  Standard_Boolean            myIsXYZPlane;
  Standard_Boolean            myHasOwnSize;
  Select3D_TypeOfSensitivity  myTypeOfSensitivity;
};

DEFINE_STANDARD_HANDLE(AIS_Plane, AIS_InteractiveObject)

#endif // _AIS_Plane_HeaderFile
