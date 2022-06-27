// Created on: 1997-02-21
// Created by: Alexander BRIVIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _VrmlConverter_Drawer_HeaderFile
#define _VrmlConverter_Drawer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Aspect_TypeOfDeflection.hxx>
#include <Standard_Transient.hxx>
class VrmlConverter_IsoAspect;
class VrmlConverter_LineAspect;
class VrmlConverter_ShadingAspect;
class VrmlConverter_PointAspect;


class VrmlConverter_Drawer;
DEFINE_STANDARD_HANDLE(VrmlConverter_Drawer, Standard_Transient)

//! qualifies the aspect properties for
//! the VRML conversation of a specific kind of object.
//! This includes for example color, maximal chordial deviation, etc...
class VrmlConverter_Drawer : public Standard_Transient
{

public:

  
  Standard_EXPORT VrmlConverter_Drawer();
  
  //! by default: TOD_Relative; however, except for the shapes,
  //! the drawing  will be made using the absolute deviation.
  Standard_EXPORT void SetTypeOfDeflection (const Aspect_TypeOfDeflection aTypeOfDeflection);
  
  Standard_EXPORT Aspect_TypeOfDeflection TypeOfDeflection() const;
  
  //! Defines the maximal chordial deviation when drawing any curve;
  //! If this  value is  one  of  the  obvious  parameters  of  methods,
  //! current  value  from  Drawer won't be used.
  //! This value is used by:
  //!
  //! VrmlConverter_DeflectionCurve
  //! VrmlConverter_WFDeflectionRestrictedFace
  //! VrmlConverter_WFDeflectionShape
  Standard_EXPORT void SetMaximalChordialDeviation (const Standard_Real aChordialDeviation);
  
  //! returns the maximal chordial deviation.
  //! Default value: 0.1
  Standard_EXPORT Standard_Real MaximalChordialDeviation() const;
  
  //! default 0.001
  Standard_EXPORT void SetDeviationCoefficient (const Standard_Real aCoefficient);
  
  Standard_EXPORT Standard_Real DeviationCoefficient() const;
  
  //! default: 17 points.
  //! Defines the Discretisation  (myNbPoints) when drawing any curve;
  //! If this  value is  one  of  the  obvious  parameters  of  methods,
  //! current  value  from  Drawer won't be used.
  //! This value is used by:
  //!
  //! VrmlConverter_Curve
  //! VrmlConverter_WFRestrictedFace
  //! VrmlConverter_WFShape
  Standard_EXPORT void SetDiscretisation (const Standard_Integer d);
  
  Standard_EXPORT Standard_Integer Discretisation() const;
  
  //! defines the maximum value allowed  for the first and last
  //! parameters of an infinite curve.
  //! Default value: 500.
  //! VrmlConverter_Curve
  //! VrmlConverter_WFRestrictedFace
  //! VrmlConverter_WFShape
  Standard_EXPORT void SetMaximalParameterValue (const Standard_Real Value);
  
  Standard_EXPORT Standard_Real MaximalParameterValue() const;
  
  //! enables the drawing of isos on planes.
  //! By default there are no isos on planes.
  Standard_EXPORT void SetIsoOnPlane (const Standard_Boolean OnOff);
  
  //! returns True if the drawing of isos on planes is enabled.
  Standard_EXPORT Standard_Boolean IsoOnPlane() const;
  
  //! Defines the attributes which are used when drawing an
  //! U isoparametric curve of a face. Defines the number
  //! of U isoparametric curves to be drawn for a single face.
  //! The default values are the same default values from Vrml package.
  //!
  //! These attributes are used by the following algorithms:
  //! VrmlConverter_WFRestrictedFace
  //! VrmlConverter_WFDeflectionRestrictedFace
  Standard_EXPORT Handle(VrmlConverter_IsoAspect) UIsoAspect();
  
  Standard_EXPORT void SetUIsoAspect (const Handle(VrmlConverter_IsoAspect)& anAspect);
  
  //! Defines the attributes which are used when drawing an
  //! V isoparametric curve of a face. Defines the number
  //! of V isoparametric curves to be drawn for a single face.
  //! The default values are the same default values from Vrml package.
  //!
  //! These attributes are used by the following algorithms:
  //! VrmlConverter_WFRestrictedFace
  //! VrmlConverter_WFDeflectionRestrictedFace
  Standard_EXPORT Handle(VrmlConverter_IsoAspect) VIsoAspect();
  
  Standard_EXPORT void SetVIsoAspect (const Handle(VrmlConverter_IsoAspect)& anAspect);
  

  //! The default values are the same default values from Vrml package.
  //! These attributes are used by the following algorithms:
  //! VrmlConverter_WFShape
  //! VrmlConverter_WFDeflectionShape
  Standard_EXPORT Handle(VrmlConverter_LineAspect) FreeBoundaryAspect();
  
  Standard_EXPORT void SetFreeBoundaryAspect (const Handle(VrmlConverter_LineAspect)& anAspect);
  
  //! enables the drawing the free boundaries
  //! By default the free boundaries  are drawn.
  Standard_EXPORT void SetFreeBoundaryDraw (const Standard_Boolean OnOff);
  
  //! returns True if the drawing of the free boundaries is enabled.
  Standard_EXPORT Standard_Boolean FreeBoundaryDraw() const;
  

  //! The default values are the same default values from Vrml package.
  //! These attributes are used by the following algorithms:
  //! VrmlConverter_WFShape
  //! VrmlConverter_WFDeflectionShape
  Standard_EXPORT Handle(VrmlConverter_LineAspect) WireAspect();
  
  Standard_EXPORT void SetWireAspect (const Handle(VrmlConverter_LineAspect)& anAspect);
  
  //! enables the drawing the wire
  //! By default the wire  are drawn.
  Standard_EXPORT void SetWireDraw (const Standard_Boolean OnOff);
  
  //! returns True if the drawing of the wire is enabled.
  Standard_EXPORT Standard_Boolean WireDraw() const;
  

  //! The default values are the same default values from Vrml package.
  //! These attributes are used by the following algorithms:
  //! VrmlConverter_WFShape
  //! VrmlConverter_WFDeflectionShape
  Standard_EXPORT Handle(VrmlConverter_LineAspect) UnFreeBoundaryAspect();
  
  Standard_EXPORT void SetUnFreeBoundaryAspect (const Handle(VrmlConverter_LineAspect)& anAspect);
  
  //! enables the drawing the unfree boundaries
  //! By default the unfree boundaries  are drawn.
  Standard_EXPORT void SetUnFreeBoundaryDraw (const Standard_Boolean OnOff);
  
  //! returns True if the drawing of the unfree boundaries is enabled.
  Standard_EXPORT Standard_Boolean UnFreeBoundaryDraw() const;
  

  //! The default values are the same default values from Vrml package.
  Standard_EXPORT Handle(VrmlConverter_LineAspect) LineAspect();
  
  Standard_EXPORT void SetLineAspect (const Handle(VrmlConverter_LineAspect)& anAspect);
  
  Standard_EXPORT Handle(VrmlConverter_PointAspect) PointAspect();
  
  Standard_EXPORT void SetPointAspect (const Handle(VrmlConverter_PointAspect)& anAspect);
  

  //! The default values are the same default values from Vrml package.
  Standard_EXPORT Handle(VrmlConverter_ShadingAspect) ShadingAspect();
  
  Standard_EXPORT void SetShadingAspect (const Handle(VrmlConverter_ShadingAspect)& anAspect);
  
  //! returns Standard_True if the hidden lines are to be drawn.
  //! By default the hidden lines are not drawn.
  Standard_EXPORT Standard_Boolean DrawHiddenLine() const;
  
  //! sets DrawHiddenLine  =  Standard_True  -  the hidden lines are drawn.
  Standard_EXPORT void EnableDrawHiddenLine();
  
  //! sets DrawHiddenLine  =  Standard_False  -  the hidden lines are not drawn.
  Standard_EXPORT void DisableDrawHiddenLine();
  
  //! returns LineAspect  for  the hidden lines.
  //! The default values are the same default values from Vrml package.
  Standard_EXPORT Handle(VrmlConverter_LineAspect) HiddenLineAspect();
  
  //! sets LineAspect  for  the hidden lines.
  Standard_EXPORT void SetHiddenLineAspect (const Handle(VrmlConverter_LineAspect)& anAspect);
  
  //! returns LineAspect  for  the seen lines.
  //! The default values are the same default values from Vrml package.
  Standard_EXPORT Handle(VrmlConverter_LineAspect) SeenLineAspect();
  
  //! sets LineAspect  for  the seen lines.
  Standard_EXPORT void SetSeenLineAspect (const Handle(VrmlConverter_LineAspect)& anAspect);

  DEFINE_STANDARD_RTTIEXT(VrmlConverter_Drawer,Standard_Transient)

private:

  Handle(VrmlConverter_IsoAspect) myUIsoAspect;
  Handle(VrmlConverter_IsoAspect) myVIsoAspect;
  Standard_Integer myNbPoints;
  Standard_Boolean myIsoOnPlane;
  Handle(VrmlConverter_LineAspect) myFreeBoundaryAspect;
  Standard_Boolean myFreeBoundaryDraw;
  Handle(VrmlConverter_LineAspect) myUnFreeBoundaryAspect;
  Standard_Boolean myUnFreeBoundaryDraw;
  Handle(VrmlConverter_LineAspect) myWireAspect;
  Standard_Boolean myWireDraw;
  Handle(VrmlConverter_LineAspect) myLineAspect;
  Handle(VrmlConverter_ShadingAspect) myShadingAspect;
  Standard_Real myChordialDeviation;
  Aspect_TypeOfDeflection myTypeOfDeflection;
  Standard_Real myMaximalParameterValue;
  Standard_Real myDeviationCoefficient;
  Handle(VrmlConverter_PointAspect) myPointAspect;
  Standard_Boolean myDrawHiddenLine;
  Handle(VrmlConverter_LineAspect) myHiddenLineAspect;
  Handle(VrmlConverter_LineAspect) mySeenLineAspect;

};

#endif // _VrmlConverter_Drawer_HeaderFile
