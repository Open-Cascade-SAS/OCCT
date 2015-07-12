// Created on: 1997-04-23
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

#ifndef _VrmlConverter_Curve_HeaderFile
#define _VrmlConverter_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_OStream.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class Adaptor3d_Curve;
class VrmlConverter_Drawer;


//! Curve - computes the presentation of objects to be
//! seen  as curves  (the  computation  will  be  made
//! with a constant  number  of  points),  converts this one
//! into  VRML  objects  and  writes (adds) them  into
//! anOStream.   All  requested   properties  of   the
//! representation are specify  in aDrawer  of  Drawer
//! class (VrmlConverter).
//! This kind of the presentation is converted into
//! IndexedLineSet ( VRML ).
class VrmlConverter_Curve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! adds to the OStream the drawing of the curve aCurve.
  //! The aspect is defined by LineAspect in aDrawer.
  Standard_EXPORT static void Add (const Adaptor3d_Curve& aCurve, const Handle(VrmlConverter_Drawer)& aDrawer, Standard_OStream& anOStream);
  
  //! adds to the OStream the drawing of the curve aCurve.
  //! The aspect is defined by LineAspect in aDrawer.
  //! The drawing will be limited between the points of parameter
  //! U1 and U2.
  Standard_EXPORT static void Add (const Adaptor3d_Curve& aCurve, const Standard_Real U1, const Standard_Real U2, const Handle(VrmlConverter_Drawer)& aDrawer, Standard_OStream& anOStream);
  
  //! adds to the OStream the drawing of the curve aCurve.
  //! The aspect is the current aspect.
  //! The drawing will be limited between the points of parameter
  //! U1 and U2. aNbPoints defines  number of points on  one interval.
  Standard_EXPORT static void Add (const Adaptor3d_Curve& aCurve, const Standard_Real U1, const Standard_Real U2, Standard_OStream& anOStream, const Standard_Integer aNbPoints);




protected:





private:





};







#endif // _VrmlConverter_Curve_HeaderFile
