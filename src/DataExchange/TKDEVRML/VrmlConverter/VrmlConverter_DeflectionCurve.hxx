// Created on: 1997-04-29
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

#ifndef _VrmlConverter_DeflectionCurve_HeaderFile
#define _VrmlConverter_DeflectionCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_OStream.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
class Adaptor3d_Curve;
class VrmlConverter_Drawer;

//! DeflectionCurve - computes the presentation of
//! objects to be seen as curves, converts this one into
//! VRML objects and writes (adds) into
//! anOStream. All requested properties of the
//! representation are specify in aDrawer.
//! This kind of the presentation
//! is converted into IndexedLineSet (VRML).
//! The computation will be made according to a maximal
//! chordial deviation.
class VrmlConverter_DeflectionCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! adds to the OStream the drawing of the curve aCurve with
  //! respect to the maximal chordial deviation defined
  //! by the drawer aDrawer.
  //! The aspect is defined by LineAspect in aDrawer.
  Standard_EXPORT static void Add(Standard_OStream&                        anOStream,
                                  Adaptor3d_Curve&                         aCurve,
                                  const occ::handle<VrmlConverter_Drawer>& aDrawer);

  //! adds to the OStream the drawing of the curve aCurve with
  //! respect to the maximal chordial deviation defined
  //! by the drawer aDrawer.
  //! The aspect is defined by LineAspect in aDrawer.
  //! The drawing will be limited between the points of parameter
  //! U1 and U2.
  Standard_EXPORT static void Add(Standard_OStream&                        anOStream,
                                  Adaptor3d_Curve&                         aCurve,
                                  const double                             U1,
                                  const double                             U2,
                                  const occ::handle<VrmlConverter_Drawer>& aDrawer);

  //! adds to the OStream the drawing of the curve aCurve with
  //! respect to the maximal chordial deviation aDeflection.
  //! The aspect is the current aspect
  Standard_EXPORT static void Add(Standard_OStream& anOStream,
                                  Adaptor3d_Curve&  aCurve,
                                  const double      aDeflection,
                                  const double      aLimit);

  //! adds to the OStream the drawing of the curve aCurve with
  //! respect to the maximal chordial deviation aDeflection.
  //! The aspect is the current aspect
  Standard_EXPORT static void Add(Standard_OStream&                        anOStream,
                                  Adaptor3d_Curve&                         aCurve,
                                  const double                             aDeflection,
                                  const occ::handle<VrmlConverter_Drawer>& aDrawer);

  //! adds to the OStream the drawing of the curve aCurve with
  //! respect to the maximal chordial deviation aDeflection.
  //! The aspect is the current aspect
  //! The drawing will be limited between the points of parameter
  //! U1 and U2.
  Standard_EXPORT static void Add(Standard_OStream& anOStream,
                                  Adaptor3d_Curve&  aCurve,
                                  const double      U1,
                                  const double      U2,
                                  const double      aDeflection);

  //! adds to the OStream the drawing of the curve aCurve with
  //! the array of parameters to retrieve points on curve.
  Standard_EXPORT static void Add(Standard_OStream&                               anOStream,
                                  const Adaptor3d_Curve&                          aCurve,
                                  const occ::handle<NCollection_HArray1<double>>& aParams,
                                  const int                                       aNbNodes,
                                  const occ::handle<VrmlConverter_Drawer>&        aDrawer);
};

#endif // _VrmlConverter_DeflectionCurve_HeaderFile
