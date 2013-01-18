// File:      StdPrs_ShadedShape.cxx
// Created:   23 Sep 1993
// Author:    Jean-Louis FRENKEL
// Copyright: OPEN CASCADE 2012

#include <StdPrs_ShadedShape.hxx>

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <BRepMesh_DiscretFactory.hxx>
#include <BRepMesh_DiscretRoot.hxx>
#include <BRepTools.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Group.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Poly_Connect.hxx>
#include <Poly_Triangulation.hxx>
#include <StdPrs_ToolShadedShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TColgp_Array1OfDir.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopoDS_Compound.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <TopExp.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <NCollection_List.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Prs3d_LineAspect.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Aspect_PolygonOffsetMode.hxx>

#define MAX2(X, Y)	  (Abs(X) > Abs(Y) ? Abs(X) : Abs(Y))
#define MAX3(X, Y, Z)	(MAX2 (MAX2 (X, Y), Z))

namespace
{
  // =======================================================================
  // function : GetDeflection
  // purpose  :
  // =======================================================================
  static Standard_Real GetDeflection (const TopoDS_Shape&         theShape,
                                      const Handle(Prs3d_Drawer)& theDrawer)
  {
    Standard_Real aDeflection = theDrawer->MaximalChordialDeviation();
    if (theDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE)
    {
      Bnd_Box aBndBox;
      BRepBndLib::Add (theShape, aBndBox, Standard_False);
      if (!aBndBox.IsVoid())
      {
        Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
        aBndBox.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
        aDeflection = MAX3 (aXmax-aXmin, aYmax-aYmin, aZmax-aZmin) * theDrawer->DeviationCoefficient() * 4.0;
      }
    }
    return aDeflection;
  }

  // =======================================================================
  // function : ShadeFromShape
  // purpose  :
  // =======================================================================
  static Standard_Boolean ShadeFromShape (const TopoDS_Shape&                theShape,
                                          const Handle (Prs3d_Presentation)& thePresentation,
                                          const Handle (Prs3d_Drawer)&       theDrawer,
                                          const Standard_Boolean             theHasTexels,
                                          const gp_Pnt2d&                    theUVOrigin,
                                          const gp_Pnt2d&                    theUVRepeat,
                                          const gp_Pnt2d&                    theUVScale)
  {
    StdPrs_ToolShadedShape SST;
    Handle(Poly_Triangulation) T;
    TopLoc_Location aLoc;
    gp_Pnt p;
    Standard_Integer decal;
    Standard_Integer t[3], n[3];
    Standard_Integer nbTriangles = 0, nbVertices = 0;
    Standard_Real    aUmin (0.0), aUmax (0.0), aVmin (0.0), aVmax (0.0), dUmax (0.0), dVmax (0.0);

    // precision for compare square distances
    const double aPreci = Precision::SquareConfusion();

    if (!theDrawer->ShadingAspectGlobal())
    {
      Handle(Graphic3d_AspectFillArea3d) anAsp = theDrawer->ShadingAspect()->Aspect();
      if (StdPrs_ToolShadedShape::IsClosed (theShape))
      {
        anAsp->SuppressBackFace();
      }
      else
      {
        anAsp->AllowBackFace();
      }
      Prs3d_Root::CurrentGroup (thePresentation)->SetGroupPrimitivesAspect (anAsp);
    }

    for (SST.Init (theShape); SST.MoreFace(); SST.NextFace())
    {
      const TopoDS_Face& aFace = SST.CurrentFace();
      T = SST.Triangulation (aFace, aLoc);
      if (!T.IsNull())
      {
        nbTriangles += T->NbTriangles();
        nbVertices  += T->NbNodes();
      }
    }

    if (nbVertices > 2 && nbTriangles > 0)
    {
      Handle(Graphic3d_ArrayOfTriangles) aPArray
        = new Graphic3d_ArrayOfTriangles (nbVertices, 3 * nbTriangles,
                                          Standard_True, Standard_False, theHasTexels, Standard_True);
      for (SST.Init (theShape); SST.MoreFace(); SST.NextFace())
      {
        const TopoDS_Face& aFace = SST.CurrentFace();
        T = SST.Triangulation (aFace, aLoc);
        if (T.IsNull())
        {
          continue;
        }
        const gp_Trsf& aTrsf = aLoc.Transformation();
        Poly_Connect pc (T);
        // Extracts vertices & normals from nodes
        const TColgp_Array1OfPnt&   aNodes   = T->Nodes();
        const TColgp_Array1OfPnt2d& aUVNodes = T->UVNodes();
        TColgp_Array1OfDir aNormals (aNodes.Lower(), aNodes.Upper());
        SST.Normal (aFace, pc, aNormals);

        if (theHasTexels)
        {
          BRepTools::UVBounds (aFace, aUmin, aUmax, aVmin, aVmax);
          dUmax = (aUmax - aUmin);
          dVmax = (aVmax - aVmin);
        }

        decal = aPArray->VertexNumber();
        for (Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
        {
          p = aNodes (aNodeIter);
          if (!aLoc.IsIdentity())
          {
            p.Transform (aTrsf);
            aNormals (aNodeIter).Transform (aTrsf);
          }

          if (theHasTexels && aUVNodes.Upper() == aNodes.Upper())
          {
            const gp_Pnt2d aTexel = gp_Pnt2d ((-theUVOrigin.X() + (theUVRepeat.X() * (aUVNodes (aNodeIter).X() - aUmin)) / dUmax) / theUVScale.X(),
                                              (-theUVOrigin.Y() + (theUVRepeat.Y() * (aUVNodes (aNodeIter).Y() - aVmin)) / dVmax) / theUVScale.Y());
            aPArray->AddVertex (p, aNormals (aNodeIter), aTexel);
          }
          else
          {
            aPArray->AddVertex (p, aNormals (aNodeIter));
          }
        }

        // Fill parray with vertex and edge visibillity info
        const Poly_Array1OfTriangle& aTriangles = T->Triangles();
        for (Standard_Integer aTriIter = 1; aTriIter <= T->NbTriangles(); ++aTriIter)
        {
          pc.Triangles (aTriIter, t[0], t[1], t[2]);
          if (SST.Orientation (aFace) == TopAbs_REVERSED)
            aTriangles (aTriIter).Get (n[0], n[2], n[1]);
          else
            aTriangles (aTriIter).Get (n[0], n[1], n[2]);

          gp_Pnt P1 = aNodes (n[0]);
          gp_Pnt P2 = aNodes (n[1]);
          gp_Pnt P3 = aNodes (n[2]);

          gp_Vec V1 (P1, P2);
          if (V1.SquareMagnitude() <= aPreci)
          {
            continue;
          }
          gp_Vec V2 (P2, P3);
          if (V2.SquareMagnitude() <= aPreci)
          {
            continue;
          }
          gp_Vec V3 (P3, P1);
          if (V3.SquareMagnitude() <= aPreci)
          {
            continue;
          }
          V1.Normalize();
          V2.Normalize();
          V1.Cross (V2);
          if (V1.SquareMagnitude() > aPreci)
          {
            aPArray->AddEdge (n[0] + decal, t[0] == 0);
            aPArray->AddEdge (n[1] + decal, t[1] == 0);
            aPArray->AddEdge (n[2] + decal, t[2] == 0);
          }
        }
      }
      Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPArray);
    }
    return Standard_True;
  }

  // =======================================================================
  // function : ComputeFaceBoundaries
  // purpose  : Compute boundary presentation for faces of the shape.
  // =======================================================================
  static void ComputeFaceBoundaries (const TopoDS_Shape& theShape,
                                     const Handle (Prs3d_Presentation)& thePresentation,
                                     const Handle (Prs3d_Drawer)& theDrawer)
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

    Handle(Graphic3d_Group) aPrsGrp = Prs3d_Root::NewGroup (thePresentation);
    aPrsGrp->SetGroupPrimitivesAspect (aBoundaryAspect);
    aPrsGrp->AddPrimitiveArray (aSegments);
  }
};

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void StdPrs_ShadedShape::Add (const Handle(Prs3d_Presentation)& thePresentation,
                                    const TopoDS_Shape& theShape,
                                    const Handle(Prs3d_Drawer)& theDrawer)
{
  gp_Pnt2d aDummy;
  StdPrs_ShadedShape::Add (thePresentation, theShape, theDrawer,
                           Standard_False, aDummy, aDummy, aDummy);
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void StdPrs_ShadedShape::Add (const Handle (Prs3d_Presentation)& thePresentation,
                              const TopoDS_Shape&                theShape,
                              const Handle (Prs3d_Drawer)&       theDrawer,
                              const Standard_Boolean             theHasTexels,
                              const gp_Pnt2d&                    theUVOrigin,
                              const gp_Pnt2d&                    theUVRepeat,
                              const gp_Pnt2d&                    theUVScale)
{
  if (theShape.IsNull())
  {
    return;
  }

  if (theShape.ShapeType() == TopAbs_COMPOUND)
  {
    TopExp_Explorer ex;
    ex.Init (theShape, TopAbs_FACE);
    if (ex.More())
    {
      TopoDS_Compound CO;
      BRep_Builder aBuilder;
      aBuilder.MakeCompound (CO);
      Standard_Boolean hasElement = Standard_False;

      // il faut presenter les edges  isoles.
      for (ex.Init (theShape, TopAbs_EDGE, TopAbs_FACE); ex.More(); ex.Next())
      {
        hasElement = Standard_True;
        aBuilder.Add (CO, ex.Current());
      }
      // il faut presenter les vertex isoles.
      for (ex.Init (theShape, TopAbs_VERTEX, TopAbs_EDGE); ex.More(); ex.Next())
      {
        hasElement = Standard_True;
        aBuilder.Add (CO, ex.Current());
      }
      if (hasElement)
      {
        StdPrs_WFShape::Add (thePresentation, CO, theDrawer);
      }
    }
    else
    {
      StdPrs_WFShape::Add (thePresentation, theShape, theDrawer);
    }
  }
  Standard_Real aDeflection = GetDeflection (theShape, theDrawer);

  // Check if it is possible to avoid unnecessary recomputation
  // of shape triangulation
  if (!BRepTools::Triangulation (theShape, aDeflection))
  {
    BRepTools::Clean (theShape);

    // retrieve meshing tool from Factory
    Handle(BRepMesh_DiscretRoot) aMeshAlgo = BRepMesh_DiscretFactory::Get().Discret (theShape,
                                                                                     aDeflection,
                                                                                     theDrawer->HLRAngle());
    if (!aMeshAlgo.IsNull())
      aMeshAlgo->Perform();
  }

  ShadeFromShape (theShape, thePresentation, theDrawer,
                  theHasTexels, theUVOrigin, theUVRepeat, theUVScale);

  if (theDrawer->IsFaceBoundaryDraw ())
  {
    ComputeFaceBoundaries (theShape, thePresentation, theDrawer);
  }
}
