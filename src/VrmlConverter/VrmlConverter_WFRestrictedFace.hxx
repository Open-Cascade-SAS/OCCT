// Created on: 1997-05-13
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

#ifndef _VrmlConverter_WFRestrictedFace_HeaderFile
#define _VrmlConverter_WFRestrictedFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_OStream.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class BRepAdaptor_HSurface;
class VrmlConverter_Drawer;


//! WFRestrictedFace -     computes     the  wireframe
//! presentation  of faces  with   restrictions by
//! displaying   a  given  number    of  U   and/or  V
//! isoparametric  curves,  converts this  one into VRML
//! objects  and writes  (adds)  into anOStream.
//! All requested  properties  of the representation
//! are specify in  aDrawer.
//! This kind of the presentation is converted into
//! IndexedLineSet ( VRML ).
class VrmlConverter_WFRestrictedFace 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void Add (Standard_OStream& anOStream, const Handle(BRepAdaptor_HSurface)& aFace, const Handle(VrmlConverter_Drawer)& aDrawer);
  
  Standard_EXPORT static void AddUIso (Standard_OStream& anOStream, const Handle(BRepAdaptor_HSurface)& aFace, const Handle(VrmlConverter_Drawer)& aDrawer);
  
  Standard_EXPORT static void AddVIso (Standard_OStream& anOStream, const Handle(BRepAdaptor_HSurface)& aFace, const Handle(VrmlConverter_Drawer)& aDrawer);
  
  Standard_EXPORT static void Add (Standard_OStream& anOStream, const Handle(BRepAdaptor_HSurface)& aFace, const Standard_Boolean DrawUIso, const Standard_Boolean DrawVIso, const Standard_Integer NBUiso, const Standard_Integer NBViso, const Handle(VrmlConverter_Drawer)& aDrawer);




protected:





private:





};







#endif // _VrmlConverter_WFRestrictedFace_HeaderFile
