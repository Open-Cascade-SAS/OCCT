// Created on: 1997-02-18
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

#ifndef _VrmlConverter_WFDeflectionRestrictedFace_HeaderFile
#define _VrmlConverter_WFDeflectionRestrictedFace_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <Standard_OStream.hxx>

class VrmlConverter_Drawer;

//! WFDeflectionRestrictedFace    -    computes    the
//! wireframe   presentation   of  faces       with
//! restrictions by  displaying  a given  number of  U
//! and/or  V  isoparametric  curves,  converts his
//! into VRML objects   and writes (adds) them  into
//! anOStream.    All   requested properties  of the
//! representation  are  specify in  aDrawer of Drawer
//! class (Prs3d).    This kind  of the presentation
//! is     converted       into   IndexedFaceSet   and
//! IndexedLineSet ( VRML ).
class VrmlConverter_WFDeflectionRestrictedFace
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void Add(Standard_OStream&                   anOStream,
                                  const Handle(BRepAdaptor_Surface)&  aFace,
                                  const Handle(VrmlConverter_Drawer)& aDrawer);

  Standard_EXPORT static void AddUIso(Standard_OStream&                   anOStream,
                                      const Handle(BRepAdaptor_Surface)&  aFace,
                                      const Handle(VrmlConverter_Drawer)& aDrawer);

  Standard_EXPORT static void AddVIso(Standard_OStream&                   anOStream,
                                      const Handle(BRepAdaptor_Surface)&  aFace,
                                      const Handle(VrmlConverter_Drawer)& aDrawer);

  Standard_EXPORT static void Add(Standard_OStream&                   anOStream,
                                  const Handle(BRepAdaptor_Surface)&  aFace,
                                  const Standard_Boolean              DrawUIso,
                                  const Standard_Boolean              DrawVIso,
                                  const Standard_Real                 Deflection,
                                  const Standard_Integer              NBUiso,
                                  const Standard_Integer              NBViso,
                                  const Handle(VrmlConverter_Drawer)& aDrawer);
};

#endif // _VrmlConverter_WFDeflectionRestrictedFace_HeaderFile
