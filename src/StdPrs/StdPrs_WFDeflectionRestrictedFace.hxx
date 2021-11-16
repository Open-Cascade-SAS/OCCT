// Created on: 1995-08-07
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

#ifndef _StdPrs_WFDeflectionRestrictedFace_HeaderFile
#define _StdPrs_WFDeflectionRestrictedFace_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>


//! A framework to provide display of U and V
//! isoparameters of faces, while allowing you to impose
//! a deflection on them.
//! Computes the wireframe presentation of faces with
//! restrictions by displaying a given number of U and/or
//! V isoparametric curves. The isoparametric curves are
//! drawn with respect to a maximal chordial deviation.
//! The presentation includes the restriction curves.
class StdPrs_WFDeflectionRestrictedFace  : public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines a display featuring U and V isoparameters.
  //! Adds the surface aFace to the
  //! StdPrs_WFRestrictedFace algorithm. This face is
  //! found in a shape in the presentation object
  //! aPresentation, and its display attributes - in
  //! particular, the number of U and V isoparameters - are
  //! set in the attribute manager aDrawer.
  //! aFace is BRepAdaptor_Surface surface created
  //! from a face in a topological shape.   which is passed
  //! as an argument through the
  //! BRepAdaptor_Surface surface created from it.
  //! This is what allows the topological face to be treated
  //! as a geometric surface.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(BRepAdaptor_Surface)& aFace, const Handle(Prs3d_Drawer)& aDrawer);
  
  //! Defines a display featuring U isoparameters
  //! respectively. Add the surface aFace to the
  //! StdPrs_WFRestrictedFace algorithm. This face
  //! is found in a shape in the presentation object
  //! aPresentation, and its display attributes - in
  //! particular, the number of U isoparameters -
  //! are set in the attribute manager aDrawer.
  //! aFace is BRepAdaptor_Surface surface
  //! created from a face in a topological shape.   which
  //! is passed to the function as an argument through
  //! the BRepAdaptor_Surface surface created from
  //! it. This is what allows the topological face to be
  //! treated as a geometric surface.
  Standard_EXPORT static void AddUIso (const Handle(Prs3d_Presentation)& aPresentation, const Handle(BRepAdaptor_Surface)& aFace, const Handle(Prs3d_Drawer)& aDrawer);
  
  //! Defines a display featuring V isoparameters
  //! respectively. Add the surface aFace to the
  //! StdPrs_WFRestrictedFace algorithm. This face
  //! is found in a shape in the presentation object
  //! aPresentation, and its display attributes - in
  //! particular, the number of V isoparameters -
  //! are set in the attribute manager aDrawer.
  //! aFace is BRepAdaptor_Surface surface
  //! created from a face in a topological shape.   which
  //! is passed to the function as an argument through
  //! the BRepAdaptor_Surface surface created from
  //! it. This is what allows the topological face to be
  //! treated as a geometric surface.
  Standard_EXPORT static void AddVIso (const Handle(Prs3d_Presentation)& aPresentation, const Handle(BRepAdaptor_Surface)& aFace, const Handle(Prs3d_Drawer)& aDrawer);
  
  //! Defines a display of a delection-specified face. The
  //! display will feature U and V isoparameters.
  //! Adds the topology aShape to the
  //! StdPrs_WFRestrictedFace algorithm. This shape is
  //! found in the presentation object aPresentation, and
  //! its display attributes - except the number of U and V
  //! isoparameters - are set in the attribute manager aDrawer.
  //! The function sets the number of U and V
  //! isoparameters, NBUiso and NBViso, in the shape. To
  //! do this, the arguments DrawUIso and DrawVIso must be true.
  //! aFace is BRepAdaptor_Surface surface created
  //! from a face in a topological shape.   which is passed
  //! as an argument through the
  //! BRepAdaptor_Surface surface created from it.
  //! This is what allows the topological face to be treated
  //! as a geometric surface.
  //! Curves give a sequence of face curves, it is used if the PrimitiveArray
  //! visualization approach is activated (it is activated by default).
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(BRepAdaptor_Surface)& aFace, const Standard_Boolean DrawUIso, const Standard_Boolean DrawVIso, const Standard_Real Deflection, const Standard_Integer NBUiso, const Standard_Integer NBViso, const Handle(Prs3d_Drawer)& aDrawer, Prs3d_NListOfSequenceOfPnt& Curves);
  
  Standard_EXPORT static Standard_Boolean Match (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const Handle(BRepAdaptor_Surface)& aFace, const Handle(Prs3d_Drawer)& aDrawer);
  
  Standard_EXPORT static Standard_Boolean MatchUIso (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const Handle(BRepAdaptor_Surface)& aFace, const Handle(Prs3d_Drawer)& aDrawer);
  
  Standard_EXPORT static Standard_Boolean MatchVIso (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const Handle(BRepAdaptor_Surface)& aFace, const Handle(Prs3d_Drawer)& aDrawer);
  
  Standard_EXPORT static Standard_Boolean Match (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const Handle(BRepAdaptor_Surface)& aFace, const Handle(Prs3d_Drawer)& aDrawer, const Standard_Boolean DrawUIso, const Standard_Boolean DrawVIso, const Standard_Real aDeflection, const Standard_Integer NBUiso, const Standard_Integer NBViso);

};

#endif // _StdPrs_WFDeflectionRestrictedFace_HeaderFile
