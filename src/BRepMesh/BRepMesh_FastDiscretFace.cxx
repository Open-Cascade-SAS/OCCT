// Created by: Ekaterina SMIRNOVA
// Copyright (c) 2008-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepMesh_FastDiscretFace.ixx>

#include <Adaptor3d_IsoCurve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <BRepMesh_ListOfXY.hxx>
#include <BRepMesh_Array1OfVertexOfDelaun.hxx>
#include <BRepMesh_ListIteratorOfListOfVertex.hxx>
#include <BRepMesh_ListIteratorOfListOfXY.hxx>
#include <BRepMesh_PairOfPolygon.hxx>
#include <BRepMesh_DataMapIteratorOfDataMapOfShapePairOfPolygon.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRepMesh_ClassifierPtr.hxx>
#include <BRep_Builder.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_TVertex.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <ElSLib.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <SortTools_ShellSortOfReal.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <TCollection_CompareOfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <NCollection_Map.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomLib.hxx>
#include <Bnd_Box2d.hxx>

#define UVDEFLECTION 1.e-05

static Standard_Real FUN_CalcAverageDUV(TColStd_Array1OfReal& P, const Standard_Integer PLen)
{
  Standard_Integer i, j, n = 0;
  Standard_Real p, result = 0.;

  for(i = 1; i <= PLen; i++)
  {
    // Sort
    for(j = i + 1; j <= PLen; j++)
    {
      if(P(i) > P(j))
      {
        p = P(i);
        P(i) = P(j);
        P(j) = p;
      }
    }
    // Accumulate
    if (i != 1)
    {
      p = Abs(P(i) - P(i-1));
      if(p > 1.e-7)
      {
        result += p;
        n++;
      }
    }
  }
  return (n? (result / (Standard_Real) n) : -1.);
}

//=======================================================================
//function : BRepMesh_FastDiscretFace
//purpose  : 
//=======================================================================
BRepMesh_FastDiscretFace::BRepMesh_FastDiscretFace
                          (const Standard_Real     theAngle,
                           const Standard_Boolean  theWithShare) : 
  myAngle(theAngle), myWithShare(theWithShare), myNbLocat(0), 
  myInternalVerticesMode(Standard_True)
{
  myAllocator = new NCollection_IncAllocator(64000);
}

//=======================================================================
//function : Add(face)
//purpose  : 
//=======================================================================

void BRepMesh_FastDiscretFace::Add(const TopoDS_Face&                    theFace,
                                   const Handle(BRepMesh_FaceAttribute)& theAttrib,
                                   const TopTools_DataMapOfShapeReal&    theMapDefle,
                                   const TopTools_MutexForShapeProvider& theMutexProvider)
{
#ifndef DEB_MESH
  try
  {
    OCC_CATCH_SIGNALS
#endif
    TopoDS_Face face = theFace;
    TopLoc_Location loc;

    const Handle(Poly_Triangulation)& aFaceTrigu = BRep_Tool::Triangulation(face, loc);
    if ( aFaceTrigu.IsNull() )
      return;

    myAttrib = theAttrib;
    face.Orientation(TopAbs_FORWARD);
    myStructure.Nullify();
    Handle(NCollection_IncAllocator) anAlloc = Handle(NCollection_IncAllocator)::DownCast(myAllocator);
    anAlloc->Reset(Standard_False);  
    myStructure=new BRepMesh_DataStructureOfDelaun(anAlloc);
    
    Standard_Real umax   = myAttrib->GetUMax();
    Standard_Real umin   = myAttrib->GetUMin();
    Standard_Real vmax   = myAttrib->GetVMax();
    Standard_Real vmin   = myAttrib->GetVMin();

    Standard_Real aTolU = Max( Precision::PConfusion(), (umax - umin) * UVDEFLECTION );
    Standard_Real aTolV = Max( Precision::PConfusion(), (vmax - vmin) * UVDEFLECTION );
    Standard_Real uCellSize = 14 * aTolU;
    Standard_Real vCellSize = 14 * aTolV;

    myStructure->Data().SetCellSize ( uCellSize, vCellSize );
    myStructure->Data().SetTolerance( aTolU, aTolV );


    BRepAdaptor_Surface  BS(face, Standard_False);
    Handle(BRepAdaptor_HSurface) gFace = new BRepAdaptor_HSurface(BS);
    
    GeomAbs_SurfaceType thetype;
    thetype = BS.GetType();
    
    TopAbs_Orientation orFace = face.Orientation();

    if (!myWithShare)        
      myVertices.Clear();

    myListver.Clear();
    myVemap.Clear();
    myLocation2d.Clear();
    myInternaledges.Clear();

    Standard_Integer i = 1;
    Standard_Integer ipn = 0;
 
    TopoDS_Iterator exW(face);
    for (; exW.More(); exW.Next()) {
      const TopoDS_Shape& aWire = exW.Value();
      if (aWire.ShapeType() != TopAbs_WIRE)
        continue;
      TopoDS_Iterator ex(aWire);
      for(; ex.More(); ex.Next()) {
        const TopoDS_Edge& edge = TopoDS::Edge(ex.Value());
        if(edge.IsNull())
          continue;
        RestoreStructureFromTriangulation(edge, face, gFace, aFaceTrigu, theMapDefle(edge), loc, theMutexProvider);
      }
    }
    
    //////////////////////////////////////////////////////////// 
    //add internal vertices after self-intersection check
    Standard_Integer nbVertices = 0;
    if(myInternalVerticesMode) {
      for(TopExp_Explorer ex(face,TopAbs_VERTEX ,TopAbs_EDGE); ex.More(); ex.Next()) {
        const TopoDS_Vertex& aVert = TopoDS::Vertex(ex.Current());
        Add(aVert,face,gFace);
      }
      nbVertices = myVemap.Extent();
    }
    
    // essai de determination de la longueur vraie:
    // akm (bug OCC16) : We must calculate these measures in non-singular
    //     parts of face. Let`s try to compute average value of three
    //     (umin, (umin+umax)/2, umax), and respectively for v.
    //                 vvvvv
    //Standard_Real longu = 0.0, longv = 0.0; //, last , first;
    //gp_Pnt P11, P12, P21, P22, P31, P32;

    Standard_Real deltaX = myAttrib->GetDeltaX();
    Standard_Real deltaY = myAttrib->GetDeltaY();
    
    TColStd_Array1OfInteger tabvert_corr(1, nbVertices);
    gp_Pnt2d p2d;
    
    // Check the necessity to fill the map of parameters
    const Standard_Boolean useUVParam = (thetype == GeomAbs_Torus ||
                                         thetype == GeomAbs_BezierSurface ||
                                         thetype == GeomAbs_BSplineSurface);
    myUParam.Clear(); 
    myVParam.Clear();
  
    BRepMesh_VertexTool aMoveNodes(myVemap.Extent(), myAllocator);
    
    aMoveNodes.SetCellSize ( uCellSize / deltaX, vCellSize / deltaY);
    aMoveNodes.SetTolerance( aTolU     / deltaX, aTolV     / deltaY);

    for (i = 1; i <= myStructure->NbNodes(); i++)
    {
      const BRepMesh_Vertex& v = myStructure->GetNode(i);
      p2d = v.Coord();
      if (useUVParam) {
        myUParam.Add(p2d.X());
        myVParam.Add(p2d.Y());
      }
      gp_XY res;
      res.SetCoord((p2d.X() - umin ) / deltaX,
                   (p2d.Y() - vmin ) / deltaY);
      BRepMesh_Vertex v_new(res,v.Location3d(),v.Movability());
      const BRepMesh_ListOfInteger& alist = myStructure->GetNodeList(i);
      aMoveNodes.Add(v_new, alist);
      tabvert_corr(i) = i;
    }
    myStructure->ReplaceNodes(aMoveNodes);
    
    Standard_Boolean rajout;
    
    BRepMesh_ClassifierPtr& classifier = theAttrib->GetClassifier();

    switch (thetype)
    {
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
      rajout = Standard_True;
      break;
    default:
      rajout = Standard_False;
    }  

    BRepMesh_Delaun trigu(myStructure, tabvert_corr, orFace==TopAbs_FORWARD);
    
    //removed all free edges from triangulation
    Standard_Integer nbLinks = myStructure->NbLinks();
    for( i = 1; i <= nbLinks; i++ ) 
    {
      if( myStructure->ElemConnectedTo(i).Extent() < 1 )
      {
        BRepMesh_Edge& anEdge = (BRepMesh_Edge&)trigu.GetEdge(i);
        if ( anEdge.Movability() == BRepMesh_Deleted )
          continue;

        anEdge.SetMovability(BRepMesh_Free);
        myStructure->RemoveLink(i);
      }
    }

    Standard_Boolean isaline;
    isaline = ((umax-umin) < UVDEFLECTION) || ((vmax-vmin) < UVDEFLECTION);
    
    Standard_Real aDef = -1;
    if (!isaline && myStructure->ElemOfDomain().Extent() > 0) {
      TColStd_ListOfInteger badTri, nulTri;
      
      if(!rajout)
      {
        aDef = Control(gFace, theAttrib->GetDefFace(), myListver, badTri, nulTri, trigu, Standard_True);
        if( aDef > theAttrib->GetDefFace() || aDef < 0.)
          rajout = Standard_True;
      }

      if(!rajout) {
        if(useUVParam) {
          if(BS.IsUClosed()) {
            if(myVParam.Extent() > 2) {
              rajout = Standard_True;
            }
          }
          if(BS.IsVClosed()) {
            if(myUParam.Extent() > 2) {
              rajout = Standard_True;
            }
          }
        }
      }

      if(rajout){
        InternalVertices(gFace, myListver, theAttrib->GetDefFace(), classifier);
        
        if (myListver.Extent() > 0) {
          BRepMesh_Array1OfVertexOfDelaun verttab(1, myListver.Extent());
          BRepMesh_ListIteratorOfListOfVertex itVer(myListver);
          ipn = 1;
          for (; itVer.More(); itVer.Next())
            verttab(ipn++) = itVer.Value();
          trigu.AddVertices(verttab);
        }
        //control internal points
        BRepMesh_ListOfVertex vvlist;
        aDef = Control(gFace, theAttrib->GetDefFace(), vvlist, badTri, nulTri, trigu, Standard_False);
        myListver.Append(vvlist);
      }
    }

    //modify myStructure back
    aMoveNodes.SetCellSize ( uCellSize, vCellSize );
    aMoveNodes.SetTolerance( aTolU    , aTolV     );
    for (i = 1; i <= myStructure->NbNodes(); i++)
    {
      const BRepMesh_Vertex& v = myStructure->GetNode(i);
      p2d = v.Coord();
      gp_XY res;
      res.SetCoord(p2d.X() * deltaX + umin, p2d.Y() * deltaY + vmin);
      BRepMesh_Vertex v_new(res,v.Location3d(),v.Movability());
      const BRepMesh_ListOfInteger& alist = myStructure->GetNodeList(i);
      aMoveNodes.Add(v_new, alist);
    }
    myStructure->ReplaceNodes(aMoveNodes);
  
    AddInShape(face, (aDef < 0.0)? theAttrib->GetDefFace() : aDef, theMutexProvider);
#ifndef DEB_MESH
  }
  catch(Standard_Failure)
  {
    BRep_Builder B;
    Handle(Poly_Triangulation) TNull;
    B.UpdateFace(theFace,TNull);
  }
#endif // DEB_MESH
  myStructure.Nullify();
  myAttrib.Nullify();
}

//=======================================================================
//function : RestoreStructureFromTriangulation(edge)
//purpose  : Restore structure of Delaun from triangulation on face
//=======================================================================
Standard_Boolean BRepMesh_FastDiscretFace::RestoreStructureFromTriangulation
                               (const TopoDS_Edge&                  theEdge,
                                const TopoDS_Face&                  theFace,
                                const Handle(BRepAdaptor_HSurface)& theSurf,
                                const Handle(Poly_Triangulation)&   theTrigu,
                                const Standard_Real                 theDefEdge,
                                const TopLoc_Location&              theLoc,
                                const TopTools_MutexForShapeProvider& theMutexProvider)
{
  // 2d vertex indices
  TopAbs_Orientation orEdge = theEdge.Orientation();
  // Get end points on 2d curve
  gp_Pnt2d uvFirst, uvLast;
  // oan: changes for right restoring of triangulation data from face & edges
  Handle(Poly_PolygonOnTriangulation) Poly;

  {
    // lock mutex during querying data from edge curves to prevent parallel change of the same data
    Standard_Mutex* aMutex = theMutexProvider.GetMutex(theEdge);
    Standard_Mutex::Sentry aSentry (aMutex);

    Poly = BRep_Tool::PolygonOnTriangulation(theEdge, theTrigu, theLoc);
    if (Poly.IsNull() || !Poly->HasParameters())
      return Standard_False;

    BRep_Tool::UVPoints(theEdge, theFace, uvFirst, uvLast);
  }

  // Get vertices
  TopoDS_Vertex pBegin, pEnd;
  TopExp::Vertices(theEdge,pBegin,pEnd);

  const Standard_Boolean sameUV =
    uvFirst.IsEqual(uvLast, Precision::PConfusion());

  const TColgp_Array1OfPnt2d&    UVNodes = theTrigu->UVNodes();
  const TColgp_Array1OfPnt&      Nodes   = theTrigu->Nodes();
  const TColStd_Array1OfInteger& Indices = Poly->Nodes();

  const Standard_Integer nbnodes = Indices.Length();
  TColStd_Array1OfInteger NewNodes(1, nbnodes);
  
  gp_Pnt  P3d;
  gp_XY   anUV;

  // Process first vertex
  Standard_Integer ipf;
  if (myVertices.IsBound(pBegin))
  {
    ipf = myVertices.Find(pBegin);
  }
  else
  {
    if (sameUV && myVertices.IsBound(pEnd))
    {
      ipf = myVertices.Find(pEnd);
    }
    else
    {
      P3d = Nodes(Indices(1));
      if (!theLoc.IsIdentity())
        P3d.Transform(theLoc.Transformation());
      myNbLocat++;
      myLocation3d.Bind(myNbLocat, P3d);
      ipf = myNbLocat;
    }
    myVertices.Bind(pBegin,ipf);
  } 
    
 //Controle vertice tolerances
  gp_Pnt pFirst = theSurf->Value(uvFirst.X(), uvFirst.Y());
  gp_Pnt pLast  = theSurf->Value(uvLast.X(), uvLast.Y());
  
  Standard_Real mindist = 10. * Max(pFirst.Distance(BRep_Tool::Pnt(pBegin)), 
                                    pLast.Distance(BRep_Tool::Pnt(pEnd)));

  if (mindist < BRep_Tool::Tolerance(pBegin) ||
      mindist < BRep_Tool::Tolerance(pEnd) ) mindist = theDefEdge;
  
  anUV = FindUV(pBegin, uvFirst, ipf, theSurf, mindist, myAttrib, myLocation2d);
  Standard_Integer iv1, isv1;
  BRepMesh_Vertex vf(anUV, ipf, BRepMesh_Frontier);
  iv1 = myStructure->AddNode(vf);
  isv1 = myVemap.FindIndex(iv1);
  if (isv1 == 0)
    isv1 = myVemap.Add(iv1);
  NewNodes(1) = isv1;

  // Process last vertex
  Standard_Integer ipl, ivl;
  if (pEnd.IsSame(pBegin))
  {
    ipl = ipf;
  }
  else
  {
    if (myVertices.IsBound(pEnd))
    {
      ipl = myVertices.Find(pEnd);
    }
    else
    {
      if (sameUV)
      {
        ipl = ipf;
        ivl = iv1;
      }
      else
      {
        P3d = Nodes(Indices(nbnodes));
        if (!theLoc.IsIdentity())
          P3d.Transform(theLoc.Transformation());
        myNbLocat++;
        myLocation3d.Bind(myNbLocat, P3d);
        ipl = myNbLocat;
      }
      myVertices.Bind(pEnd,ipl);
    }
  }

  anUV = FindUV(pEnd, uvLast, ipl, theSurf, mindist, myAttrib, myLocation2d);
  BRepMesh_Vertex vl(anUV, ipl, BRepMesh_Frontier);
  
  Standard_Integer isvl;
  ivl = myStructure->AddNode(vl);
  isvl = myVemap.FindIndex(ivl);
  if (isvl == 0) 
    isvl = myVemap.Add(ivl);
  NewNodes(nbnodes) = isvl;
      
  BRepMesh_Vertex v;

  Standard_Integer i;
  for (i = 2; i < nbnodes; i++)
  {
    // Record 3d point
    P3d = Nodes(Indices(i));
    if (!theLoc.IsIdentity())
      P3d.Transform(theLoc.Transformation());
    myNbLocat++;
    myLocation3d.Bind(myNbLocat, P3d);
    
    // Record 2d point
    anUV = UVNodes(Indices(i)).Coord();

    Standard_Integer iv2, isv;
    v.Initialize(anUV, myNbLocat, BRepMesh_Frontier);
    iv2 = myStructure->AddNode(v);
    isv = myVemap.FindIndex(iv2);
    if (isv == 0)
      isv = myVemap.Add(iv2);
    NewNodes(i) = isv;
    
    //add links
    if (orEdge == TopAbs_FORWARD)
      myStructure->AddLink(BRepMesh_Edge(iv1,iv2,BRepMesh_Frontier));
    else if (orEdge == TopAbs_REVERSED)
      myStructure->AddLink(BRepMesh_Edge(iv2,iv1,BRepMesh_Frontier));
    else if (orEdge == TopAbs_INTERNAL)
      myStructure->AddLink(BRepMesh_Edge(iv1,iv2,BRepMesh_Fixed));
    iv1 = iv2;  
  }
  
  // last point
  if (iv1 != ivl) {
    if (orEdge == TopAbs_FORWARD)
      myStructure->AddLink(BRepMesh_Edge(iv1,ivl,BRepMesh_Frontier));
    else if (orEdge == TopAbs_REVERSED)
      myStructure->AddLink(BRepMesh_Edge(ivl,iv1,BRepMesh_Frontier));
    else if (orEdge == TopAbs_INTERNAL)
      myStructure->AddLink(BRepMesh_Edge(iv1,ivl,BRepMesh_Fixed));
  }
  
  Handle(Poly_PolygonOnTriangulation) P1 =
        new Poly_PolygonOnTriangulation(NewNodes, Poly->Parameters()->Array1());
  P1->Deflection(theDefEdge);
  if (myInternaledges.IsBound(theEdge))
  {
    BRepMesh_PairOfPolygon& pair = myInternaledges.ChangeFind(theEdge);
    if (theEdge.Orientation() == TopAbs_REVERSED)
      pair.Append(P1);
    else
      pair.Prepend(P1);
  }
  else
  {
    BRepMesh_PairOfPolygon pair1;
    pair1.Append(P1);
    myInternaledges.Bind(theEdge, pair1);
  }

  return Standard_True;
}

//=======================================================================
//function : InternalVertices
//purpose  : 
//=======================================================================

static void filterParameters(const TColStd_IndexedMapOfReal& theParams,
                             const Standard_Real theMinDist,
                             const Standard_Real theFilterDist,
                             TColStd_SequenceOfReal& theResult)
{
  // Sort sequence of parameters
  TColStd_SequenceOfReal aParamTmp;
  Standard_Integer aParamLength = 1;
  const Standard_Integer anInitLen = theParams.Extent();
    
  TColStd_Array1OfReal aParamArray(1, anInitLen);
  Standard_Integer j;
  for (j = 1; j <= anInitLen; j++)
    aParamArray(j) = theParams(j);

  TCollection_CompareOfReal aCompare;
  SortTools_ShellSortOfReal::Sort(aParamArray, aCompare);

  // mandadory pre-filtering using the first (minimal) filter value
  Standard_Real aP1, aP2;
  aP1 = aParamArray(1);
  aParamTmp.Append(aP1);
  for (j = 2; j <= anInitLen; j++) 
  {
    aP2 = aParamArray(j);
    if ((aP2-aP1) > theMinDist)
    {
      aParamTmp.Append(aP2);
      aP1 = aP2;
      aParamLength++;
    }
  }

  //add last point if required
  if(aParamArray(anInitLen)-theParams(aParamLength) > theMinDist)
  {
    aParamTmp.Append(aParamArray(anInitLen));        
    aParamLength++;
  }
  
  //perform filtering on series
  Standard_Real aLastAdded, aLastCandidate;
  Standard_Boolean isCandidateDefined = Standard_False;
  aLastAdded = aParamTmp.First();
  aLastCandidate = aLastAdded;
  theResult.Append(aParamTmp.First());
  
  for(j=2;j<aParamTmp.Length();j++) 
  {
    Standard_Real aVal = aParamTmp.Value(j);
    if(aVal-aLastAdded > theFilterDist) 
    {
      //adds the parameter
      if(isCandidateDefined) {
        aLastAdded = aLastCandidate;
        isCandidateDefined = Standard_False;
        j--;
      }
      else 
      {
        aLastAdded = aVal;
      }
      theResult.Append(aLastAdded);
      continue;
    }
    
    aLastCandidate = aVal;
    isCandidateDefined = Standard_True;
  }
  theResult.Append(aParamTmp.Last());
  
  if( theResult.Length() == 2 )
  {
    Standard_Real    dist  = theResult.Last() - theResult.First();
    Standard_Integer nbint = (Standard_Integer)((dist / theFilterDist) + 0.5);

    if( nbint > 1 )
    {
      //Five points more is maximum
      if( nbint > 5 )
      {
        nbint = 5;
      }

      Standard_Integer i;
      Standard_Real dU = dist / nbint;
      for( i = 1; i < nbint; i++ )
      {
        theResult.InsertAfter(i, theResult.First()+i*dU);
      }
    }
  }
}

void BRepMesh_FastDiscretFace::InternalVertices(const Handle(BRepAdaptor_HSurface)& theCaro,
                                                BRepMesh_ListOfVertex&              theInternalV,
                                                const Standard_Real                 theDefFace,
                                                const BRepMesh_ClassifierPtr&       theClassifier)
{
  BRepMesh_Vertex newV;
  gp_Pnt2d p2d;
  gp_Pnt p3d;
  
  // work following the type of surface
  const BRepAdaptor_Surface& BS = *(BRepAdaptor_Surface*)&(theCaro->Surface());
  GeomAbs_SurfaceType thetype = theCaro->GetType();

  Standard_Real umax = myAttrib->GetUMax();
  Standard_Real umin = myAttrib->GetUMin();
  Standard_Real vmax = myAttrib->GetVMax();
  Standard_Real vmin = myAttrib->GetVMin();
  Standard_Real deltaX = myAttrib->GetDeltaX();
  Standard_Real deltaY = myAttrib->GetDeltaY();

  if (thetype == GeomAbs_Sphere)
  { 
    gp_Sphere S = BS.Sphere(); 
    const Standard_Real R = S.Radius();

    // Calculate parameters for iteration in V direction
    Standard_Real Dv = 1.0 - (theDefFace/R);
    if (Dv < 0.0) Dv = 0.0;
    Standard_Real oldDv = 2.0 * ACos (Dv);
    Dv  =  .7 * oldDv; //.7 ~= sqrt(2.) - Dv is hypotenuse of triangle when oldDv is legs
    const Standard_Real sv = vmax - vmin;
    Dv = sv/((Standard_Integer)(sv/Dv) + 1);
    const Standard_Real pasvmax = vmax-Dv*0.5;

    //Du can be defined from relation: 2*r*Sin(Du/2) = 2*R*Sin(Dv/2), r = R*Cos(v)
    //here approximate relation r*Du = R*Dv is used
    
    Standard_Real Du, pasu, pasv; //, ru;
    const Standard_Real su = umax-umin;
    Standard_Boolean Shift = Standard_False;
    for (pasv = vmin + Dv; pasv < pasvmax; pasv += Dv)
    {
      // Calculate parameters for iteration in U direction
      // 1.-.365*pasv*pasv is simple approximation of Cos(pasv)
      // with condition that it gives ~.1 when pasv = pi/2
      Du = Dv/(1.-.365*pasv*pasv);
      Du = su/((Standard_Integer)(su/Du) + 1);
      Shift = !Shift;
      const Standard_Real d = (Shift)? Du*.5 : 0.;
      const Standard_Real pasumax = umax-Du*0.5 + d;
      for (pasu = umin + Du - d; pasu < pasumax; pasu += Du)
      {
        if (theClassifier->Perform(gp_Pnt2d(pasu, pasv)) == TopAbs_IN)
        {
          // Record 3d point
#ifdef DEB_MESH_CHRONO
          D0Internal++;
#endif
          ElSLib::D0(pasu, pasv, S, p3d);
          myNbLocat++;
          myLocation3d.Bind(myNbLocat, p3d);
          // Record 2d point
          p2d.SetCoord((pasu-umin)/deltaX, (pasv-vmin)/deltaY);
          newV.Initialize(p2d.XY(), myNbLocat, BRepMesh_Free);
          theInternalV.Append(newV);
        }
      }
    }
  }
  else if (thetype == GeomAbs_Cylinder)
  {
    gp_Cylinder S = BS.Cylinder();
    const Standard_Real R = S.Radius();

    // Calculate parameters for iteration in U direction
    Standard_Real Du = 1.0 - (theDefFace/R);
    if (Du < 0.0) Du = 0.0;
    Du = 2.0 * ACos (Du);
    if (Du > myAngle) Du = myAngle;
    const Standard_Real su = umax - umin;
    const Standard_Integer nbU = (Standard_Integer)(su/Du);
    Du = su/(nbU+1);

    // Calculate parameters for iteration in V direction
    const Standard_Real sv = vmax - vmin;
    Standard_Integer nbV = (Standard_Integer)( nbU*sv/(su*R) );
    nbV = Min(nbV, 100*nbU);
    Standard_Real Dv = sv/(nbV+1);

    Standard_Real pasu, pasv, pasvmax = vmax-Dv*0.5, pasumax = umax-Du*0.5;
    for (pasv = vmin + Dv; pasv < pasvmax; pasv += Dv) {
      for (pasu = umin + Du; pasu < pasumax; pasu += Du) {
        if (theClassifier->Perform(gp_Pnt2d(pasu, pasv)) == TopAbs_IN)
        {
          // Record 3d point
          ElSLib::D0(pasu, pasv, S, p3d);
          myNbLocat++;
          myLocation3d.Bind(myNbLocat, p3d);
          // Record 2d point
          p2d.SetCoord((pasu-umin)/deltaX, (pasv-vmin)/deltaY);
          newV.Initialize(p2d.XY(), myNbLocat, BRepMesh_Free);
          theInternalV.Append(newV);
        }
      }
    }
  }
  else if (thetype == GeomAbs_Cone)
  {
    Standard_Real R, RefR, SAng;
    gp_Cone C = BS.Cone();
    RefR = C.RefRadius();
    SAng = C.SemiAngle();
    R = Max(Abs(RefR+vmin*Sin(SAng)), Abs(RefR+vmax*Sin(SAng)));
    Standard_Real Du, Dv, pasu, pasv;
    Du = Max(1.0e0 - (theDefFace/R),0.0e0);
    Du  = (2.0 * ACos (Du));
    Standard_Integer nbU = (Standard_Integer) ( (umax-umin)/Du );
    Standard_Integer nbV = (Standard_Integer) ( nbU*(vmax-vmin)/((umax-umin)*R) );
    Du = (umax-umin)/(nbU+1);
    Dv = (vmax-vmin)/(nbV+1);

    Standard_Real pasvmax = vmax-Dv*0.5, pasumax = umax-Du*0.5;
    for (pasv = vmin + Dv; pasv < pasvmax; pasv += Dv) {
      for (pasu = umin + Du; pasu < pasumax; pasu += Du) {
        if (theClassifier->Perform(gp_Pnt2d(pasu, pasv)) == TopAbs_IN)
        {
          // Record 3d point
          ElSLib::D0(pasu, pasv, C, p3d);
          myNbLocat++;
          myLocation3d.Bind(myNbLocat, p3d);
          // Record 2d point
          p2d.SetCoord((pasu-umin)/deltaX, (pasv-vmin)/deltaY);
          newV.Initialize(p2d.XY(), myNbLocat, BRepMesh_Free);
          theInternalV.Append(newV);
        }
      }
    }
  }
  else if (thetype == GeomAbs_Torus)
  {
    gp_Torus T = BS.Torus();

    Standard_Boolean insert;
    Standard_Integer i, j, ParamULength, ParamVLength;
    Standard_Real pp, pasu, pasv;
    Standard_Real r = T.MinorRadius(), R = T.MajorRadius();

    TColStd_SequenceOfReal ParamU, ParamV;

    Standard_Real Du, Dv;//, pasu, pasv;
    Dv = Max(1.0e0 - (theDefFace/r),0.0e0) ;
    Standard_Real oldDv = 2.0 * ACos (Dv);
    oldDv = Min(oldDv, myAngle);
    Dv  =  0.9*oldDv; //TWOTHIRD * oldDv;
    Dv = oldDv;
    
    Standard_Integer nbV = Max((Standard_Integer)((vmax-vmin)/Dv), 2);
    Dv = (vmax-vmin)/(nbV+1);
    Standard_Real ru = R + r;
    if (ru > 1.e-16)
    {
      Du  = 2.0 * ACos(Max(1.0 - (theDefFace/ru),0.0));
      if (myAngle < Du) Du = myAngle;
      Standard_Real aa = sqrt(Du*Du + oldDv*oldDv);
      if(aa < gp::Resolution())
        return; 
      Du *= Min(oldDv, Du) / aa;
    }
    else Du = Dv;     
    
    Standard_Integer nbU = Max((Standard_Integer)((umax-umin)/Du), 2);
    nbU = Max(nbU , (int)(nbV*(umax-umin)*R/((vmax-vmin)*r)/5.));
    Du = (umax-umin)/(nbU+1);
    
    if (R < r)
    {
      // As the points of edges are returned.
      // in this case, the points are not representative.
            
      //-- Choose DeltaX and DeltaY so that to avoid skipping points on the grid
           for (i = 0; i <= nbU; i++) ParamU.Append(umin + i* Du);
    }//R<r
    else //U if R > r
    {
      //--ofv: U
      // Number of mapped U parameters
      const Standard_Integer LenU = myUParam.Extent();
      // Fill array of U parameters
      TColStd_Array1OfReal Up(1,LenU);
      for (j = 1; j <= LenU; j++) Up(j) = myUParam(j);
      
      // Calculate DU, leave array of parameters
      Standard_Real aDU = FUN_CalcAverageDUV(Up,LenU);
      aDU = Max(aDU, Abs(umax -  umin) / (Standard_Real) nbU / 2.);
      Standard_Real dUstd = Abs(umax -  umin) / (Standard_Real) LenU;
      if (aDU > dUstd) dUstd = aDU;
      // Add U parameters
      for (j = 1; j <= LenU; j++)
      {
        pp = Up(j);
        insert = Standard_True;
        ParamULength = ParamU.Length();
        for (i = 1; i <= ParamULength && insert; i++)
        {
          insert = (Abs(ParamU.Value(i)-pp) > (0.5*dUstd));
        }
        if (insert) ParamU.Append(pp);
      }
    } 

    //--ofv: V
    // Number of mapped V parameters
    const Standard_Integer LenV = myVParam.Extent();
    // Fill array of V parameters
    TColStd_Array1OfReal Vp(1,LenV);
    for (j = 1; j <= LenV; j++) Vp(j) = myVParam(j);
    // Calculate DV, sort array of parameters
    Standard_Real aDV = FUN_CalcAverageDUV(Vp,LenV);
    aDV = Max(aDV, Abs(vmax -  vmin) / (Standard_Real) nbV / 2.);

    Standard_Real dVstd = Abs(vmax -  vmin) / (Standard_Real) LenV;
    if (aDV > dVstd) dVstd = aDV;
    // Add V parameters
    for (j = 1; j <= LenV; j++)
    {
      pp = Vp(j);

      insert = Standard_True;
      ParamVLength = ParamV.Length();
      for (i = 1; i <= ParamVLength && insert; i++)
      {
        insert = (Abs(ParamV.Value(i)-pp) > (dVstd*2./3.));
      }
      if (insert) ParamV.Append(pp);
    }

    Standard_Integer Lu = ParamU.Length(), Lv = ParamV.Length();
    Standard_Real uminnew = umin+deltaY*0.1;
    Standard_Real vminnew = vmin+deltaX*0.1;
    Standard_Real umaxnew = umax-deltaY*0.1;
    Standard_Real vmaxnew = vmax-deltaX*0.1;

    for (i = 1; i <= Lu; i++)
    {
      pasu = ParamU.Value(i);
      if (pasu >= uminnew && pasu < umaxnew)
      {
        for (j = 1; j <= Lv; j++)
        {
          pasv = ParamV.Value(j);
          if (pasv >= vminnew && pasv < vmaxnew)
          {
            if (theClassifier->Perform(gp_Pnt2d(pasu, pasv)) == TopAbs_IN)
            {
              // Record 3d point
              ElSLib::D0(pasu, pasv, T, p3d);
              myNbLocat++;
              myLocation3d.Bind(myNbLocat, p3d);
              // Record 2d point
              p2d.SetCoord((pasu-umin)/deltaX, (pasv-vmin)/deltaY);
              newV.Initialize(p2d.XY(), myNbLocat, BRepMesh_Free);
              theInternalV.Append(newV);
            }
          }
        }
      }
    }
  }
  else if (thetype == GeomAbs_BezierSurface || thetype == GeomAbs_BSplineSurface)
  {
    //define resolutions
    Standard_Real uRes = BS.UResolution(theDefFace);
    Standard_Real vRes = BS.VResolution(theDefFace);

    // Sort and filter sequence of parameters
    Standard_Real aMinDu = Precision::PConfusion();
    if(deltaX < 1.)
      aMinDu/=deltaX;

    Standard_Real aDuMaxLim = 0.1*(umax-umin);
    Standard_Real ddu = Min(aDuMaxLim,Max(0.005*(umax-umin),2.*uRes));
    TColStd_SequenceOfReal ParamU; 
    filterParameters(myUParam,aMinDu,ddu,ParamU);
    Standard_Integer ParamULength = ParamU.Length();
   
    Standard_Real aMinDv = Precision::PConfusion();
    if(deltaY < 1)
      aMinDv/=deltaY;

    Standard_Real aDvMaxLim = 0.1*(vmax-vmin);
    Standard_Real ddv = Min(aDvMaxLim,Max(0.005*(vmax-vmin),2.*vRes));
    TColStd_SequenceOfReal ParamV; 
    filterParameters(myVParam,aMinDv,ddv,ParamV);
    Standard_Integer ParamVLength = ParamV.Length();

    // check intermediate isolines
    Handle(Geom_Surface) B;
    if (thetype == GeomAbs_BezierSurface) {
      B = BS.Bezier();
    }
    else {
      B = BS.BSpline();
    }

    gp_Pnt P1, P2, PControl;
    Standard_Real u, v, dist;

    // precision for compare square distances
    double dPreci = Precision::SquareConfusion();

    // Insert V parameters by deflection criterion
    Standard_Integer i,j;
    Standard_Real V1, V2, U1, U2;
    for (i = 1; i <= ParamULength; i++) {
      Handle(Geom_Curve) IsoU = B->UIso(ParamU.Value(i));
      V1 = ParamV.Value(1);
      P1 = IsoU->Value(V1);
      for (j = 2; j <= ParamVLength;) {
        V2 = ParamV.Value(j);
        P2 = IsoU->Value(V2);
        v = 0.5*(V1+V2);
        PControl = IsoU->Value(v);
        // 23.03.2010 skl for OCC21645 - change precision for comparison
        if( P1.SquareDistance(P2) > dPreci ) {
          gp_Lin L (P1, gp_Dir(gp_Vec(P1, P2)));
          dist = L.Distance(PControl);
        }
        else {
          dist = P1.Distance(PControl);
        }
        if (dist > theDefFace) {
          // insertion 
          ParamV.InsertBefore(j, v);
          ParamVLength++;
        }
        else {
          //put regular grig for normals
          gp_Dir N1(0,0,1),N2(0,0,1);
          Standard_Boolean aSt1 = GeomLib::NormEstim(B, gp_Pnt2d(ParamU.Value(i),V1), Precision::Confusion(), N1);
          Standard_Boolean aSt2 = GeomLib::NormEstim(B, gp_Pnt2d(ParamU.Value(i),v), Precision::Confusion(), N2);

          Standard_Real anAngle1 = N2.Angle(N1);
	  if(aSt1 < 1 && aSt2 < 1 && anAngle1 > myAngle ) {
            // insertion 
            ParamV.InsertBefore(j, v);
            ParamVLength++;
          }
          else {
            V1 = V2;
            P1 = P2;
            j++;
          }
        }
      }
    }

    for (i = 2; i < ParamVLength; i++) {
      v = ParamV.Value(i);
      Handle(Geom_Curve) IsoV = B->VIso(v);
      U1 = ParamU.Value(1);
      P1 = IsoV->Value(U1);
      for (j = 2; j <= ParamULength;) {
        U2 = ParamU.Value(j);
        P2 = IsoV->Value(U2);
        u = 0.5*(U1+U2);
        PControl = IsoV->Value(u);
        // 23.03.2010 skl for OCC21645 - change precision for comparison
        if( P1.SquareDistance(P2) > dPreci ) {
          gp_Lin L (P1, gp_Dir(gp_Vec(P1, P2)));
          dist = L.Distance(PControl);
        }
        else {
          dist = P1.Distance(PControl);
        }
        if (dist > theDefFace) {
          // insertion 
          ParamU.InsertBefore(j, u);
          ParamULength++;
        }
        else {
          //check normal
          //put regular grig for normals
          gp_Dir N1(0,0,1),N2(0,0,1);
          Standard_Boolean aSt1 = GeomLib::NormEstim(B, gp_Pnt2d(U1,v), Precision::Confusion(), N1);
          Standard_Boolean aSt2 = GeomLib::NormEstim(B, gp_Pnt2d(u,v), Precision::Confusion(), N2);
          
          Standard_Real anAngle1 = N2.Angle(N1);
	        if(aSt1 < 1 && aSt2 < 1 && anAngle1 > myAngle) {
            // insertion 
            ParamU.InsertBefore(j, u);
            ParamULength++;
          }
          else {
            U1 = U2;
            P1 = P2;
            if (j < ParamULength) {
              // Classify intersection point
              if (theClassifier->Perform(gp_Pnt2d(U1, v)) == TopAbs_IN)
              {
                // Record 3d point
                myNbLocat++;
                myLocation3d.Bind(myNbLocat, P1);
                // Record 2d point
                p2d.SetCoord((U1-umin)/deltaX, (v-vmin)/deltaY);
                newV.Initialize(p2d.XY(), myNbLocat, BRepMesh_Free);
                theInternalV.Append(newV);
              }
            }
            j++;
          }
        }
      }
    }
  }
  else {
    const Standard_Real anAngle = 0.35;

    Standard_Integer i, j, nbpointsU = 10, nbpointsV = 10;
    Adaptor3d_IsoCurve tabu[11], tabv[11];

    TColStd_SequenceOfReal ParamU, ParamV;
    Standard_Real u, v, du, dv;
    Standard_Integer iu, iv;
    Standard_Real f, l;

    du = (umax-umin) / (nbpointsU+1); dv = (vmax-vmin) / (nbpointsV+1);

    for (iu = 0; iu <= nbpointsU; iu++) {
      u = umin + iu*du;
      tabu[iu].Load(theCaro);
      tabu[iu].Load(GeomAbs_IsoU, u);
    }

    for (iv = 0; iv <= nbpointsV; iv++) {
      v = vmin + iv*dv;
      tabv[iv].Load(theCaro);
      tabv[iv].Load(GeomAbs_IsoV, v);
    }

    Standard_Integer imax = 1, MaxV = 0;
    
    GCPnts_TangentialDeflection* tabGU = new GCPnts_TangentialDeflection[nbpointsU+1];

    for (i = 0; i <= nbpointsU; i++) {
      f = Max(vmin, tabu[i].FirstParameter());
      l = Min(vmax, tabu[i].LastParameter());
      GCPnts_TangentialDeflection theDeflection(tabu[i], f, l, anAngle, 0.7*theDefFace, 2);
      tabGU[i] = theDeflection;
      if (tabGU[i].NbPoints() > MaxV) {
        MaxV = tabGU[i].NbPoints();
        imax = i;
      }
    }
    
    // return table of parameters V:
    Standard_Integer NV = tabGU[imax].NbPoints();
    for (i = 1; i <= NV; i++) {
      ParamV.Append(tabGU[imax].Parameter(i));
    }
    delete [] tabGU;

    imax = 1;
    Standard_Integer MaxU = 0;

    GCPnts_TangentialDeflection* tabGV = new GCPnts_TangentialDeflection[nbpointsV+1];

    for (i = 0; i <= nbpointsV; i++) {
      f = Max(umin, tabv[i].FirstParameter());
      l = Min(umax, tabv[i].LastParameter());
      GCPnts_TangentialDeflection thedeflection2(tabv[i], f, l, anAngle, 0.7*theDefFace, 2);
      tabGV[i] = thedeflection2;
      if (tabGV[i].NbPoints() > MaxU) {
        MaxU = tabGV[i].NbPoints();
        imax = i;
      }
    }
    
    // return table of parameters U:
    Standard_Integer NU = tabGV[imax].NbPoints();
    for (i = 1; i <= NU; i++) {
      ParamU.Append(tabGV[imax].Parameter(i));
    }
    delete [] tabGV;
    
    if (ParamU.Length() == 2) {
      ParamU.InsertAfter(1, (umax+umin)*0.5);
    }
    if (ParamV.Length() == 2) {
      ParamV.InsertAfter(1, (vmax+vmin)*0.5);
    }
    
    TColStd_SequenceOfReal InsertV, InsertU;
    gp_Pnt P1;

    Adaptor3d_IsoCurve IsoV;
    IsoV.Load(theCaro);

    Standard_Integer Lu = ParamU.Length(), Lv = ParamV.Length();
    
    for (i = 2; i < Lv; i++) {
      v = ParamV.Value(i);
      IsoV.Load(GeomAbs_IsoV, v);
      for (j = 2; j < Lu; j++) {
        u = ParamU.Value(j);
        if (theClassifier->Perform(gp_Pnt2d(u, v)) == TopAbs_IN)
        {
          // Record 3d point
          P1 = IsoV.Value(u);
          myNbLocat++;
          myLocation3d.Bind(myNbLocat, P1);
          // Record 2d point
          p2d.SetCoord((u-umin)/deltaX, (v-vmin)/deltaY);
          newV.Initialize(p2d.XY(), myNbLocat, BRepMesh_Free);
          theInternalV.Append(newV); 
        }
      }
    } 
  }
#ifdef DEB_MESH_CHRONO
  chInternal.Stop();
#endif
}

/**
*  Internal class Couple, moved from MeshData package
*/

class BRepMesh_Couple
{
 public:
  BRepMesh_Couple() { myI1 = myI2 = 0; }
  BRepMesh_Couple(const Standard_Integer I1,
                  const Standard_Integer I2)
  { myI1 = I1; myI2 = I2; }

  Standard_Integer myI1;
  Standard_Integer myI2;
};

inline Standard_Boolean IsEqual(const BRepMesh_Couple& one,
                                const BRepMesh_Couple& other)
{
  if (one.myI1 == other.myI1 &&
      one.myI2 == other.myI2) return Standard_True;
  else return Standard_False;
}

inline Standard_Integer HashCode(const BRepMesh_Couple& one,
                                 const Standard_Integer Upper)
{
  return ::HashCode((one.myI1+one.myI2), Upper);
}

typedef NCollection_Map<BRepMesh_Couple> BRepMesh_MapOfCouple;

//=======================================================================
//function : Control
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_FastDiscretFace::Control(const Handle(BRepAdaptor_HSurface)& theCaro,
                                                const Standard_Real                 theDefFace,
                                                BRepMesh_ListOfVertex&              theInternalV,
                                                TColStd_ListOfInteger&              theBadTriangles,
                                                TColStd_ListOfInteger&              theNulTriangles,
                                                BRepMesh_Delaun&                    theTrigu,
                                                const Standard_Boolean              theIsFirst)
{
  //IMPORTANT: Constants used in calculations
  const Standard_Real MinimalArea2d = 1.e-9;
  const Standard_Real MinimalSqLength3d = 1.e-12;

  // Define the number of iterations
  Standard_Integer myNbIterations = 11;
  const Standard_Integer nbPasses = (theIsFirst? 1 : myNbIterations);

  // Initialize stop condition
  Standard_Boolean allDegenerated = Standard_False;
  Standard_Integer nbInserted = 1;

  // Create map of links to skip already processed
  Standard_Integer nbtriangles;

  nbtriangles = myStructure->ElemOfDomain().Extent();
  if (nbtriangles <= 0) return -1.0;
  BRepMesh_MapOfCouple theCouples(3*nbtriangles);

  gp_XY mi2d;
  gp_XYZ vecEd1, vecEd2, vecEd3;
  gp_Pnt pDef;
  Standard_Real dv = 0, defl = 0, maxdef = -1;
  Standard_Integer pass = 1, nf = 0, nl = 0;
  BRepMesh_Vertex InsVertex;
  Standard_Boolean caninsert;

  Standard_Real sqdefface = theDefFace * theDefFace;

  GeomAbs_SurfaceType thetype = theCaro->GetType();
  Handle(Geom_Surface) BSpl;
  Standard_Boolean isSpline = Standard_False;
  if (thetype == GeomAbs_BezierSurface || thetype == GeomAbs_BSplineSurface)
  {
    isSpline = Standard_True;
    if (thetype == GeomAbs_BezierSurface) 
      BSpl = theCaro->Bezier();
    else 
      BSpl = theCaro->BSpline();
  }

  NCollection_DataMap<Standard_Integer,gp_Dir> aNorMap;
  NCollection_DataMap<Standard_Integer,Standard_Integer> aStatMap;

  // Perform refinement passes
  for (; pass <= nbPasses && nbInserted && !allDegenerated; pass++)
  {
    theInternalV.Clear();
    theBadTriangles.Clear();
    
    // Reset stop condition
    allDegenerated = Standard_True;
    nbInserted = 0;
    maxdef = -1.0;

    // Do not insert nodes in last pass in non-SharedMode
    caninsert = (myWithShare || pass < nbPasses);

    // Read mesh size
    nbtriangles = myStructure->ElemOfDomain().Extent();
    if (nbtriangles <= 0) break;

    // Iterate on current triangles
    BRepMesh_MapOfInteger::Iterator triDom;
    const BRepMesh_MapOfInteger& TriMap = myStructure->ElemOfDomain();
    triDom.Initialize(TriMap);
    Standard_Integer aNbPnt = 0;
    Standard_Real umin = myAttrib->GetUMin();
    Standard_Real vmin = myAttrib->GetVMin();
    Standard_Real deltaX = myAttrib->GetDeltaX();
    Standard_Real deltaY = myAttrib->GetDeltaY();
    for (; triDom.More(); triDom.Next())
    {
      Standard_Integer TriId = triDom.Key();
      const BRepMesh_Triangle& curTri=Triangle(TriId);
      if (curTri.Movability()==BRepMesh_Deleted) continue;
      
      Standard_Boolean o1, o2, o3;
      Standard_Integer v1 = 0, v2 = 0, v3 = 0, e1 = 0, e2 = 0, e3 = 0;
      curTri.Edges(e1, e2, e3, o1, o2, o3);
      
      const BRepMesh_Edge& edg1=Edge(e1);
      const BRepMesh_Edge& edg2=Edge(e2);
      const BRepMesh_Edge& edg3=Edge(e3);
      
      Standard_Boolean m1 = (edg1.Movability() == BRepMesh_Frontier);
      Standard_Boolean m2 = (edg2.Movability() == BRepMesh_Frontier);
      Standard_Boolean m3 = (edg3.Movability() == BRepMesh_Frontier);
      if (o1) {
        v1=edg1.FirstNode();
        v2=edg1.LastNode();
      }
      else {
        v1=edg1.LastNode();
        v2=edg1.FirstNode();
      }
      if (o2)
        v3=edg2.LastNode();
      else
        v3=edg2.FirstNode();

      const BRepMesh_Vertex& vert1=Vertex(v1);
      const BRepMesh_Vertex& vert2=Vertex(v2);
      const BRepMesh_Vertex& vert3=Vertex(v3);

      const gp_XYZ& p1=myLocation3d(vert1.Location3d()).Coord();
      const gp_XYZ& p2=myLocation3d(vert2.Location3d()).Coord();
      const gp_XYZ& p3=myLocation3d(vert3.Location3d()).Coord();

      vecEd1 = p2 - p1;
      vecEd2 = p3 - p2;
      vecEd3 = p1 - p3;

      // Check for degenerated triangle
      if (vecEd1.SquareModulus() < MinimalSqLength3d ||
          vecEd2.SquareModulus() < MinimalSqLength3d ||
          vecEd3.SquareModulus() < MinimalSqLength3d) 
      {
        theNulTriangles.Append(TriId);
        continue;
      }

      allDegenerated = Standard_False;

      gp_XY xy1(vert1.Coord().X()*deltaX+umin,vert1.Coord().Y()*deltaY+vmin);
      gp_XY xy2(vert2.Coord().X()*deltaX+umin,vert2.Coord().Y()*deltaY+vmin);
      gp_XY xy3(vert3.Coord().X()*deltaX+umin,vert3.Coord().Y()*deltaY+vmin);

      // Check triangle area in 2d
      if (Abs((xy2-xy1)^(xy3-xy1)) < MinimalArea2d)
      {
        theNulTriangles.Append(TriId);
        continue;
      }

      // Check triangle normal
      gp_XYZ normal(vecEd1^vecEd2);
      dv = normal.Modulus();
      if (dv < Precision::Confusion())
      {
        theNulTriangles.Append(TriId);
        continue;
      }
      normal /= dv;

      // Check deflection on triangle
      mi2d = (xy1+xy2+xy3)/3.0;
      theCaro->D0(mi2d.X(), mi2d.Y(), pDef);
      defl = Abs(normal*(pDef.XYZ()-p1));
      defl = defl*defl;     
      /*mi2d = (xy1+xy2+xy3)/3.0;
      theCaro->D0(mi2d.X(), mi2d.Y(), pDef);
      defl = pDef.SquareDistance((p1+p2+p3)/3.);*/
      if (defl > maxdef) maxdef = defl;
      if (defl > sqdefface)
      {
        if (theIsFirst) break;
        if (caninsert)
        {
          // Record new vertex
          aNbPnt++;
          myNbLocat++;
          myLocation3d.Bind(myNbLocat,pDef);
          mi2d.SetCoord((mi2d.X()-umin)/deltaX,(mi2d.Y()-vmin)/deltaY);
          InsVertex.Initialize(mi2d,myNbLocat,BRepMesh_Free);
          theInternalV.Append(InsVertex);
        }
        theBadTriangles.Append(TriId);
      }
      
      if (!m2) // Not a boundary
      {
        // Check if this link was already processed
        if (v2 < v3) { nf = v2; nl = v3; } else { nf = v3; nl = v2; }
        if (theCouples.Add(BRepMesh_Couple(nf,nl)))
        {
          // Check deflection on edge 1
          mi2d = (xy2+xy3)*0.5;
          theCaro->D0(mi2d.X(), mi2d.Y(), pDef);
          gp_Lin L (p2, gp_Vec(p2, p3));
          defl = L.SquareDistance(pDef);
          if (defl > maxdef) maxdef = defl;
          if (defl > sqdefface)
          {
            if (theIsFirst) break;
            if (caninsert)
            {
              // Record new vertex
              aNbPnt++;              
              myNbLocat++;
              myLocation3d.Bind(myNbLocat,pDef);
              mi2d.SetCoord((mi2d.X()-umin)/deltaX,(mi2d.Y()-vmin)/deltaY);
              InsVertex.Initialize(mi2d,myNbLocat,BRepMesh_Free);
              theInternalV.Append(InsVertex);
            }
            theBadTriangles.Append(TriId);
          }
        }
      }

      if (!m3) // Not a boundary
      {
        // Check if this link was already processed
        if (v1 < v3) { nf = v1; nl = v3; } else { nf = v3; nl = v1; }
        if (theCouples.Add(BRepMesh_Couple(nf,nl)))
        {
          // Check deflection on edge 2
          mi2d = (xy3+xy1)*0.5;
          theCaro->D0(mi2d.X(), mi2d.Y(), pDef);
          gp_Lin L (p1, gp_Vec(p1, p3));
          defl = L.SquareDistance(pDef);
          if (defl > maxdef) maxdef = defl;
          if (defl > sqdefface)
          {
            if (theIsFirst) break;
            if (caninsert)
            {
              // Record new vertex
              aNbPnt++;
              myNbLocat++;
              myLocation3d.Bind(myNbLocat,pDef);
              mi2d.SetCoord((mi2d.X()-umin)/deltaX,(mi2d.Y()-vmin)/deltaY);
              InsVertex.Initialize(mi2d,myNbLocat,BRepMesh_Free);
              theInternalV.Append(InsVertex);
            }
            theBadTriangles.Append(TriId);
          }
        }
      }

      if (!m1) // Not a boundary
      {
        // Check if this link was already processed
        if (v1 < v2) { nf = v1; nl = v2; } else { nf = v2; nl = v1; }
        if (theCouples.Add(BRepMesh_Couple(nf,nl)))
        {
          // Check deflection on edge 3
          mi2d = (xy1+xy2)*0.5;
          theCaro->D0(mi2d.X(), mi2d.Y(), pDef);
          gp_Lin L (p1, gp_Vec(p1, p2));
          defl = L.SquareDistance(pDef);
          if (defl > maxdef) maxdef = defl;
          if (defl > sqdefface)
          {
            if (theIsFirst) break;
            if (caninsert)
            {
              // Record new vertex
              aNbPnt++;
              myNbLocat++;
              myLocation3d.Bind(myNbLocat,pDef);
              mi2d.SetCoord((mi2d.X()-umin)/deltaX,(mi2d.Y()-vmin)/deltaY);              
              InsVertex.Initialize(mi2d,myNbLocat,BRepMesh_Free);
              theInternalV.Append(InsVertex);
            }
            theBadTriangles.Append(TriId);
          }
        }
      }
      
      //check normal on bsplines
      if(theIsFirst && isSpline && !BSpl.IsNull() )      
      {
        gp_Dir N1(0,0,1), N2(0,0,1), N3(0,0,1);
        Standard_Integer aSt1, aSt2, aSt3;
        if(aNorMap.IsBound(v1)) {
          aSt1 = aStatMap.Find(v1);
          N1 =aNorMap.Find(v1);
        }
        else {
          aSt1 = GeomLib::NormEstim(BSpl, gp_Pnt2d(xy1), Precision::Confusion(), N1);
          aStatMap.Bind(v1,aSt1);
          aNorMap.Bind(v1,N1);
        }

        if(aNorMap.IsBound(v2)) {
          aSt2 = aStatMap.Find(v2);
          N2 = aNorMap.Find(v2);
        }
        else {
          aSt2 = GeomLib::NormEstim(BSpl, gp_Pnt2d(xy2), Precision::Confusion(), N2);
          aStatMap.Bind(v2,aSt2);
          aNorMap.Bind(v2,N2);
        }

        if(aNorMap.IsBound(v3)) {
          aSt3 = aStatMap.Find(v3);
          N3 = aNorMap.Find(v3);
        }
        else {
          aSt3 = GeomLib::NormEstim(BSpl, gp_Pnt2d(xy3), Precision::Confusion(), N3);
          aStatMap.Bind(v3,aSt3);
          aNorMap.Bind(v3,N3.XYZ());
        }
        
        Standard_Real anAngle1 = N2.Angle(N1);
        Standard_Real anAngle2 = N3.Angle(N2);
        Standard_Real anAngle3 = N1.Angle(N3);
        if(aSt1 < 1 && aSt2 < 1 && aSt3 < 1 && 
            (anAngle1 > myAngle || anAngle2 > myAngle || anAngle3 > myAngle)) {

            maxdef = -1;
            break;
        }
      }
    }
    
    if (!theIsFirst && theInternalV.Extent() > 0) 
    {
      BRepMesh_Array1OfVertexOfDelaun verttab(1, theInternalV.Extent());
      BRepMesh_ListIteratorOfListOfVertex itVer(theInternalV);
      Standard_Integer ipn = 1;
      for (; itVer.More(); itVer.Next())
        verttab(ipn++) = itVer.Value();
        
      theTrigu.AddVertices(verttab);
      nbInserted++;
    }
  }

   if (maxdef < 0)
    return maxdef;
  return Sqrt(maxdef);
}

//=======================================================================
//function : AddInShape
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::AddInShape(const TopoDS_Face&  theFace,
                                          const Standard_Real theDefFace,
                                          const TopTools_MutexForShapeProvider& theMutexProvider)
{
//  gp_Pnt Pt;
  BRep_Builder B;
  TopLoc_Location loc = theFace.Location();
  Handle(Poly_Triangulation) TOld = BRep_Tool::Triangulation(theFace, loc);
  Handle(Poly_Triangulation) TNull;
  Handle(Poly_PolygonOnTriangulation) NullPoly;
  B.UpdateFace(theFace,TNull);

  try{
  BRepMesh_MapOfInteger::Iterator it;

  Standard_Integer e1, e2, e3, nTri;
  Standard_Integer v1, v2, v3, iv1, iv2, iv3;
  Standard_Integer i, index;
  Standard_Boolean o1, o2, o3;
  TopAbs_Orientation orFace = theFace.Orientation();

  const BRepMesh_MapOfInteger& TriMap = myStructure->ElemOfDomain();
  it.Initialize(TriMap);
    
  nTri = TriMap.Extent();

  if (nTri != 0) {
    
    Poly_Array1OfTriangle Tri(1, nTri);
    
    i = 1;
    
    for (; it.More(); it.Next()) {
      myStructure->GetElement(it.Key()).Edges(e1, e2, e3, o1, o2, o3);
      
      const BRepMesh_Edge& ve1=myStructure->GetLink(e1);
      const BRepMesh_Edge& ve2=myStructure->GetLink(e2);
      
      if (o1) {
        v1=ve1.FirstNode();
      }
      else {
        v1=ve1.LastNode();
      }
      if (o2)
      {
        v2=ve2.FirstNode();
        v3=ve2.LastNode();
      }
      else
      {
        v3=ve2.FirstNode();
        v2=ve2.LastNode();
      }
      
      iv1 = myVemap.FindIndex(v1);
      if (iv1 == 0) iv1 = myVemap.Add(v1);
      iv2 = myVemap.FindIndex(v2);
      if (iv2 == 0) iv2 = myVemap.Add(v2);
      iv3 = myVemap.FindIndex(v3);
      if (iv3 == 0) iv3 = myVemap.Add(v3);
      
      if (orFace == TopAbs_REVERSED) Tri(i++).Set(iv1, iv3, iv2);
      else Tri(i++).Set(iv1, iv2, iv3);
    }
    
    Standard_Integer nbVertices = myVemap.Extent();
    Handle(Poly_Triangulation) T = new Poly_Triangulation(nbVertices, nTri, Standard_True);
    Poly_Array1OfTriangle& Trian = T->ChangeTriangles();
    Trian = Tri;
    TColgp_Array1OfPnt&  Nodes = T->ChangeNodes();
    TColgp_Array1OfPnt2d& Nodes2d = T->ChangeUVNodes();
    
    for (i = 1; i <= nbVertices; i++) {
      index = myVemap.FindKey(i);
      Nodes(i) = Pnt(index);
      Nodes2d(i).SetXY(Vertex(index).Coord());
    }
    
    T->Deflection(theDefFace);
    
    // storage of triangulation in BRep.
    BRep_Builder B1;
    //TopLoc_Location loc = theFace.Location();
    if (!loc.IsIdentity()) {
      gp_Trsf tr = loc.Transformation();
      tr.Invert();
      for (i = Nodes.Lower(); i <= Nodes.Upper(); i++) 
        Nodes(i).Transform(tr);
    }
    B1.UpdateFace(theFace, T);

    // implement polygons on triangulation in the face:
    BRepMesh_DataMapIteratorOfDataMapOfShapePairOfPolygon It(myInternaledges);

    for (; It.More(); It.Next()) {
      const BRepMesh_PairOfPolygon& pair = It.Value();
      const Handle(Poly_PolygonOnTriangulation)& NOD1 = pair.First();
      const Handle(Poly_PolygonOnTriangulation)& NOD2 = pair.Last();

      // lock mutex to prevent parallel change of the same data
      Standard_Mutex* aMutex = theMutexProvider.GetMutex(It.Key());
      Standard_Mutex::Sentry aSentry (aMutex);

      if ( NOD1 == NOD2 ) {
        B.UpdateEdge(TopoDS::Edge(It.Key()), NullPoly, TOld,loc);
        B.UpdateEdge(TopoDS::Edge(It.Key()), NOD1, T, loc);
      }
      else {
        B.UpdateEdge(TopoDS::Edge(It.Key()), NullPoly,   TOld,loc);
        B.UpdateEdge(TopoDS::Edge(It.Key()), NOD1, NOD2, T, loc);
      }
    }
  }
 }

 catch(Standard_Failure)
 {
   //   MESH_FAILURE(theFace);
 }
}


//=======================================================================
//function : Triangle
//purpose  : 
//=======================================================================

const BRepMesh_Triangle& BRepMesh_FastDiscretFace::Triangle(const Standard_Integer Index) const
{
  return myStructure->GetElement(Index);
}

//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

/*Standard_Integer BRepMesh_FastDiscretFace::NbEdges() const
{
  return myStructure->NbLinks();
}*/

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const BRepMesh_Edge& BRepMesh_FastDiscretFace::Edge(const Standard_Integer Index) const
{
  return myStructure->GetLink(Index);
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const BRepMesh_Vertex& BRepMesh_FastDiscretFace::Vertex(const Standard_Integer Index) const
{
  return myStructure->GetNode(Index);
}

//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

const gp_Pnt& BRepMesh_FastDiscretFace::Pnt(const Standard_Integer Index) const
{
  return myLocation3d(myStructure->GetNode(Index).Location3d());
}

//=======================================================================
//function : FindUV
//purpose  : 
//=======================================================================

gp_XY BRepMesh_FastDiscretFace::FindUV(const TopoDS_Vertex&                  theV,
                                       const gp_Pnt2d&                       theXY,
                                       const Standard_Integer                theIp,
                                       const Handle(BRepAdaptor_HSurface)&   theSFace,
                                       const Standard_Real                   theMinDist,
                                       const Handle(BRepMesh_FaceAttribute)& theFaceAttribute,
                                       BRepMesh_DataMapOfIntegerListOfXY&    theLocation2dMap)
{
  gp_XY anUV;
  if (theLocation2dMap.IsBound(theIp))
  {
    BRepMesh_ListOfXY& L = theLocation2dMap.ChangeFind(theIp);
    anUV = L.First();
    if (L.Extent() != 1)
    {
      BRepMesh_ListIteratorOfListOfXY it(L);
      it.Next();
      Standard_Real dd, dmin = theXY.Distance(gp_Pnt2d(anUV));
      for (; it.More(); it.Next())
      {
        dd = theXY.Distance(gp_Pnt2d(it.Value()));
        if (dd < dmin)
        {
          anUV = it.Value();
          dmin = dd;
        }
      }
    }

    const Standard_Real tol = Min(2. * BRep_Tool::Tolerance(theV), theMinDist);

    Standard_Real aDiffU, aDiffV;
    
    if ( theFaceAttribute.IsNull() )
    {
      aDiffU = theSFace->LastUParameter() - theSFace->FirstUParameter();
      aDiffV = theSFace->LastVParameter() - theSFace->FirstVParameter();
    }
    else
    {
      aDiffU = theFaceAttribute->GetUMax() - theFaceAttribute->GetUMin();
      aDiffV = theFaceAttribute->GetVMax() - theFaceAttribute->GetVMin();
    }

    const Standard_Real Utol2d = .5 * aDiffU;
    const Standard_Real Vtol2d = .5 * aDiffV;

    const gp_Pnt p1 = theSFace->Value(anUV.X(), anUV.Y());
    const gp_Pnt p2 = theSFace->Value(theXY.X(), theXY.Y());

    if (Abs(anUV.X() - theXY.X()) > Utol2d ||
        Abs(anUV.Y() - theXY.Y()) > Vtol2d ||
        !p1.IsEqual(p2, tol))
    {
      anUV = theXY.Coord();
      L.Append(anUV);
    }
  }
  else
  {
    anUV = theXY.Coord();
    BRepMesh_ListOfXY L;
    L.Append(anUV);
    theLocation2dMap.Bind(theIp, L);
  }
  return anUV;
}


static Standard_Boolean GetVertexParameters(const TopoDS_Vertex& theVert, 
                                            const TopoDS_Face& theFace,
                                            gp_Pnt2d& thePoint)
{
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(theFace,L);
  L = L.Predivided(theVert.Location());
  BRep_ListIteratorOfListOfPointRepresentation itpr =
    ((*((Handle(BRep_TVertex)*) &theVert.TShape()))->Points());
  // Check first if there are PointRepresentation (case non Manifold)

  while (itpr.More()) {
    if (itpr.Value()->IsPointOnSurface(S,L)) {
      thePoint.SetCoord(itpr.Value()->Parameter(),
                        itpr.Value()->Parameter2());
      return Standard_True;
    }
    itpr.Next();
  }
  return Standard_False;
}

//=======================================================================
//function : Add
//purpose  : method intended to add internal myVertices in triangulation.
//=======================================================================
void BRepMesh_FastDiscretFace::Add(const TopoDS_Vertex&                theVert, 
                                   const TopoDS_Face&                  theFace, 
                                   const Handle(BRepAdaptor_HSurface)& thegFace)
{
  const TopAbs_Orientation anOrient = theVert.Orientation();
  gp_Pnt2d uvXY;
  if( anOrient != TopAbs_INTERNAL || !GetVertexParameters(theVert,theFace,uvXY))
    return;
  Standard_Integer indVert =0;
  if (myVertices.IsBound(theVert))
    indVert = myVertices.Find(theVert);
  else
  {
    myNbLocat++;
    myLocation3d.Bind(myNbLocat, BRep_Tool::Pnt(theVert));
    indVert = myNbLocat;
    myVertices.Bind(theVert, indVert);
  }
  Standard_Real mindist = BRep_Tool::Tolerance(theVert);
  // gp_Pnt2d uvXY = BRep_Tool::Parameters(theVert,theFace);
  gp_XY anUV = FindUV(theVert, uvXY, indVert, thegFace, mindist, myAttrib, myLocation2d);
  BRepMesh_Vertex vf(anUV, indVert, BRepMesh_Fixed);
  Standard_Integer ivff = myStructure->AddNode(vf);
  Standard_Integer isvf = myVemap.FindIndex(ivff);
  if (isvf == 0) isvf = myVemap.Add(ivff);  
}
