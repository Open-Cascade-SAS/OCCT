// Created on: 2014-10-14
// Created by: Anton POLETAEV
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

#ifndef _StdPrs_Isolines_H__
#define _StdPrs_Isolines_H__

#include <BRepAdaptor_HSurface.hxx>
#include <Geom_Surface.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>
#include <Poly_Triangulation.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_SequenceOfReal.hxx>

class Poly_Triangle;
class TopoDS_Face;
class TopLoc_Location;

//! Tool for computing isoline representation for a face or surface.
//! Depending on a flags set to the given Prs3d_Drawer instance, on-surface (is used
//! by default) or on-triangulation isoline builder algorithm will be used.
//! If the given shape is not triangulated, on-surface isoline builder will be applied
//! regardless of Prs3d_Drawer flags.
class StdPrs_Isolines : public Prs3d_Root
{
public:

  //! Computes isolines presentation for a TopoDS face.
  //! This method chooses proper version of isoline builder algorithm : on triangulation
  //! or surface depending on the flag passed from Prs3d_Drawer attributes.
  //! This method is a default way to display isolines for a given TopoDS face.
  //! @param thePresentation [in] the presentation.
  //! @param theFace [in] the face.
  //! @param theDrawer [in] the display settings.
  //! @param theDeflection [in] the deflection for isolines-on-surface version.
  inline static void Add (const Handle(Prs3d_Presentation)& thePresentation,
                          const TopoDS_Face&                theFace,
                          const Handle(Prs3d_Drawer)&       theDrawer,
                          const Standard_Real               theDeflection)
  {
    if (theDrawer->IsoOnTriangulation() && StdPrs_ToolTriangulatedShape::IsTriangulated (theFace))
    {
      AddOnTriangulation (thePresentation, theFace, theDrawer);
    }
    else
    {
      AddOnSurface (thePresentation, theFace, theDrawer, theDeflection);
    }
  }

  //! Computes isolines on triangulation and adds them to a presentation.
  //! @param thePresentation [in] the presentation.
  //! @param theFace [in] the face.
  //! @param theDrawer [in] the display settings.
  Standard_EXPORT static void AddOnTriangulation (const Handle(Prs3d_Presentation)& thePresentation,
                                                  const TopoDS_Face&                theFace,
                                                  const Handle(Prs3d_Drawer)&       theDrawer);

  //! Computes isolines on triangulation and adds them to a presentation.
  //! @param thePresentation [in] the presentation.
  //! @param theTriangulation [in] the triangulation.
  //! @param theSurface [in] the definition of triangulated surface. The surface
  //!        adapter is used to precisely evaluate isoline points using surface
  //!        law and fit them on triangulation. If NULL is passed, the method will
  //!        use linear interpolation of triangle node's UV coordinates to evaluate
  //!        isoline points.
  //! @param theLocation [in] the location transformation defined for triangulation (surface).
  //! @param theDrawer [in] the display settings.
  //! @param theUIsoParams [in] the parameters of u isolines to compute.
  //! @param theVIsoParams [in] the parameters of v isolines to compute.
  Standard_EXPORT static void AddOnTriangulation (const Handle(Prs3d_Presentation)& thePresentation,
                                                  const Handle(Poly_Triangulation)& theTriangulation,
                                                  const Handle(Geom_Surface)&       theSurface,
                                                  const TopLoc_Location&            theLocation,
                                                  const Handle(Prs3d_Drawer)&       theDrawer,
                                                  const TColStd_SequenceOfReal&     theUIsoParams,
                                                  const TColStd_SequenceOfReal&     theVIsoParams);

  //! Computes isolines on surface and adds them to presentation.
  //! @param thePresentation [in] the presentation.
  //! @param theFace [in] the face.
  //! @param theDrawer [in] the display settings.
  //! @param theDeflection [in] the deflection value.
  Standard_EXPORT static void AddOnSurface (const Handle(Prs3d_Presentation)& thePresentation,
                                            const TopoDS_Face&                theFace,
                                            const Handle(Prs3d_Drawer)&       theDrawer,
                                            const Standard_Real               theDeflection);

  //! Computes isolines on surface and adds them to presentation.
  //! @param thePresentation [in] the presentation.
  //! @param theSurface [in] the surface.
  //! @param theDrawer [in] the display settings.
  //! @param theDeflection [in] the deflection value.
  //! @param theUIsoParams [in] the parameters of u isolines to compute.
  //! @param theVIsoParams [in] the parameters of v isolines to compute.
  Standard_EXPORT static void AddOnSurface (const Handle(Prs3d_Presentation)&   thePresentation,
                                            const Handle(BRepAdaptor_HSurface)& theSurface,
                                            const Handle(Prs3d_Drawer)&         theDrawer,
                                            const Standard_Real                 theDeflection,
                                            const TColStd_SequenceOfReal&       theUIsoParams,
                                            const TColStd_SequenceOfReal&       theVIsoParams);

  //! Evalute sequence of parameters for drawing uv isolines for a given face.
  //! @param theFace [in] the face.
  //! @param theNbIsoU [in] the number of u isolines.
  //! @param theNbIsoV [in] the number of v isolines.
  //! @param theUVLimit [in] the u, v parameter value limit.
  //! @param theUIsoParams [out] the sequence of u isoline parameters.
  //! @param theVIsoParams [out] the sequence of v isoline parameters.
  Standard_EXPORT static void UVIsoParameters (const TopoDS_Face&      theFace,
                                               const Standard_Integer  theNbIsoU,
                                               const Standard_Integer  theNbIsoV,
                                               const Standard_Real     theUVLimit,
                                               TColStd_SequenceOfReal& theUIsoParams,
                                               TColStd_SequenceOfReal& theVIsoParams);

private:

  //! Find isoline segment on a triangle.
  //! @param theSurface [in] the surface.
  //! @param theIsoline [in] the isoline in uv coordinates.
  //! @param theNodesXYZ [in] the XYZ coordinates of triangle nodes.
  //! @param theNodesUV [in] the UV coordinates of triangle nodes.
  //! @param theSegment [out] the XYZ points of crossed triangle's links.
  //! @return TRUE if the isoline passes through the triangle.
  Standard_EXPORT static Standard_Boolean findSegmentOnTriangulation (const Handle(Geom_Surface)& theSurface,
                                                                      const gp_Lin2d&             theIsoline,
                                                                      const gp_Pnt*               theNodesXYZ,
                                                                      const gp_Pnt2d*             theNodesUV,
                                                                      gp_Pnt*                     theSegment);
};

#endif // _StdPrs_Isolines_H__
