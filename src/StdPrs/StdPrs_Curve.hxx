// Created on: 1995-08-04
// Created by: Modelistation
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

#ifndef _StdPrs_Curve_HeaderFile
#define _StdPrs_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <Standard_Integer.hxx>
class Prs3d_Presentation;
class Adaptor3d_Curve;


//! A framework to define display of lines, arcs of circles
//! and conic sections.
//! This is done with a fixed number of points, which can be modified.
class StdPrs_Curve  : public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds to the presentation aPresentation the drawing of the curve aCurve.
  //! The aspect is defined by LineAspect in aDrawer.
  //! If drawCurve equals Standard_False the curve will not be displayed,
  //! it is used if the curve is a part of some shape and PrimitiveArray
  //! visualization approach is activated (it is activated by default).
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Adaptor3d_Curve& aCurve, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Boolean drawCurve = Standard_True);
  
  //! Adds to the presentation aPresentation the drawing of the curve aCurve.
  //! The aspect is defined by LineAspect in aDrawer.
  //! The drawing will be limited between the points of parameter U1 and U2.
  //! If drawCurve equals Standard_False the curve will not be displayed,
  //! it is used if the curve is a part of some shape and PrimitiveArray
  //! visualization approach is activated (it is activated by default).
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Adaptor3d_Curve& aCurve, const Standard_Real U1, const Standard_Real U2, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Boolean drawCurve = Standard_True);
  
  //! adds to the presentation aPresentation the drawing of the curve aCurve.
  //! The aspect is the current aspect.
  //! aDeflection is used in the circle case.
  //! Points give a sequence of curve points.
  //! If drawCurve equals Standard_False the curve will not be displayed,
  //! it is used if the curve is a part of some shape and PrimitiveArray
  //! visualization approach is activated (it is activated by default).
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Adaptor3d_Curve& aCurve, const Handle(Prs3d_Drawer)& aDrawer, TColgp_SequenceOfPnt& Points, const Standard_Boolean drawCurve = Standard_True);
  
  //! adds to the presentation aPresentation the drawing of the curve
  //! aCurve.
  //! The aspect is the current aspect.
  //! The drawing will be limited between the points of parameter
  //! U1 and U2.
  //! aDeflection is used in the circle case.
  //! Points give a sequence of curve points.
  //! If drawCurve equals Standard_False the curve will not be displayed,
  //! it is used if the curve is a part of some shape and PrimitiveArray
  //! visualization approach is activated (it is activated by default).
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Adaptor3d_Curve& aCurve, const Standard_Real U1, const Standard_Real U2, TColgp_SequenceOfPnt& Points, const Standard_Integer aNbPoints = 30, const Standard_Boolean drawCurve = Standard_True);
  
  //! returns true if the distance between the point (X,Y,Z) and the
  //! drawing of the curve is less than aDistance.
  Standard_EXPORT static Standard_Boolean Match (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const Adaptor3d_Curve& aCurve, const Handle(Prs3d_Drawer)& aDrawer);
  
  //! returns true if the distance between the point (X,Y,Z) and the
  //! drawing of the curve is less than aDistance.
  Standard_EXPORT static Standard_Boolean Match (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const Adaptor3d_Curve& aCurve, const Standard_Real aDeflection, const Standard_Real aLimit, const Standard_Integer aNbPoints);
  
  //! returns true if the distance between the point (X,Y,Z) and the
  //! drawing of the curve aCurve is less than aDistance.
  //! The drawing is considered between the points
  //! of parameter U1 and U2;
  Standard_EXPORT static Standard_Boolean Match (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const Adaptor3d_Curve& aCurve, const Standard_Real U1, const Standard_Real U2, const Handle(Prs3d_Drawer)& aDrawer);
  
  //! returns true if the distance between the point (X,Y,Z) and the
  //! drawing of the curve aCurve is less than aDistance.
  //! The drawing is considered between the points
  //! of parameter U1 and U2;
  Standard_EXPORT static Standard_Boolean Match (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const Adaptor3d_Curve& aCurve, const Standard_Real U1, const Standard_Real U2, const Standard_Real aDeflection, const Standard_Integer aNbPoints);

};

#endif // _StdPrs_Curve_HeaderFile
