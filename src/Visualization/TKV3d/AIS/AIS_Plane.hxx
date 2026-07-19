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
  Standard_EXPORT AIS_Plane(const occ::handle<Geom_Plane>& aComponent,
                            const bool                     aCurrentMode = false);

  //! initializes the plane aComponent and
  //! the point aCenter. If the mode aCurrentMode
  //! equals true, the drawing tool, "Drawer" is not
  //! initialized. aCurrentMode equals true, the drawing
  //! tool, "Drawer" is not initialized.
  Standard_EXPORT AIS_Plane(const occ::handle<Geom_Plane>& aComponent,
                            const gp_Pnt&                  aCenter,
                            const bool                     aCurrentMode = false);

  //! initializes the plane aComponent, the
  //! point aCenter, and the minimum and maximum
  //! points, aPmin and aPmax. If the mode
  //! aCurrentMode equals true, the drawing tool, "Drawer" is not initialized.
  Standard_EXPORT AIS_Plane(const occ::handle<Geom_Plane>& aComponent,
                            const gp_Pnt&                  aCenter,
                            const gp_Pnt&                  aPmin,
                            const gp_Pnt&                  aPmax,
                            const bool                     aCurrentMode = false);

  Standard_EXPORT AIS_Plane(const occ::handle<Geom_Axis2Placement>& aComponent,
                            const AIS_TypeOfPlane                   aPlaneType,
                            const bool                              aCurrentMode = false);

  //! Same value for x and y directions
  Standard_EXPORT void SetSize(const double aValue);

  //! Sets the size defined by the length along the X axis
  //! XVal and the length along the Y axis YVal.
  Standard_EXPORT void SetSize(const double Xval, const double YVal);

  Standard_EXPORT void UnsetSize();

  Standard_EXPORT bool Size(double& X, double& Y) const;

  bool HasOwnSize() const { return myHasOwnSize; }

  //! Sets transform persistence for zoom with value of minimum size
  Standard_EXPORT void SetMinimumSize(const double theValue);

  //! Unsets transform persistence zoom
  Standard_EXPORT void UnsetMinimumSize();

  //! Returns true if transform persistence for zoom is set
  Standard_EXPORT bool HasMinimumSize() const;

  int Signature() const override { return 7; }

  AIS_KindOfInteractive Type() const override { return AIS_KindOfInteractive_Datum; }

  //! Returns the component specified in SetComponent.
  const occ::handle<Geom_Plane>& Component() { return myComponent; }

  //! Creates an instance of the plane aComponent.
  Standard_EXPORT void SetComponent(const occ::handle<Geom_Plane>& aComponent);

  //! Returns the settings for the selected plane
  //! aComponent, provided in SetPlaneAttributes.
  //! These include the points aCenter, aPmin, and aPmax
  Standard_EXPORT bool PlaneAttributes(occ::handle<Geom_Plane>& aComponent,
                                       gp_Pnt&                  aCenter,
                                       gp_Pnt&                  aPmin,
                                       gp_Pnt&                  aPmax);

  //! Allows you to provide settings other than default ones
  //! for the selected plane. These include: center point
  //! aCenter, maximum aPmax and minimum aPmin.
  Standard_EXPORT void SetPlaneAttributes(const occ::handle<Geom_Plane>& aComponent,
                                          const gp_Pnt&                  aCenter,
                                          const gp_Pnt&                  aPmin,
                                          const gp_Pnt&                  aPmax);

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
  Standard_EXPORT void SetAxis2Placement(const occ::handle<Geom_Axis2Placement>& aComponent,
                                         const AIS_TypeOfPlane                   aPlaneType);

  //! Returns the position of the plane's axis2 system
  //! identifying the x, y, or z axis and giving the plane a
  //! direction in 3D space. An axis2 system is a right-handed coordinate system.
  Standard_EXPORT occ::handle<Geom_Axis2Placement> Axis2Placement();

  //! Returns the type of plane - xy, yz, xz or unknown.
  AIS_TypeOfPlane TypeOfPlane() { return myTypeOfPlane; }

  //! Returns the type of plane - xy, yz, or xz.
  bool IsXYZPlane() { return myIsXYZPlane; }

  //! Returns the non-default current display mode set by SetCurrentMode.
  bool CurrentMode() { return myCurrentMode; }

  //! Allows you to provide settings for a non-default
  //! current display mode.
  void SetCurrentMode(const bool theCurrentMode) { myCurrentMode = theCurrentMode; }

  //! Returns true if the display mode selected, aMode, is valid for planes.
  Standard_EXPORT bool AcceptDisplayMode(const int aMode) const override;

  //! connection to <aCtx> default drawer implies a recomputation of Frame values.
  Standard_EXPORT void SetContext(const occ::handle<AIS_InteractiveContext>& aCtx) override;

  //! Returns the type of sensitivity for the plane;
  Select3D_TypeOfSensitivity TypeOfSensitivity() const { return myTypeOfSensitivity; }

  //! Sets the type of sensitivity for the plane.
  void SetTypeOfSensitivity(Select3D_TypeOfSensitivity theTypeOfSensitivity)
  {
    myTypeOfSensitivity = theTypeOfSensitivity;
  }

  Standard_EXPORT void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                        const int                               theMode) override;

  Standard_EXPORT void SetColor(const Quantity_Color& aColor) override;

  Standard_EXPORT void UnsetColor() override;

private:
  Standard_EXPORT void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                               const occ::handle<Prs3d_Presentation>&         thePrs,
                               const int                                      theMode) override;

  Standard_EXPORT void ComputeFrame();

  Standard_EXPORT void ComputeFields();

  Standard_EXPORT void InitDrawerAttributes();

private:
  occ::handle<Geom_Plane>          myComponent;
  occ::handle<Geom_Axis2Placement> myAx2;
  gp_Pnt                           myCenter;
  gp_Pnt                           myPmin;
  gp_Pnt                           myPmax;
  bool                             myCurrentMode;
  bool                             myAutomaticPosition;
  AIS_TypeOfPlane                  myTypeOfPlane;
  bool                             myIsXYZPlane;
  bool                             myHasOwnSize;
  Select3D_TypeOfSensitivity       myTypeOfSensitivity;
};

#endif // _AIS_Plane_HeaderFile
