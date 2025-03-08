// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef StdPrs_WFRestrictedFace_HeaderFile
#define StdPrs_WFRestrictedFace_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>

// Computes the wireframe presentation of faces with
// restrictions by displaying a given number of U and/or
// V isoparametric curves. The isoparametric curves are
// drawn with a fixed number of points.
// The presentation includes the restriction curves.

class StdPrs_WFRestrictedFace : public Prs3d_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)&  thePresentation,
                                  const Handle(BRepAdaptor_Surface)& theFace,
                                  const Standard_Boolean             theDrawUIso,
                                  const Standard_Boolean             theDrawVIso,
                                  const Standard_Integer             theNbUIso,
                                  const Standard_Integer             theNbVIso,
                                  const Handle(Prs3d_Drawer)&        theDrawer,
                                  Prs3d_NListOfSequenceOfPnt&        theCurves);

  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)&  thePresentation,
                                  const Handle(BRepAdaptor_Surface)& theFace,
                                  const Handle(Prs3d_Drawer)&        theDrawer);

  Standard_EXPORT static Standard_Boolean Match(const Standard_Real                theX,
                                                const Standard_Real                theY,
                                                const Standard_Real                theZ,
                                                const Standard_Real                theDistance,
                                                const Handle(BRepAdaptor_Surface)& theFace,
                                                const Standard_Boolean             theDrawUIso,
                                                const Standard_Boolean             theDrawVIso,
                                                const Standard_Real                theDeflection,
                                                const Standard_Integer             theNbUIso,
                                                const Standard_Integer             theNbVIso,
                                                const Handle(Prs3d_Drawer)&        theDrawer);

  Standard_EXPORT static Standard_Boolean Match(const Standard_Real                theX,
                                                const Standard_Real                theY,
                                                const Standard_Real                theZ,
                                                const Standard_Real                theDistance,
                                                const Handle(BRepAdaptor_Surface)& theFace,
                                                const Handle(Prs3d_Drawer)&        theDrawer);

  Standard_EXPORT static Standard_Boolean MatchUIso(const Standard_Real                theX,
                                                    const Standard_Real                theY,
                                                    const Standard_Real                theZ,
                                                    const Standard_Real                theDistance,
                                                    const Handle(BRepAdaptor_Surface)& theFace,
                                                    const Handle(Prs3d_Drawer)&        theDrawer);

  Standard_EXPORT static Standard_Boolean MatchVIso(const Standard_Real                theX,
                                                    const Standard_Real                theY,
                                                    const Standard_Real                theZ,
                                                    const Standard_Real                theDistance,
                                                    const Handle(BRepAdaptor_Surface)& theFace,
                                                    const Handle(Prs3d_Drawer)&        theDrawer);

  Standard_EXPORT static void AddUIso(const Handle(Prs3d_Presentation)&  thePresentation,
                                      const Handle(BRepAdaptor_Surface)& theFace,
                                      const Handle(Prs3d_Drawer)&        theDrawer);

  Standard_EXPORT static void AddVIso(const Handle(Prs3d_Presentation)&  thePresentation,
                                      const Handle(BRepAdaptor_Surface)& theFace,
                                      const Handle(Prs3d_Drawer)&        theDrawer);
};

#endif
