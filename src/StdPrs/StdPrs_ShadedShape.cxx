// Created on: 1993-09-23
// Created by: Jean-Louis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
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

#include <StdPrs_ShadedShape.hxx>

#include <Bnd_Box.hxx>
#include <BRepTools.hxx>
#include <BRepBndLib.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Group.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_List.hxx>
#include <Precision.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Poly_Connect.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TColgp_Array1OfDir.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

namespace
{

  //! Computes wireframe presentation for free wires and vertices
  void wireframeFromShape (const Handle (Prs3d_Presentation)& thePrs,
                           const TopoDS_Shape&                theShape,
                           const Handle (Prs3d_Drawer)&       theDrawer)
  {
    Standard_Boolean aDrawAllVerticesFlag = (theDrawer->VertexDrawMode() == Prs3d_VDM_All);

    if (!aDrawAllVerticesFlag && theShape.ShapeType() != TopAbs_COMPOUND)
    {
      return;
    }

    TopExp_Explorer aShapeIter (theShape, TopAbs_FACE);
    if (!aShapeIter.More())
    {
      // compound contains no shaded elements at all
      StdPrs_WFShape::Add (thePrs, theShape, theDrawer);
      return;
    }

    // We have to create a compound and collect all subshapes not drawn by the shading algo.
    // This includes:
    // - isolated edges
    // - isolated vertices, if aDrawAllVerticesFlag == Standard_False
    // - all shape's vertices, if aDrawAllVerticesFlag == Standard_True
    TopoDS_Compound aCompoundWF;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound (aCompoundWF);
    Standard_Boolean hasElement = Standard_False;

    // isolated edges
    for (aShapeIter.Init (theShape, TopAbs_EDGE, TopAbs_FACE); aShapeIter.More(); aShapeIter.Next())
    {
      hasElement = Standard_True;
      aBuilder.Add (aCompoundWF, aShapeIter.Current());
    }
    // isolated or all vertices
    aShapeIter.Init (theShape, TopAbs_VERTEX, aDrawAllVerticesFlag ? TopAbs_SHAPE : TopAbs_EDGE);
    for (; aShapeIter.More(); aShapeIter.Next())
    {
      hasElement = Standard_True;
      aBuilder.Add (aCompoundWF, aShapeIter.Current());
    }
    if (hasElement)
    {
      StdPrs_WFShape::Add (thePrs, aCompoundWF, theDrawer);
    }
  }

  //! Computes special wireframe presentation for faces without triangulation.
  void wireframeNoTriangFacesFromShape (const Handle(Prs3d_Presentation)& thePrs,
                                        const TopoDS_Shape&               theShape,
                                        const Handle(Prs3d_Drawer)&       theDrawer)
  {
    TopoDS_Compound aCompoundWF;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound (aCompoundWF);
    TopLoc_Location aLoc;
    Standard_Boolean hasElement = Standard_False;

    for (TopExp_Explorer aShapeIter(theShape, TopAbs_FACE); aShapeIter.More(); aShapeIter.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face (aShapeIter.Current());
      const Handle(Poly_Triangulation) aTriang = BRep_Tool::Triangulation (aFace, aLoc);
      if (aTriang.IsNull())
      {
        hasElement = Standard_True;
        aBuilder.Add (aCompoundWF, aFace);
      }
    }

    if (hasElement)
    {
      Standard_Integer aPrevUIsoNb = theDrawer->UIsoAspect()->Number();
      Standard_Integer aPrevVIsoNb = theDrawer->VIsoAspect()->Number();
      theDrawer->UIsoAspect()->SetNumber (5);
      theDrawer->VIsoAspect()->SetNumber (5);

      StdPrs_WFShape::Add (thePrs, aCompoundWF, theDrawer);

      theDrawer->UIsoAspect()->SetNumber (aPrevUIsoNb);
      theDrawer->VIsoAspect()->SetNumber (aPrevVIsoNb);
    }
  }

  //! Gets triangulation of every face of shape and fills output array of triangles
  static Handle(Graphic3d_ArrayOfTriangles) fillTriangles (const TopoDS_Shape&    theShape,
                                                           const Standard_Boolean theHasTexels,
                                                           const gp_Pnt2d&        theUVOrigin,
                                                           const gp_Pnt2d&        theUVRepeat,
                                                           const gp_Pnt2d&        theUVScale)
  {
    Handle(Poly_Triangulation) aT;
    TopLoc_Location aLoc;
    gp_Pnt aPoint;
    Standard_Integer aNbTriangles = 0;
    Standard_Integer aNbVertices  = 0;

    // Precision for compare square distances
    const Standard_Real aPreci = Precision::SquareConfusion();

    TopExp_Explorer aFaceIt(theShape, TopAbs_FACE);
    for (; aFaceIt.More(); aFaceIt.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face(aFaceIt.Current());
      aT = BRep_Tool::Triangulation (aFace, aLoc);
      if (!aT.IsNull())
      {
        aNbTriangles += aT->NbTriangles();
        aNbVertices  += aT->NbNodes();
      }
    }
    if (aNbVertices  <  3 || aNbTriangles <= 0)
    {
      return Handle(Graphic3d_ArrayOfTriangles)();
    }

    Handle(Graphic3d_ArrayOfTriangles) anArray = new Graphic3d_ArrayOfTriangles (aNbVertices, 3 * aNbTriangles,
                                                                                 Standard_True, Standard_False, theHasTexels);
    Standard_Real aUmin (0.0), aUmax (0.0), aVmin (0.0), aVmax (0.0), dUmax (0.0), dVmax (0.0);
    for (aFaceIt.Init (theShape, TopAbs_FACE); aFaceIt.More(); aFaceIt.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face(aFaceIt.Current());
      aT = BRep_Tool::Triangulation (aFace, aLoc);
      if (aT.IsNull())
      {
        continue;
      }
      const gp_Trsf& aTrsf = aLoc.Transformation();

      // Determinant of transform matrix less then 0 means that mirror transform applied.
      Standard_Boolean isMirrored = aTrsf.VectorialPart().Determinant() < 0;

      Poly_Connect aPolyConnect (aT);
      // Extracts vertices & normals from nodes
      const TColgp_Array1OfPnt&   aNodes   = aT->Nodes();
      const TColgp_Array1OfPnt2d& aUVNodes = aT->UVNodes();
      TColgp_Array1OfDir aNormals (aNodes.Lower(), aNodes.Upper());
      StdPrs_ToolTriangulatedShape::Normal (aFace, aPolyConnect, aNormals);

      if (theHasTexels)
      {
        BRepTools::UVBounds (aFace, aUmin, aUmax, aVmin, aVmax);
        dUmax = (aUmax - aUmin);
        dVmax = (aVmax - aVmin);
      }

      const Standard_Integer aDecal = anArray->VertexNumber();
      for (Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
      {
        aPoint = aNodes (aNodeIter);
        if (!aLoc.IsIdentity())
        {
          aPoint.Transform (aTrsf);

          aNormals (aNodeIter) = aNormals (aNodeIter).Transformed (aTrsf);
        }

        if (theHasTexels && aUVNodes.Upper() == aNodes.Upper())
        {
          const gp_Pnt2d aTexel = gp_Pnt2d ((-theUVOrigin.X() + (theUVRepeat.X() * (aUVNodes (aNodeIter).X() - aUmin)) / dUmax) / theUVScale.X(),
                                            (-theUVOrigin.Y() + (theUVRepeat.Y() * (aUVNodes (aNodeIter).Y() - aVmin)) / dVmax) / theUVScale.Y());
          anArray->AddVertex (aPoint, aNormals (aNodeIter), aTexel);
        }
        else
        {
          anArray->AddVertex (aPoint, aNormals (aNodeIter));
        }
      }

      // Fill array with vertex and edge visibility info
      const Poly_Array1OfTriangle& aTriangles = aT->Triangles();
      Standard_Integer anIndex[3];
      for (Standard_Integer aTriIter = 1; aTriIter <= aT->NbTriangles(); ++aTriIter)
      {
        if ((aFace.Orientation() == TopAbs_REVERSED) ^ isMirrored)
        {
          aTriangles (aTriIter).Get (anIndex[0], anIndex[2], anIndex[1]);
        }
        else
        {
          aTriangles (aTriIter).Get (anIndex[0], anIndex[1], anIndex[2]);
        }

        gp_Pnt aP1 = aNodes (anIndex[0]);
        gp_Pnt aP2 = aNodes (anIndex[1]);
        gp_Pnt aP3 = aNodes (anIndex[2]);

        gp_Vec aV1 (aP1, aP2);
        if (aV1.SquareMagnitude() <= aPreci)
        {
          continue;
        }
        gp_Vec aV2 (aP2, aP3);
        if (aV2.SquareMagnitude() <= aPreci)
        {
          continue;
        }
        gp_Vec aV3 (aP3, aP1);
        if (aV3.SquareMagnitude() <= aPreci)
        {
          continue;
        }
        aV1.Normalize();
        aV2.Normalize();
        aV1.Cross (aV2);
        if (aV1.SquareMagnitude() > aPreci)
        {
          anArray->AddEdge (anIndex[0] + aDecal);
          anArray->AddEdge (anIndex[1] + aDecal);
          anArray->AddEdge (anIndex[2] + aDecal);
        }
      }
    }
    return anArray;
  }

  //! Prepare shaded presentation for specified shape
  static Standard_Boolean shadeFromShape (const TopoDS_Shape&               theShape,
                                          const Handle(Prs3d_Presentation)& thePrs,
                                          const Handle(Prs3d_Drawer)&       theDrawer,
                                          const Standard_Boolean            theHasTexels,
                                          const gp_Pnt2d&                   theUVOrigin,
                                          const gp_Pnt2d&                   theUVRepeat,
                                          const gp_Pnt2d&                   theUVScale,
                                          const Standard_Boolean            theIsClosed)
  {
    Handle(Graphic3d_ArrayOfTriangles) aPArray = fillTriangles (theShape, theHasTexels, theUVOrigin, theUVRepeat, theUVScale);
    if (aPArray.IsNull())
    {
      return Standard_False;
    }

    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePrs);
    aGroup->SetClosed (theIsClosed);
    if (!theDrawer->ShadingAspectGlobal())
    {
      Handle(Graphic3d_AspectFillArea3d) anAsp = theDrawer->ShadingAspect()->Aspect();
      theIsClosed ? anAsp->SuppressBackFace() : anAsp->AllowBackFace();
      aGroup->SetGroupPrimitivesAspect (anAsp);
    }
    aGroup->AddPrimitiveArray (aPArray);
    return Standard_True;
  }

  //! Compute boundary presentation for faces of the shape.
  static void computeFaceBoundaries (const TopoDS_Shape&               theShape,
                                     const Handle(Prs3d_Presentation)& thePrs,
                                     const Handle(Prs3d_Drawer)&       theDrawer)
  {
    // collection of all triangulation nodes on edges
    // for computing boundaries presentation
    NCollection_List<Handle(TColgp_HArray1OfPnt)> aNodeCollection;
    Standard_Integer aNodeNumber = 0;

    TopLoc_Location aTrsf;

    // explore all boundary edges
    TopTools_IndexedDataMapOfShapeListOfShape anEdgesMap;
    TopExp::MapShapesAndAncestors (
      theShape, TopAbs_EDGE, TopAbs_FACE, anEdgesMap);

    Standard_Integer anEdgeIdx = 1;
    for ( ; anEdgeIdx <= anEdgesMap.Extent (); anEdgeIdx++)
    {
      // reject free edges
      const TopTools_ListOfShape& aFaceList = anEdgesMap.FindFromIndex (anEdgeIdx);
      if (aFaceList.Extent() == 0)
        continue;

      // take one of the shared edges and get edge triangulation
      const TopoDS_Face& aFace  = TopoDS::Face (aFaceList.First ());
      const TopoDS_Edge& anEdge = TopoDS::Edge (anEdgesMap.FindKey (anEdgeIdx));

      Handle(Poly_Triangulation) aTriangulation =
        BRep_Tool::Triangulation (aFace, aTrsf);

      if (aTriangulation.IsNull ())
        continue;

      Handle(Poly_PolygonOnTriangulation) anEdgePoly =
        BRep_Tool::PolygonOnTriangulation (anEdge, aTriangulation, aTrsf);

      if (anEdgePoly.IsNull ())
        continue;

      // get edge nodes indexes from face triangulation
      const TColgp_Array1OfPnt& aTriNodes = aTriangulation->Nodes ();
      const TColStd_Array1OfInteger& anEdgeNodes = anEdgePoly->Nodes ();

      if (anEdgeNodes.Length () < 2)
        continue;

      // collect the edge nodes
      Handle(TColgp_HArray1OfPnt) aCollected =
        new TColgp_HArray1OfPnt (anEdgeNodes.Lower (), anEdgeNodes.Upper ());

      Standard_Integer aNodeIdx = anEdgeNodes.Lower ();
      for ( ; aNodeIdx <= anEdgeNodes.Upper (); aNodeIdx++)
      {
        // node index in face triangulation
        Standard_Integer aTriIndex = anEdgeNodes.Value (aNodeIdx);

        // get node and apply location transformation to the node
        gp_Pnt aTriNode = aTriNodes.Value (aTriIndex);
        if (!aTrsf.IsIdentity ())
          aTriNode.Transform (aTrsf);

        // add node to the boundary array
        aCollected->SetValue (aNodeIdx, aTriNode);
      }

      aNodeNumber += anEdgeNodes.Length ();
      aNodeCollection.Append (aCollected);
    }

    // check if it possible to continue building the presentation
    if (aNodeNumber == 0)
      return;

    // allocate polyline array for presentation
    Standard_Integer aSegmentEdgeNb = 
      (aNodeNumber - aNodeCollection.Extent()) * 2;

    Handle(Graphic3d_ArrayOfSegments) aSegments = 
      new Graphic3d_ArrayOfSegments (aNodeNumber, aSegmentEdgeNb);

    // build presentation for edge bondaries
    NCollection_List<Handle(TColgp_HArray1OfPnt)>::Iterator 
      aCollIt (aNodeCollection);

    // the edge index is increased in each iteration step to
    // avoid contiguous segments between different face edges.
    for ( ; aCollIt.More(); aCollIt.Next () )
    {
      const Handle(TColgp_HArray1OfPnt)& aNodeArray = aCollIt.Value ();

      Standard_Integer aNodeIdx = aNodeArray->Lower ();

      // add first node (this node is not shared with previous segment).
      // for each face edge, indices for sharing nodes 
      // between segments begin from the first added node.
      Standard_Integer aSegmentEdge = 
        aSegments->AddVertex (aNodeArray->Value (aNodeIdx));

      // add subsequent nodes and provide edge indexes for sharing
      // the nodes between the sequential segments.
      for ( aNodeIdx++; aNodeIdx <= aNodeArray->Upper (); aNodeIdx++ )
      {
        aSegments->AddVertex (aNodeArray->Value (aNodeIdx));
        aSegments->AddEdge (  aSegmentEdge);
        aSegments->AddEdge (++aSegmentEdge);
      }
    }

    // set up aspect and add polyline data
    Handle(Graphic3d_AspectLine3d) aBoundaryAspect = 
      theDrawer->FaceBoundaryAspect ()->Aspect ();

    Handle(Graphic3d_Group) aPrsGrp = Prs3d_Root::CurrentGroup (thePrs);
    aPrsGrp->SetGroupPrimitivesAspect (aBoundaryAspect);
    aPrsGrp->AddPrimitiveArray (aSegments);
  }
};

// =======================================================================
// function : ExploreSolids
// purpose  :
// =======================================================================
void StdPrs_ShadedShape::ExploreSolids (const TopoDS_Shape&    theShape,
                                        const BRep_Builder&    theBuilder,
                                        TopoDS_Compound&       theClosed,
                                        TopoDS_Compound&       theOpened,
                                        const Standard_Boolean theIgnore1DSubShape)
{
  if (theShape.IsNull())
  {
    return;
  }

  switch (theShape.ShapeType())
  {
    case TopAbs_COMPOUND:
    case TopAbs_COMPSOLID:
    {
      for (TopoDS_Iterator anIter (theShape); anIter.More(); anIter.Next())
      {
        ExploreSolids (anIter.Value(), theBuilder, theClosed, theOpened, theIgnore1DSubShape);
      }
      return;
    }
    case TopAbs_SOLID:
    {
      for (TopoDS_Iterator anIter (theShape); anIter.More(); anIter.Next())
      {
        const TopoDS_Shape& aSubShape   = anIter.Value();
        const Standard_Boolean isClosed = aSubShape.ShapeType() == TopAbs_SHELL &&
                                          BRep_Tool::IsClosed (aSubShape)       &&
                                          StdPrs_ToolTriangulatedShape::IsTriangulated (aSubShape);
        theBuilder.Add (isClosed ? theClosed : theOpened, aSubShape);
      }
      return;
    }
    case TopAbs_SHELL:
    case TopAbs_FACE:
    {
      theBuilder.Add (theOpened, theShape);
      return;
    }
    case TopAbs_WIRE:
    case TopAbs_EDGE:
    case TopAbs_VERTEX:
    {
      if (!theIgnore1DSubShape)
      {
        theBuilder.Add (theOpened, theShape);
      }
      return;
    }
    case TopAbs_SHAPE:
    default:
      return;
  }
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void StdPrs_ShadedShape::Add (const Handle(Prs3d_Presentation)& thePrs,
                              const TopoDS_Shape&               theShape,
                              const Handle(Prs3d_Drawer)&       theDrawer,
                              const StdPrs_Volume               theVolume)
{
  gp_Pnt2d aDummy;
  StdPrs_ShadedShape::Add (thePrs, theShape, theDrawer,
                           Standard_False, aDummy, aDummy, aDummy, theVolume);
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void StdPrs_ShadedShape::Add (const Handle (Prs3d_Presentation)& thePrs,
                              const TopoDS_Shape&                theShape,
                              const Handle (Prs3d_Drawer)&       theDrawer,
                              const Standard_Boolean             theHasTexels,
                              const gp_Pnt2d&                    theUVOrigin,
                              const gp_Pnt2d&                    theUVRepeat,
                              const gp_Pnt2d&                    theUVScale,
                              const StdPrs_Volume                theVolume)
{
  if (theShape.IsNull())
  {
    return;
  }

  // add wireframe presentation for isolated edges and vertices
  wireframeFromShape (thePrs, theShape, theDrawer);

  // Use automatic re-triangulation with deflection-check logic only if this feature is enable
  if (theDrawer->IsAutoTriangulation())
  {
    // Triangulation completeness is important for "open-closed" analysis - perform tessellation beforehand
    StdPrs_ToolTriangulatedShape::Tessellate (theShape, theDrawer);
  }

  // add special wireframe presentation for faces without triangulation
  wireframeNoTriangFacesFromShape (thePrs, theShape, theDrawer);

  // The shape types listed below need advanced analysis as potentially containing
  // both closed and open parts. Solids are also included, because they might
  // contain non-manifold parts inside (internal open shells)
  if ((theShape.ShapeType() == TopAbs_COMPOUND
    || theShape.ShapeType() == TopAbs_COMPSOLID
    || theShape.ShapeType() == TopAbs_SOLID)
   &&  theVolume == StdPrs_Volume_Autodetection)
  {
    // collect two compounds: for opened and closed (solid) sub-shapes
    TopoDS_Compound anOpened, aClosed;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound (aClosed);
    aBuilder.MakeCompound (anOpened);
    ExploreSolids (theShape, aBuilder, aClosed, anOpened, Standard_True);

    TopoDS_Iterator aShapeIter (aClosed);
    if (aShapeIter.More())
    {
      shadeFromShape (aClosed, thePrs, theDrawer,
                      theHasTexels, theUVOrigin, theUVRepeat, theUVScale, Standard_True);
    }

    aShapeIter.Initialize (anOpened);
    if (aShapeIter.More())
    {
      shadeFromShape (anOpened, thePrs, theDrawer,
                      theHasTexels, theUVOrigin, theUVRepeat, theUVScale, Standard_False);
    }
  }
  else
  {
    // if the shape type is not compound, composolid or solid, use autodetection back-facing filled
    shadeFromShape (theShape, thePrs, theDrawer,
                    theHasTexels, theUVOrigin, theUVRepeat, theUVScale,
                    (theVolume == StdPrs_Volume_Closed ? Standard_True : Standard_False));
  }

  if (theDrawer->FaceBoundaryDraw())
  {
    computeFaceBoundaries (theShape, thePrs, theDrawer);
  }
}
