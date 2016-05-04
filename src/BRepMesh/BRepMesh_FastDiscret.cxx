// Created on: 1996-02-27
// Created by: Ekaterina SMIRNOVA
// Copyright (c) 1996-1999 Matra Datavision
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

#include <BRepMesh_FastDiscret.hxx>

#include <BRepMesh_WireChecker.hxx>
#include <BRepMesh_FastDiscretFace.hxx>
#include <BRepMesh_FaceAttribute.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <BRepMesh_PairOfPolygon.hxx>
#include <BRepMesh_Classifier.hxx>
#include <BRepMesh_EdgeParameterProvider.hxx>
#include <BRepMesh_IEdgeTool.hxx>
#include <BRepMesh_EdgeTessellator.hxx>
#include <BRepMesh_EdgeTessellationExtractor.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>

#include <Bnd_Box.hxx>
#include <BRepTools.hxx>
#include <BRepBndLib.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>

#include <Precision.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Extrema_LocateExtPC.hxx>

#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfCharacter.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>

#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <OSD_Parallel.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <NCollection_IncAllocator.hxx>

#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_PointRepresentation.hxx>

#include <vector>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_FastDiscret,Standard_Transient)

#define UVDEFLECTION 1.e-05

//=======================================================================
//function : BRepMesh_FastDiscret
//purpose  : 
//=======================================================================
BRepMesh_FastDiscret::BRepMesh_FastDiscret( const Bnd_Box&         theBox,
                                            const BRepMesh_FastDiscret::Parameters& theParams)
   :
  myMapdefle(1000, new NCollection_IncAllocator()),
  myBoundaryVertices(new BRepMesh::DMapOfVertexInteger),
  myBoundaryPoints(new BRepMesh::DMapOfIntegerPnt),
  myParameters(theParams),
  myDtotale(0.)
{ 
  if ( myParameters.Relative )
    BRepMesh_ShapeTool::BoxMaxDimension(theBox, myDtotale);
}

//=======================================================================
//function : InitSharedFaces
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::InitSharedFaces(const TopoDS_Shape& theShape)
{
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, mySharedFaces);
}

//=======================================================================
//function : Perform(shape)
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::Perform(const TopoDS_Shape& theShape)
{
  InitSharedFaces(theShape);

  std::vector<TopoDS_Face> aFaces;
  TopExp_Explorer anExplorer(theShape, TopAbs_FACE);
  for (; anExplorer.More(); anExplorer.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExplorer.Current());
    Add(aFace);
    aFaces.push_back(aFace);
  }

  OSD_Parallel::ForEach(aFaces.begin(), aFaces.end(), *this, !myParameters.InParallel);
}


//=======================================================================
//function : Process
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::Process(const TopoDS_Face& theFace) const
{
  Handle(BRepMesh_FaceAttribute) anAttribute;
  if (GetFaceAttribute(theFace, anAttribute))
  {
    try
    {
      OCC_CATCH_SIGNALS

      BRepMesh_FastDiscretFace aTool(myParameters.Angle, myParameters.MinSize, 
        myParameters.InternalVerticesMode, myParameters.ControlSurfaceDeflection);
      aTool.Perform(anAttribute);
    }
    catch (Standard_Failure)
    {
      anAttribute->SetStatus(BRepMesh_Failure);
    }
  }
}

//=======================================================================
//function : resetDataStructure
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::resetDataStructure()
{
  Handle(NCollection_IncAllocator) aAllocator;
  if (myAttribute->ChangeStructure().IsNull())
    aAllocator = new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE);
  else
    aAllocator = myAttribute->ChangeStructure()->Allocator();

  myAttribute->Clear();
  aAllocator->Reset(Standard_False);
  Handle(BRepMesh_DataStructureOfDelaun) aStructure = 
    new BRepMesh_DataStructureOfDelaun(aAllocator);

  const Standard_Real aTolU = myAttribute->ToleranceU();
  const Standard_Real aTolV = myAttribute->ToleranceV();
  const Standard_Real uCellSize = 14.0 * aTolU;
  const Standard_Real vCellSize = 14.0 * aTolV;

  aStructure->Data()->SetCellSize ( uCellSize, vCellSize);
  aStructure->Data()->SetTolerance( aTolU    , aTolV    );

  myAttribute->ChangeStructure() = aStructure;
}

//=======================================================================
//function : Add(face)
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_FastDiscret::Add(const TopoDS_Face& theFace)
{
  myAttribute.Nullify();
  GetFaceAttribute(theFace, myAttribute, Standard_True);

  try
  {
    OCC_CATCH_SIGNALS

    // Initialize face attributes
    if (!myAttribute->IsInitialized ())
      myAttribute->SetFace (theFace, myParameters.AdaptiveMin);
    
    BRepMesh::HIMapOfInteger&            aVertexEdgeMap = myAttribute->ChangeVertexEdgeMap();
    BRepMesh::HDMapOfShapePairOfPolygon& aInternalEdges = myAttribute->ChangeInternalEdges();

    resetDataStructure();

    Standard_Real defedge = myParameters.Deflection;
    Standard_Integer nbEdge = 0;
    Standard_Real savangle = myParameters.Angle;
    Standard_Real cdef;
    Standard_Real maxdef = 2.* BRepMesh_ShapeTool::MaxFaceTolerance(theFace);

    Standard_Real defface = 0.;
    if (!myParameters.Relative)
    {
      defedge = Max(UVDEFLECTION, defedge);
      defface = Max(myParameters.Deflection, maxdef);
    }

    const TopoDS_Face&                  aFace = myAttribute->Face();
    for (TopoDS_Iterator aWireIt(aFace); aWireIt.More(); aWireIt.Next())
    {
      for (TopoDS_Iterator aEdgeIt(aWireIt.Value()); aEdgeIt.More(); aEdgeIt.Next(), ++nbEdge)
      {
        const TopoDS_Edge& aEdge = TopoDS::Edge(aEdgeIt.Value());
        if (aEdge.IsNull())
          continue;
        if (myParameters.Relative)
        {
          if (!myMapdefle.IsBound(aEdge))
          {
            if (myEdges.IsBound(aEdge))
            {
              const BRepMesh_PairOfPolygon& aPair = myEdges.Find(aEdge);
              const Handle(Poly_PolygonOnTriangulation)& aPolygon = aPair.First();
              defedge = aPolygon->Deflection();
            }
            else
            {
              defedge = BRepMesh_ShapeTool::RelativeEdgeDeflection(
                aEdge, myParameters.Deflection, myDtotale, cdef);

              myParameters.Angle = savangle * cdef;
            }
          }
          else
          {
            defedge = myMapdefle(aEdge);
          }

          defface += defedge;
          defface = Max(maxdef, defface);

          if (!myMapdefle.IsBound(aEdge))
          {
            defedge = Max(UVDEFLECTION, defedge);
            myMapdefle.Bind(aEdge, defedge);
          }
        }
        else
        {
          if (!myMapdefle.IsBound(aEdge))
          {
            myMapdefle.Bind(aEdge, defedge);
          }
        }

        Standard_Real aFirstParam, aLastParam;
        Handle(Geom2d_Curve) aCurve2d = 
          BRep_Tool::CurveOnSurface(aEdge, aFace, aFirstParam, aLastParam);

        if (aCurve2d.IsNull())
          continue;
        Handle(Geom2dAdaptor_HCurve) aPCurve =
          new Geom2dAdaptor_HCurve(aCurve2d, aFirstParam, aLastParam);

        add(aEdge, aPCurve, defedge);
        myParameters.Angle = savangle;
      }
    }

    if ( nbEdge == 0 || aVertexEdgeMap->Extent() < 3 )
    {
      myAttribute->ChangeStructure().Nullify();
      myAttribute->SetStatus(BRepMesh_Failure);
      return myAttribute->GetStatus();
    }

    if ( myParameters.Relative )
    {
      defface = defface / nbEdge;
    }
    else
    {
      defface = myParameters.Deflection;
    }

    defface = Max(maxdef, defface);

    TopLoc_Location aLoc;
    Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(aFace, aLoc);
    const Handle(BRepAdaptor_HSurface)& gFace = myAttribute->Surface();

    if ( aTriangulation.IsNull() )
    {
      Standard_Real xCur, yCur;
      Standard_Real maxX, minX, maxY, minY;

      minX = minY = 1.e100;
      maxX = maxY =-1.e100;

      Standard_Integer ipn = 0;
      Standard_Integer i1 = 1;
      for ( i1 = 1; i1 <= aVertexEdgeMap->Extent(); ++i1 )
      {
        const BRepMesh_Vertex& aVertex = 
          myAttribute->ChangeStructure()->GetNode(aVertexEdgeMap->FindKey(i1));

        ++ipn;

        xCur = aVertex.Coord().X();
        yCur = aVertex.Coord().Y();

        minX = Min(xCur, minX);
        maxX = Max(xCur, maxX);
        minY = Min(yCur, minY);
        maxY = Max(yCur, maxY);
      }

      Standard_Real myumin = minX;
      Standard_Real myumax = maxX;
      Standard_Real myvmin = minY;
      Standard_Real myvmax = maxY;

      const Standard_Real umin = gFace->FirstUParameter();
      const Standard_Real umax = gFace->LastUParameter();
      const Standard_Real vmin = gFace->FirstVParameter();
      const Standard_Real vmax = gFace->LastVParameter();

      if (myumin < umin || myumax > umax)
      {
        if (gFace->IsUPeriodic())
        {
          if ((myumax - myumin) > (umax - umin))
            myumax = myumin + (umax - umin);
        }
        else
        {
          if (umin > myumin)
            myumin = umin;

          if (umax < myumax)
            myumax = umax;
        }
      }

      if (myvmin < vmin || myvmax > vmax)
      {
        if (gFace->IsVPeriodic())
        {
          if ((myvmax - myvmin) > (vmax - vmin))
            myvmax = myvmin + (vmax - vmin);
        }
        else
        {
          if ( vmin > myvmin )
            myvmin = vmin;

          if (vmax < myvmax)
            myvmax = vmax;
        }
      }

      GeomAbs_SurfaceType aSurfType = gFace->GetType();
      // Fast verification of the validity of calculated limits.
      // If wrong, sure a problem of pcurve.
      if (aSurfType == GeomAbs_BezierSurface &&
         (myumin < -0.5 || myumax > 1.5 || myvmin < -0.5 || myvmax > 1.5) )
      {
        myAttribute->ChangeStructure().Nullify();
        myAttribute->SetStatus(BRepMesh_Failure);
        return myAttribute->GetStatus();
      }

      //define parameters for correct parametrics
      Standard_Real deltaX = 1.0;
      Standard_Real deltaY = 1.0;

      {
        Standard_Real aTolU, aTolV;
        myAttribute->ChangeStructure()->Data()->GetTolerance(aTolU, aTolV);
        const Standard_Real aTol = Sqrt(aTolU * aTolU + aTolV * aTolV);

        BRepMesh::HClassifier& aClassifier = myAttribute->ChangeClassifier();
        BRepMesh_WireChecker aDFaceChecker(aFace, aTol, aInternalEdges, 
          aVertexEdgeMap, myAttribute->ChangeStructure(),
          myumin, myumax, myvmin, myvmax, myParameters.InParallel );

        aDFaceChecker.ReCompute(aClassifier);
        BRepMesh_Status aCheckStatus = aDFaceChecker.Status();

        if (aCheckStatus == BRepMesh_SelfIntersectingWire)
        {
          Standard_Integer nbmaill = 0;
          Standard_Real eps = Precision::Confusion();
          while (nbmaill < 5 && aCheckStatus != BRepMesh_ReMesh)
          {
            ++nbmaill;

            resetDataStructure();

            for (TopoDS_Iterator aWireIt(aFace); aWireIt.More(); aWireIt.Next())
            {
              for (TopoDS_Iterator aEdgeIt(aWireIt.Value()); aEdgeIt.More(); aEdgeIt.Next(), ++nbEdge)
              {
                const TopoDS_Edge& anEdge = TopoDS::Edge(aEdgeIt.Value());
                if (anEdge.IsNull())
                  continue;
                if (myEdges.IsBound(anEdge))
                  myEdges.UnBind(anEdge);

                defedge = Max(myMapdefle(anEdge) / 3.0, eps);
                myMapdefle.Bind(anEdge, defedge);

                Standard_Real aFirstParam, aLastParam;
                Handle(Geom2d_Curve) aCurve2d =
                  BRep_Tool::CurveOnSurface(anEdge, aFace, aFirstParam, aLastParam);
                if (aCurve2d.IsNull())
                  continue;

                Handle(Geom2dAdaptor_HCurve) aPCurve =
                  new Geom2dAdaptor_HCurve(aCurve2d, aFirstParam, aLastParam);
                add(anEdge, aPCurve, defedge);
              }
            }

            aDFaceChecker.ReCompute(aClassifier);
            if (aDFaceChecker.Status() == BRepMesh_NoError)
              aCheckStatus = BRepMesh_ReMesh;
          }
        }

        myAttribute->SetStatus(aCheckStatus);
        if (!myAttribute->IsValid())
        {
          myAttribute->ChangeStructure().Nullify();
          return myAttribute->GetStatus();
        }
      }

      // try to find the real length:
      // akm (bug OCC16) : We must calculate these measures in non-singular
      //     parts of face. Let's try to compute average value of three
      //     (umin, (umin+umax)/2, umax), and respectively for v.
      //                 vvvvv
      Standard_Real longu = 0.0, longv = 0.0; //, last , first;
      gp_Pnt P11, P12, P21, P22, P31, P32;

      Standard_Real du = 0.05 * ( myumax - myumin );
      Standard_Real dv = 0.05 * ( myvmax - myvmin );
      Standard_Real dfuave = 0.5 * ( myumin + myumax );
      Standard_Real dfvave = 0.5 * ( myvmin + myvmax );
      Standard_Real dfucur, dfvcur;

      // U loop
      gFace->D0(myumin, myvmin, P11);
      gFace->D0(myumin, dfvave, P21);
      gFace->D0(myumin, myvmax, P31);
      for (i1=1, dfucur=myumin+du; i1 <= 20; i1++, dfucur+=du)
      {
        gFace->D0(dfucur, myvmin, P12);
        gFace->D0(dfucur, dfvave, P22);
        gFace->D0(dfucur, myvmax, P32);
        longu += ( P11.Distance(P12) + P21.Distance(P22) + P31.Distance(P32) );
        P11 = P12;
        P21 = P22;
        P31 = P32;
      }

      // V loop
      gFace->D0(myumin, myvmin, P11);
      gFace->D0(dfuave, myvmin, P21);
      gFace->D0(myumax, myvmin, P31);
      for (i1=1, dfvcur=myvmin+dv; i1 <= 20; i1++, dfvcur+=dv)
      {
        gFace->D0(myumin, dfvcur, P12);
        gFace->D0(dfuave, dfvcur, P22);
        gFace->D0(myumax, dfvcur, P32);
        longv += ( P11.Distance(P12) + P21.Distance(P22) + P31.Distance(P32) );
        P11 = P12;
        P21 = P22;
        P31 = P32;
      }

      longu /= 3.;
      longv /= 3.;
      // akm (bug OCC16) ^^^^^

      if (longu <= 1.e-16 || longv <= 1.e-16)
      {
        //yes, it is seen!!
        myAttribute->ChangeStructure().Nullify();
        myAttribute->SetStatus(BRepMesh_Failure);
        return myAttribute->GetStatus();
      }


      if (aSurfType == GeomAbs_Torus)
      {
        gp_Torus Tor = gFace->Torus();
        Standard_Real r = Tor.MinorRadius(), R = Tor.MajorRadius();
        Standard_Real Du, Dv;//, pasu, pasv;

        Dv = Max(1.0e0 - (defface/r),0.0e0) ;
        Standard_Real oldDv = 2.0 * ACos (Dv);
        oldDv = Min(oldDv, myParameters.Angle);
        Dv  =  0.9*oldDv; //TWOTHIRD * oldDv;
        Dv = oldDv;

        Standard_Integer nbV = Max((Standard_Integer)((myvmax-myvmin)/Dv), 2);
        Dv = (myvmax-myvmin)/(nbV+1);

        Standard_Real ru = R + r;
        if ( ru > 1.e-16 )
        {
          Du = Max(1.0e0 - (defface/ru),0.0e0);
          Du  = (2.0 * ACos (Du));
          Du = Min(Du, myParameters.Angle);
          Standard_Real aa = sqrt(Du*Du + oldDv*oldDv);

          if (aa < gp::Resolution())
          {
            myAttribute->ChangeStructure().Nullify();
            return myAttribute->GetStatus();
          }

          Du = Du * Min(oldDv, Du) / aa;
        }
        else
        {
          Du = Dv;
        }

        Standard_Integer nbU = Max((Standard_Integer)((myumax-myumin)/Du), 2);
        nbU = Max(nbU, (Standard_Integer)(nbV*(myumax-myumin)*R/((myvmax-myvmin)*r)/5.));
      
        Du = (myumax-myumin)/(nbU+1);
        //-- DeltaX and DeltaY are chosen so that to avoid "jumping" 
        //-- of points on the grid
        deltaX = Du;
        deltaY = Dv;
      }
      else if (aSurfType == GeomAbs_Cylinder)
      {
        gp_Cylinder Cyl = gFace->Cylinder();
        Standard_Real R = Cyl.Radius();

        // Calculate parameters for iteration in U direction
        Standard_Real Du = 1.0 - (defface/R);
        if (Du < 0.0)
          Du = 0.0;

        Du = 2.0 * ACos (Du);
        if (Du > myParameters.Angle)
          Du = myParameters.Angle;

        deltaX = Du / longv;
        deltaY = 1.;
      }
      else
      {
        deltaX = (myumax-myumin)/longu;
        deltaY = (myvmax-myvmin)/longv;
      }

      // Restore face attribute
      myAttribute->SetDefFace(defface);
      myAttribute->SetUMax(myumax);
      myAttribute->SetVMax(myvmax);
      myAttribute->SetUMin(myumin);
      myAttribute->SetVMin(myvmin);
      myAttribute->SetDeltaX(deltaX);
      myAttribute->SetDeltaY(deltaY);
    }

    myAttribute->ChangeMeshNodes() = 
      myAttribute->ChangeStructure()->Data()->Vertices();
  }
  catch(Standard_Failure)
  {
    myAttribute->SetStatus(BRepMesh_Failure);
  }

  myAttribute->ChangeStructure().Nullify();
  return myAttribute->GetStatus();
}

//=======================================================================
//function : getEdgeAttributes
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_FastDiscret::getEdgeAttributes(
  const TopoDS_Edge&                      theEdge,
  const Handle(Geom2dAdaptor_HCurve)&     thePCurve,
  const Standard_Real                     theDefEdge,
  BRepMesh_FastDiscret::EdgeAttributes&   theAttributes) const
{
  EdgeAttributes& aEAttr = theAttributes;

  // Get vertices
  TopExp::Vertices(theEdge, aEAttr.FirstVertex, aEAttr.LastVertex);
  if (aEAttr.FirstVertex.IsNull() || aEAttr.LastVertex.IsNull())
    return Standard_False;

  // Get range on 2d curve
  const TopoDS_Face& aFace = myAttribute->Face();
  BRep_Tool::Range(theEdge, aFace, aEAttr.FirstParam, aEAttr.LastParam);

  // Get end points on 2d curve
  BRep_Tool::UVPoints(theEdge, aFace, aEAttr.FirstUV, aEAttr.LastUV);

  aEAttr.IsSameUV =
    aEAttr.FirstUV.IsEqual(aEAttr.LastUV, Precision::PConfusion());
  if (aEAttr.IsSameUV)
  {
    // 1. is it really sameUV without being degenerated
    gp_Pnt2d uvF, uvL;
    thePCurve->D0(thePCurve->FirstParameter(), uvF);
    thePCurve->D0(thePCurve->LastParameter(),  uvL);

    if (!aEAttr.FirstUV.IsEqual(uvF, Precision::PConfusion()))
      aEAttr.FirstUV = uvF;

    if (!aEAttr.LastUV.IsEqual(uvL, Precision::PConfusion()))
      aEAttr.LastUV = uvL;

    aEAttr.IsSameUV =
      aEAttr.FirstUV.IsEqual(aEAttr.LastUV, Precision::PConfusion());
  }

  //Control tolerance of vertices
  const Handle(BRepAdaptor_HSurface)& gFace = myAttribute->Surface();
  gp_Pnt pFirst = gFace->Value(aEAttr.FirstUV.X(), aEAttr.FirstUV.Y());
  gp_Pnt pLast  = gFace->Value(aEAttr.LastUV.X(),  aEAttr.LastUV.Y());

  Standard_Real aSqDist = pFirst.SquareDistance(BRep_Tool::Pnt(aEAttr.FirstVertex));
  aSqDist = Max(aSqDist, pLast.SquareDistance(BRep_Tool::Pnt(aEAttr.LastVertex)));

  aEAttr.Deflection = Max(theDefEdge, BRep_Tool::Tolerance(theEdge));
  aEAttr.Deflection = Max(aEAttr.Deflection, sqrt(aSqDist));

  return Standard_True;
}

//=======================================================================
//function : registerEdgeVertices
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::registerEdgeVertices(
  BRepMesh_FastDiscret::EdgeAttributes& theAttributes,
  Standard_Integer&                     ipf,
  Standard_Integer&                     ivf,
  Standard_Integer&                     isvf,
  Standard_Integer&                     ipl,
  Standard_Integer&                     ivl,
  Standard_Integer&                     isvl)
{
  EdgeAttributes& aEAttr = theAttributes;
  if (aEAttr.FirstVExtractor.IsNull())
  {
    // Use edge geometry to produce tesselation.
    aEAttr.FirstVExtractor = 
      new TopoDSVExplorer(aEAttr.FirstVertex, aEAttr.IsSameUV, aEAttr.LastVertex);
  }

  if (aEAttr.LastVExtractor.IsNull())
  {
    // Use edge geometry to produce tesselation.
    aEAttr.LastVExtractor = 
      new TopoDSVExplorer(aEAttr.LastVertex, aEAttr.IsSameUV, aEAttr.FirstVertex);
  }

  // Process first vertex
  ipf = myAttribute->GetVertexIndex(aEAttr.FirstVExtractor, Standard_True);
  Standard_Real aMinDist = 2. * BRep_Tool::Tolerance(aEAttr.FirstVertex);
  gp_XY aTmpUV1 = BRepMesh_ShapeTool::FindUV(ipf, aEAttr.FirstUV, aMinDist, myAttribute);

  myAttribute->AddNode(ipf, aTmpUV1, BRepMesh_Frontier, ivf, isvf);

  // Process last vertex
  ipl = aEAttr.LastVertex.IsSame(aEAttr.FirstVertex) ? ipf :
    myAttribute->GetVertexIndex(aEAttr.LastVExtractor, Standard_True);
  aMinDist = 2. * BRep_Tool::Tolerance(aEAttr.LastVertex);
  gp_XY aTmpUV2 = BRepMesh_ShapeTool::FindUV(ipl, aEAttr.LastUV, aMinDist, myAttribute);

  myAttribute->AddNode(ipl, aTmpUV2, BRepMesh_Frontier, ivl, isvl);

  // Update edge deflection
  const Handle(BRepAdaptor_HSurface)& gFace = myAttribute->Surface();
  gp_Pnt aPntE1 = gFace->Value(aEAttr.FirstUV.X(), aEAttr.FirstUV.Y());
  gp_Pnt aPntFound1 = gFace->Value(aTmpUV1.X(), aTmpUV1.Y());
  Standard_Real aSqDist = aPntE1.SquareDistance(aPntFound1);
  gp_Pnt aPntE2 = gFace->Value(aEAttr.LastUV.X(), aEAttr.LastUV.Y());
  gp_Pnt aPntFound2 = gFace->Value(aTmpUV2.X(), aTmpUV2.Y());
  aSqDist = Max(aSqDist, aPntE2.SquareDistance(aPntFound2));
  aEAttr.Deflection = Max(aEAttr.Deflection, sqrt(aSqDist));
}

//=======================================================================
//function : add
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::add(
  const TopoDS_Edge&                      theEdge,
  const Handle(Geom2dAdaptor_HCurve)&     thePCurve,
  const Standard_Real                     theDefEdge)
{
  const TopAbs_Orientation orEdge = theEdge.Orientation();
  if (orEdge == TopAbs_EXTERNAL)
    return;

  EdgeAttributes aEAttr;
  if (!getEdgeAttributes(theEdge, thePCurve, theDefEdge, aEAttr))
    return;

  if (!myEdges.IsBound(theEdge))
  {
    update(theEdge, thePCurve, theDefEdge, aEAttr);
    return;
  }

  Standard_Integer ipf, ivf, isvf, ipl, ivl, isvl;
  registerEdgeVertices(aEAttr, ipf, ivf, isvf, ipl, ivl, isvl);

  // If this Edge has been already checked and it is not degenerated, 
  // the points of the polygon calculated at the first check are retrieved :

  // retrieve the polygone:
  const BRepMesh_PairOfPolygon&              aPair    = myEdges.Find(theEdge);
  const Handle(Poly_PolygonOnTriangulation)& aPolygon = aPair.First();
  const TColStd_Array1OfInteger&             aNodes   = aPolygon->Nodes();
  Handle(TColStd_HArray1OfReal)              aParams  = aPolygon->Parameters();

  // creation anew:
  const Standard_Integer  aNodesNb = aNodes.Length();
  TColStd_Array1OfInteger aNewNodes (1, aNodesNb);
  TColStd_Array1OfReal    aNewParams(1, aNodesNb);

  aNewNodes (1) = isvf;
  aNewParams(1) = aEAttr.FirstParam;

  aNewNodes (aNodesNb) = isvl;
  aNewParams(aNodesNb) = aEAttr.LastParam;

  const TopoDS_Face& aFace = myAttribute->Face();
  if (!BRepMesh_ShapeTool::IsDegenerated(theEdge, aFace))
  {
    BRepMesh_EdgeParameterProvider aProvider(theEdge, aFace, aParams);
    for (Standard_Integer i = 2; i < aNodesNb; ++i)
    {
      const Standard_Integer aPointId = aNodes(i);
      const gp_Pnt& aPnt = myBoundaryPoints->Find(aPointId);

      const Standard_Real aParam = aProvider.Parameter(i, aPnt);
      gp_Pnt2d aUV = thePCurve->Value(aParam);

      Standard_Integer iv2, isv;
      myAttribute->AddNode(aPointId, aUV.Coord(), BRepMesh_OnCurve, iv2, isv);

      aNewNodes (i) = isv;
      aNewParams(i) = aParam;
    }
  }

  Handle(Poly_PolygonOnTriangulation) P1 = 
    new Poly_PolygonOnTriangulation(aNewNodes, aNewParams);

  storePolygon(theEdge, P1, aEAttr.Deflection);
}

//=======================================================================
//function : update(edge)
//purpose  :
//=======================================================================
void BRepMesh_FastDiscret::update(
  const TopoDS_Edge&                                theEdge,
  const Handle(Geom2dAdaptor_HCurve)&               theC2d,
  const Standard_Real                               theDefEdge,
  BRepMesh_FastDiscret::EdgeAttributes&             theAttributes)
{
  EdgeAttributes& aEAttr = theAttributes;

  const TopoDS_Face& aFace = myAttribute->Face();
  Handle(BRepMesh_IEdgeTool) aEdgeTool;
  // Try to find existing tessellation.
  for (Standard_Integer i = 1; aEdgeTool.IsNull(); ++i)
  {
    TopLoc_Location aLoc;
    Handle(Poly_Triangulation) aTriangulation;
    Handle(Poly_PolygonOnTriangulation) aPolygon;
    BRep_Tool::PolygonOnTriangulation(theEdge, aPolygon, aTriangulation, aLoc, i);

    if (aPolygon.IsNull())
      break;

    if (aTriangulation.IsNull() || !aPolygon->HasParameters())
      continue;

    if (aPolygon->Deflection() > 1.1 * theDefEdge)
      continue;

    const TColgp_Array1OfPnt&      aNodes   = aTriangulation->Nodes();
    const TColStd_Array1OfInteger& aIndices = aPolygon->Nodes();
    Handle(TColStd_HArray1OfReal)  aParams  = aPolygon->Parameters();

    aEAttr.FirstVExtractor = new PolyVExplorer(aEAttr.FirstVertex, 
      aEAttr.IsSameUV, aEAttr.LastVertex, aIndices(1), aNodes, aLoc);

    aEAttr.LastVExtractor = new PolyVExplorer(aEAttr.LastVertex, 
      aEAttr.IsSameUV, aEAttr.FirstVertex, aIndices(aIndices.Length()), aNodes, aLoc);

    aEdgeTool = new BRepMesh_EdgeTessellationExtractor(theEdge, theC2d, 
      aFace, aTriangulation, aPolygon, aLoc);
  }

  if (aEdgeTool.IsNull())
  {
    aEdgeTool = new BRepMesh_EdgeTessellator(theEdge, myAttribute, 
      mySharedFaces, theDefEdge, myParameters.Angle, myParameters.MinSize);
  }

  Standard_Integer ipf, ivf, isvf, ipl, ivl, isvl;
  registerEdgeVertices(aEAttr, ipf, ivf, isvf, ipl, ivl, isvl);

  Handle(Poly_PolygonOnTriangulation) P1, P2;
  if (BRepMesh_ShapeTool::IsDegenerated(theEdge, aFace))
  {
    // two nodes
    Standard_Integer aNewNodesArr[] = {isvf, isvl};
    Standard_Integer aNewNodInStructArr[] = {ipf, ipl};
    Standard_Real aNewParamsArr[] = {aEAttr.FirstParam, aEAttr.LastParam};
    TColStd_Array1OfInteger aNewNodes      (aNewNodesArr[0], 1, 2);
    TColStd_Array1OfInteger aNewNodInStruct(aNewNodInStructArr[0], 1, 2);
    TColStd_Array1OfReal    aNewParams     (aNewParamsArr[0], 1, 2);

    P1 = new Poly_PolygonOnTriangulation(aNewNodes,       aNewParams);
    P2 = new Poly_PolygonOnTriangulation(aNewNodInStruct, aNewParams);
  }
  else
  {
    const Standard_Integer  aNodesNb = aEdgeTool->NbPoints();
    // Allocate the memory for arrays aNewNodesVec, aNewNodesInStructVec, aNewParamsVec
    // only once using the buffer aBuf.
    TColStd_Array1OfCharacter aBuf(1, aNodesNb * (2*sizeof(Standard_Integer) + sizeof(Standard_Real)));
    TColStd_Array1OfInteger aNewNodesVec(*reinterpret_cast<const Standard_Integer*>
      (&aBuf(1)), 1, aNodesNb);
    TColStd_Array1OfInteger aNewNodesInStructVec(*reinterpret_cast<const Standard_Integer*>
      (&aBuf(1 + aNodesNb*sizeof(Standard_Integer))), 1, aNodesNb);
    TColStd_Array1OfReal    aNewParamsVec(*reinterpret_cast<const Standard_Real*>
      (&aBuf(1 + aNodesNb*2*sizeof(Standard_Integer))), 1, aNodesNb);

    Standard_Integer aNodesCount = 1;
    aNewNodesInStructVec(aNodesCount) = ipf;
    aNewNodesVec        (aNodesCount) = isvf;
    aNewParamsVec       (aNodesCount) = aEAttr.FirstParam;

    ++aNodesCount;
    Standard_Integer aPrevNodeId  = ivf;
    Standard_Integer aLastPointId = myAttribute->LastPointId();
    for (Standard_Integer i = 2; i < aNodesNb; ++i)
    {
      gp_Pnt        aPnt;
      gp_Pnt2d      aUV;
      Standard_Real aParam;
      if (!aEdgeTool->Value(i, aParam, aPnt, aUV))
        continue;

      // Imitate index of 3d point in order to not to add points to map without necessity.
      Standard_Integer iv2, isv;
      myAttribute->AddNode(aLastPointId + 1, aUV.Coord(), BRepMesh_Frontier, iv2, isv);
      if (aPrevNodeId == iv2)
        continue;

      // Ok, now we can add point to the map.
      myBoundaryPoints->Bind (++aLastPointId, aPnt);

      aNewNodesInStructVec(aNodesCount) = aLastPointId;
      aNewNodesVec        (aNodesCount) = isv;
      aNewParamsVec       (aNodesCount) = aParam;

      ++aNodesCount;
      aPrevNodeId = iv2;
    }

    aNewNodesInStructVec(aNodesCount) = ipl;
    aNewNodesVec        (aNodesCount) = isvl;
    aNewParamsVec       (aNodesCount) = aEAttr.LastParam;

    TColStd_Array1OfInteger aNewNodes      (aNewNodesVec.First (),        1, aNodesCount);
    TColStd_Array1OfInteger aNewNodInStruct(aNewNodesInStructVec.First(), 1, aNodesCount);
    TColStd_Array1OfReal    aNewParams     (aNewParamsVec.First(),        1, aNodesCount);

    P1 = new Poly_PolygonOnTriangulation(aNewNodes,       aNewParams);
    P2 = new Poly_PolygonOnTriangulation(aNewNodInStruct, aNewParams);
  }

  storePolygon(theEdge, P1, aEAttr.Deflection);
  storePolygonSharedData(theEdge, P2, aEAttr.Deflection);
}

//=======================================================================
//function : storeInternalPolygon
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::storePolygon(
  const TopoDS_Edge&                         theEdge,
  Handle(Poly_PolygonOnTriangulation)&       thePolygon,
  const Standard_Real                        theDeflection)
{
  thePolygon->Deflection(theDeflection);
  BRepMesh::HDMapOfShapePairOfPolygon& aInternalEdges = myAttribute->ChangeInternalEdges();
  if (aInternalEdges->IsBound(theEdge))
  {
    BRepMesh_PairOfPolygon& aPair = aInternalEdges->ChangeFind(theEdge);
    if (theEdge.Orientation() == TopAbs_REVERSED)
      aPair.Append(thePolygon);
    else
      aPair.Prepend(thePolygon);

    return;
  }

  BRepMesh_PairOfPolygon aPair;
  aPair.Append(thePolygon);
  aInternalEdges->Bind(theEdge, aPair);
}

//=======================================================================
//function : storePolygonSharedData
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::storePolygonSharedData(
  const TopoDS_Edge&                         theEdge,
  Handle(Poly_PolygonOnTriangulation)&       thePolygon,
  const Standard_Real                        theDeflection)
{
  thePolygon->Deflection(theDeflection);
  BRepMesh_PairOfPolygon aPair;
  aPair.Append(thePolygon);
  myEdges.Bind(theEdge, aPair);
}

//=======================================================================
//function : GetFaceAttribute
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_FastDiscret::GetFaceAttribute(
  const TopoDS_Face&              theFace,
  Handle(BRepMesh_FaceAttribute)& theAttribute,
  const Standard_Boolean          isForceCreate) const
{
  if (myAttributes.IsBound(theFace))
  {
    theAttribute = myAttributes(theFace);
    return Standard_True;
  }
  else if (isForceCreate)
  {
    theAttribute = new BRepMesh_FaceAttribute(myBoundaryVertices, myBoundaryPoints);
    myAttributes.Bind(theFace, theAttribute);
  }

  return Standard_False;
}

//=======================================================================
//function : RemoveFaceAttribute
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::RemoveFaceAttribute(const TopoDS_Face& theFace)
{
  if (myAttributes.IsBound(theFace))
    myAttributes.UnBind(theFace);
}
