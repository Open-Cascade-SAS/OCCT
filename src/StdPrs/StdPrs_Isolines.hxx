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

#include <BRepAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Lin2d.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <TColStd_SequenceOfReal.hxx>

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
  //! @param[in] thePresentation  the presentation.
  //! @param[in] theFace  the face.
  //! @param[in] theDrawer  the display settings.
  //! @param[in] theDeflection  the deflection for isolines-on-surface version.
  inline static void Add(const Handle(Prs3d_Presentation)& thePresentation,
                         const TopoDS_Face&                theFace,
                         const Handle(Prs3d_Drawer)&       theDrawer,
                         const Standard_Real               theDeflection)
  {
    if (theDrawer->IsoOnTriangulation() && StdPrs_ToolTriangulatedShape::IsTriangulated(theFace))
    {
      AddOnTriangulation(thePresentation, theFace, theDrawer);
    }
    else
    {
      AddOnSurface(thePresentation, theFace, theDrawer, theDeflection);
    }
  }

  //! Computes isolines presentation for a TopoDS face.
  //! This method chooses proper version of isoline builder algorithm : on triangulation
  //! or surface depending on the flag passed from Prs3d_Drawer attributes.
  //! This method is a default way to display isolines for a given TopoDS face.
  //! @param[in] theFace  the face.
  //! @param[in] theDrawer  the display settings.
  //! @param[in] theDeflection  the deflection for isolines-on-surface version.
  static void Add(const TopoDS_Face&          theFace,
                  const Handle(Prs3d_Drawer)& theDrawer,
                  const Standard_Real         theDeflection,
                  Prs3d_NListOfSequenceOfPnt& theUPolylines,
                  Prs3d_NListOfSequenceOfPnt& theVPolylines)
  {
    if (theDrawer->IsoOnTriangulation() && StdPrs_ToolTriangulatedShape::IsTriangulated(theFace))
    {
      AddOnTriangulation(theFace, theDrawer, theUPolylines, theVPolylines);
    }
    else
    {
      AddOnSurface(theFace, theDrawer, theDeflection, theUPolylines, theVPolylines);
    }
  }

  //! Computes isolines on triangulation and adds them to a presentation.
  //! @param[in] thePresentation  the presentation.
  //! @param[in] theFace  the face.
  //! @param[in] theDrawer  the display settings.
  Standard_EXPORT static void AddOnTriangulation(const Handle(Prs3d_Presentation)& thePresentation,
                                                 const TopoDS_Face&                theFace,
                                                 const Handle(Prs3d_Drawer)&       theDrawer);

  //! Computes isolines on triangulation.
  //! @param[in] theFace  the face.
  //! @param[in] theDrawer  the display settings.
  //! @param[out] theUPolylines  the sequence of result polylines
  //! @param[out] theVPolylines  the sequence of result polylines
  Standard_EXPORT static void AddOnTriangulation(const TopoDS_Face&          theFace,
                                                 const Handle(Prs3d_Drawer)& theDrawer,
                                                 Prs3d_NListOfSequenceOfPnt& theUPolylines,
                                                 Prs3d_NListOfSequenceOfPnt& theVPolylines);

  //! Computes isolines on triangulation and adds them to a presentation.
  //! @param[in] thePresentation  the presentation.
  //! @param[in] theTriangulation  the triangulation.
  //! @param[in] theSurface  the definition of triangulated surface. The surface
  //!        adapter is used to precisely evaluate isoline points using surface
  //!        law and fit them on triangulation. If NULL is passed, the method will
  //!        use linear interpolation of triangle node's UV coordinates to evaluate
  //!        isoline points.
  //! @param[in] theLocation  the location transformation defined for triangulation (surface).
  //! @param[in] theDrawer  the display settings.
  //! @param[in] theUIsoParams  the parameters of u isolines to compute.
  //! @param[in] theVIsoParams  the parameters of v isolines to compute.
  Standard_EXPORT static void AddOnTriangulation(const Handle(Prs3d_Presentation)& thePresentation,
                                                 const Handle(Poly_Triangulation)& theTriangulation,
                                                 const Handle(Geom_Surface)&       theSurface,
                                                 const TopLoc_Location&            theLocation,
                                                 const Handle(Prs3d_Drawer)&       theDrawer,
                                                 const TColStd_SequenceOfReal&     theUIsoParams,
                                                 const TColStd_SequenceOfReal&     theVIsoParams);

  //! Computes isolines on surface and adds them to presentation.
  //! @param[in] thePresentation  the presentation.
  //! @param[in] theFace  the face.
  //! @param[in] theDrawer  the display settings.
  //! @param[in] theDeflection  the deflection value.
  Standard_EXPORT static void AddOnSurface(const Handle(Prs3d_Presentation)& thePresentation,
                                           const TopoDS_Face&                theFace,
                                           const Handle(Prs3d_Drawer)&       theDrawer,
                                           const Standard_Real               theDeflection);

  //! Computes isolines on surface and adds them to presentation.
  //! @param[in] theFace  the face
  //! @param[in] theDrawer  the display settings
  //! @param[in] theDeflection  the deflection value
  //! @param[out] theUPolylines  the sequence of result polylines
  //! @param[out] theVPolylines  the sequence of result polylines
  Standard_EXPORT static void AddOnSurface(const TopoDS_Face&          theFace,
                                           const Handle(Prs3d_Drawer)& theDrawer,
                                           const Standard_Real         theDeflection,
                                           Prs3d_NListOfSequenceOfPnt& theUPolylines,
                                           Prs3d_NListOfSequenceOfPnt& theVPolylines);

  //! Computes isolines on surface and adds them to presentation.
  //! @param[in] thePresentation  the presentation.
  //! @param[in] theSurface  the surface.
  //! @param[in] theDrawer  the display settings.
  //! @param[in] theDeflection  the deflection value.
  //! @param[in] theUIsoParams  the parameters of u isolines to compute.
  //! @param[in] theVIsoParams  the parameters of v isolines to compute.
  Standard_EXPORT static void AddOnSurface(const Handle(Prs3d_Presentation)&  thePresentation,
                                           const Handle(BRepAdaptor_Surface)& theSurface,
                                           const Handle(Prs3d_Drawer)&        theDrawer,
                                           const Standard_Real                theDeflection,
                                           const TColStd_SequenceOfReal&      theUIsoParams,
                                           const TColStd_SequenceOfReal&      theVIsoParams);

  //! Evaluate sequence of parameters for drawing uv isolines for a given face.
  //! @param[in] theFace  the face.
  //! @param[in] theNbIsoU  the number of u isolines.
  //! @param[in] theNbIsoV  the number of v isolines.
  //! @param[in] theUVLimit  the u, v parameter value limit.
  //! @param[out] theUIsoParams  the sequence of u isoline parameters.
  //! @param[out] theVIsoParams  the sequence of v isoline parameters.
  //! @param[out] theUmin  the lower U boundary of  theFace.
  //! @param[out] theUmax  the upper U boundary of  theFace.
  //! @param[out] theVmin  the lower V boundary of  theFace.
  //! @param[out] theVmax  the upper V boundary of  theFace.
  Standard_EXPORT static void UVIsoParameters(const TopoDS_Face&      theFace,
                                              const Standard_Integer  theNbIsoU,
                                              const Standard_Integer  theNbIsoV,
                                              const Standard_Real     theUVLimit,
                                              TColStd_SequenceOfReal& theUIsoParams,
                                              TColStd_SequenceOfReal& theVIsoParams,
                                              Standard_Real&          theUmin,
                                              Standard_Real&          theUmax,
                                              Standard_Real&          theVmin,
                                              Standard_Real&          theVmax);

public:
  //! Auxiliary structure defining 3D point on isoline.
  struct PntOnIso
  {
    gp_Pnt Pnt;   //!< 3D point
    double Param; //!< parameter along the line (for sorting)
  };

  //! Auxiliary structure defining segment of isoline.
  struct SegOnIso
  {

    PntOnIso Pnts[2];

    operator PntOnIso*() { return Pnts; }

    operator const PntOnIso*() const { return Pnts; }

    bool operator<(const SegOnIso& theOther) const
    {
      return Pnts[1].Param < theOther.Pnts[0].Param;
    }
  };

private:
  //! Computes isolines on surface.
  //! @param[in] theSurface  the surface
  //! @param[in] theDrawer  the display settings
  //! @param[in] theDeflection  the deflection value
  //! @param[in] theUIsoParams  the parameters of u isolines to compute
  //! @param[in] theVIsoParams  the parameters of v isolines to compute
  //! @param[out] theUPolylines  the sequence of result polylines
  //! @param[out] theVPolylines  the sequence of result polylines
  Standard_EXPORT static void addOnSurface(const Handle(BRepAdaptor_Surface)& theSurface,
                                           const Handle(Prs3d_Drawer)&        theDrawer,
                                           const Standard_Real                theDeflection,
                                           const TColStd_SequenceOfReal&      theUIsoParams,
                                           const TColStd_SequenceOfReal&      theVIsoParams,
                                           Prs3d_NListOfSequenceOfPnt&        theUPolylines,
                                           Prs3d_NListOfSequenceOfPnt&        theVPolylines);

  //! Computes isolines on triangulation.
  //! @param[in] thePresentation  the presentation
  //! @param[in] theTriangulation  the triangulation
  //! @param[in] theSurface  the definition of triangulated surface. The surface
  //!        adapter is used to precisely evaluate isoline points using surface
  //!        law and fit them on triangulation. If NULL is passed, the method will
  //!        use linear interpolation of triangle node's UV coordinates to evaluate
  //!        isoline points
  //! @param[in] theLocation  the location transformation defined for triangulation (surface)
  //! @param[in] theDrawer  the display settings
  //! @param[in] theUIsoParams  the parameters of u isolines to compute
  //! @param[in] theVIsoParams  the parameters of v isolines to compute
  //! @param[out] theUPolylines  the sequence of result polylines
  //! @param[out] theVPolylines  the sequence of result polylines
  Standard_EXPORT static void addOnTriangulation(const Handle(Poly_Triangulation)& theTriangulation,
                                                 const Handle(Geom_Surface)&       theSurface,
                                                 const TopLoc_Location&            theLocation,
                                                 const TColStd_SequenceOfReal&     theUIsoParams,
                                                 const TColStd_SequenceOfReal&     theVIsoParams,
                                                 Prs3d_NListOfSequenceOfPnt&       theUPolylines,
                                                 Prs3d_NListOfSequenceOfPnt&       theVPolylines);

  //! Find isoline segment on a triangle.
  //! @param[in] theSurface  the surface.
  //! @param[in] theIsU      when true than U isoline is specified, V isoline otherwise
  //! @param[in] theIsoline  the isoline in uv coordinates.
  //! @param[in] theNodesXYZ  the XYZ coordinates of triangle nodes.
  //! @param[in] theNodesUV  the UV coordinates of triangle nodes.
  //! @param[out] theSegment  the XYZ points of crossed triangle's links.
  //!                         with U cross point parameter for V isoline
  //!                         or V parameters for U isoline (depending on theIsU)
  //! @return TRUE if the isoline passes through the triangle.
  Standard_EXPORT static Standard_Boolean findSegmentOnTriangulation(
    const Handle(Geom_Surface)& theSurface,
    const bool                  theIsU,
    const gp_Lin2d&             theIsoline,
    const gp_Pnt*               theNodesXYZ,
    const gp_Pnt2d*             theNodesUV,
    SegOnIso&                   theSegment);
};

#endif // _StdPrs_Isolines_H__
