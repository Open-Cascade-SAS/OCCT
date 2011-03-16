// File:	BRepMesh_FastDiscretFace.cxx
// Created:	
// Author:	Ekaterina SMIRNOVA
// Copyright: Open CASCADE SAS 2008

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
#include <Extrema_LocateExtPC.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <SortTools_ShellSortOfReal.hxx>
#include <Poly_Connect.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <TCollection_CompareOfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_ListOfTransient.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <NCollection_Map.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomLib.hxx>
#include <Bnd_Box2d.hxx>

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
BRepMesh_FastDiscretFace::BRepMesh_FastDiscretFace(const Standard_Real    angl,
				   const Standard_Boolean ws,
				   const Standard_Boolean inshape,
				   const Standard_Boolean shapetrigu): 
  angle(angl), WithShare(ws), nbLocat(0), 
  myshapetrigu(shapetrigu), myinshape(inshape),
  myInternalVerticesMode(Standard_True)
{
  myAllocator = new NCollection_IncAllocator(64000);
}

//=======================================================================
//function : Add(face)
//purpose  : 
//=======================================================================

void BRepMesh_FastDiscretFace::Add(const TopoDS_Face& theface,
                                   const Handle(BRepMesh_FaceAttribute)& attrib,
                                   const TopTools_DataMapOfShapeReal& mapdefle)
{
#ifndef DEB_MESH
  try
  {
    OCC_CATCH_SIGNALS
#endif
    TopoDS_Face face = theface;
    myattrib = attrib;
    face.Orientation(TopAbs_FORWARD);
    structure.Nullify();
    Handle(NCollection_IncAllocator) anAlloc = Handle(NCollection_IncAllocator)::DownCast(myAllocator);
    anAlloc->Reset(Standard_False);  
    structure=new BRepMesh_DataStructureOfDelaun(anAlloc);
    BRepAdaptor_Surface  BS(face, Standard_False);
    Handle(BRepAdaptor_HSurface) gFace = new BRepAdaptor_HSurface(BS);
    
    GeomAbs_SurfaceType thetype;
    thetype = BS.GetType();
    
    gp_Pnt2d uvFirst, uvLast;

    TopAbs_Orientation orFace = face.Orientation();
    TopLoc_Location loc;

    if (!WithShare) {          
      vertices.Clear();
      edges.Clear();
    }

    mylistver.Clear();
    myvemap.Clear();
    mylocation2d.Clear();
    internaledges.Clear();

    Standard_Integer i = 1;
    Standard_Integer nbEdge = 0;
    Standard_Real savangle = angle;
    Standard_Integer ipn = 0;

    TColStd_SequenceOfReal aFSeq, aLSeq;
    TColGeom2d_SequenceOfCurve aCSeq;
    TopTools_SequenceOfShape aShSeq;
    Standard_Real defedge = 0;

    TopoDS_Iterator exW(face);
    for (; exW.More(); exW.Next()) {
      const TopoDS_Shape& aWire = exW.Value();
      if (aWire.ShapeType() != TopAbs_WIRE)
	      continue;
      TopoDS_Iterator ex(aWire);
      for(; ex.More(); ex.Next()) {
	      const TopoDS_Edge& edge = TopoDS::Edge(ex.Value());
	      nbEdge++;
	      Standard_Real f,l;
	      if(edge.IsNull()) continue;
	      Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(edge, face, f, l);
	      if (C.IsNull()) continue;

	      aFSeq.Append(f);
	      aLSeq.Append(l);
	      aCSeq.Append(C);
	      aShSeq.Append(edge);
	
	      Update(edge, face, C, mapdefle(edge), f, l);
	      angle = savangle;
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
      nbVertices = myvemap.Extent();
    }
    
    // essai de determination de la longueur vraie:
    // akm (bug OCC16) : We must calculate these measures in non-singular
    //     parts of face. Let`s try to compute average value of three
    //     (umin, (umin+umax)/2, umax), and respectively for v.
    //                 vvvvv
    //Standard_Real longu = 0.0, longv = 0.0; //, last , first;
    //gp_Pnt P11, P12, P21, P22, P31, P32;

    Standard_Real umax = myattrib->GetUMax();
    Standard_Real umin = myattrib->GetUMin();
    Standard_Real vmax = myattrib->GetVMax();
    Standard_Real vmin = myattrib->GetVMin();
    
    TColStd_Array1OfInteger tabvert_corr(1, nbVertices);
    gp_Pnt2d p2d;
    
    // Check the necessity to fill the map of parameters
    const Standard_Boolean useUVParam = (thetype == GeomAbs_Torus ||
					 thetype == GeomAbs_BezierSurface ||
					 thetype == GeomAbs_BSplineSurface);
    myUParam.Clear(); 
    myVParam.Clear();
  
    BRepMesh_IDMapOfNodeOfDataStructureOfDelaun aMoveNodes(myvemap.Extent());
    
    for (i = 1; i <= structure->NbNodes(); i++)
    {
      const BRepMesh_Vertex& v = structure->GetNode(i);
      p2d = v.Coord();
      if (useUVParam) {
	      myUParam.Add(p2d.X());
	      myVParam.Add(p2d.Y());
      }
      gp_XY res;
      res.SetCoord((p2d.X()-(myattrib->GetMinX()))/(myattrib->GetDeltaX()),
		   (p2d.Y()-(myattrib->GetMinY()))/(myattrib->GetDeltaY()));
      BRepMesh_Vertex v_new(res,v.Location3d(),v.Movability());
      const MeshDS_ListOfInteger& alist = structure->GetNodeList(i);
      aMoveNodes.Add(v_new,alist);
      tabvert_corr(i) = i;
    }    
    structure->ReplaceNodes(aMoveNodes);
    
    Standard_Boolean rajout;
    
    BRepMesh_ClassifierPtr& classifier = attrib->GetClassifier();

    switch (thetype)
    {
    case GeomAbs_Plane:
      rajout = !classifier->NaturalRestriction();
      break;
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
      rajout = Standard_True;
      break;
    default:
      rajout = Standard_False;
    }  

    BRepMesh_Delaun trigu(structure, tabvert_corr, orFace==TopAbs_FORWARD);
    
    //removed all free edges from triangulation
    Standard_Integer nbLinks = structure->NbNodes(); 
    for(i = 1; i <= nbLinks; i++) 
    {
      if(structure->ElemConnectedTo(i).Extent() < 1)
      {
        BRepMesh_Edge& anEdge = (BRepMesh_Edge&)trigu.GetEdge(i);
        if(anEdge.Movability()==MeshDS_Deleted)
          continue;
        anEdge.SetMovability(MeshDS_Free);
        structure->RemoveLink(i);
      }
    }

    Standard_Boolean isaline;
    isaline = ((umax-umin)<1.e-05) || ((vmax-vmin)<1.e-05);
    
    Standard_Real aDef = -1;
    if (!isaline && structure->ElemOfDomain().Extent() > 0) {
      TColStd_ListOfInteger badTri, nulTri;
      
      if(!rajout)
      {
	      aDef = Control(gFace, attrib->GetDefFace(), mylistver, badTri, nulTri, trigu, Standard_True);
	      if( aDef > attrib->GetDefFace() || aDef < 0.)
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
        InternalVertices(gFace, mylistver, attrib->GetDefFace(), classifier);
	
	      if (mylistver.Extent() > 0) {
	        BRepMesh_Array1OfVertexOfDelaun verttab(1, mylistver.Extent());
	        BRepMesh_ListIteratorOfListOfVertex itVer(mylistver);
	        ipn = 1;
	        for (; itVer.More(); itVer.Next())
	          verttab(ipn++) = itVer.Value();
	        trigu.AddVertices(verttab);
	      }
	      //control internal points
	      BRepMesh_ListOfVertex vvlist;
	      aDef = Control(gFace, attrib->GetDefFace(), vvlist, badTri, nulTri, trigu, Standard_False);
	      mylistver.Append(vvlist);
      }
    }

    //modify structure back
    aMoveNodes.Clear();
    Standard_Real deltaX = myattrib->GetDeltaX();
    Standard_Real deltaY = myattrib->GetDeltaY();
    for (i = 1; i <= structure->NbNodes(); i++)
    {
      const BRepMesh_Vertex& v = structure->GetNode(i);
      p2d = v.Coord();
      gp_XY res;
      res.SetCoord(p2d.X()*deltaX+umin,p2d.Y()*deltaY+vmin);
      BRepMesh_Vertex v_new(res,v.Location3d(),v.Movability());
      const MeshDS_ListOfInteger& alist = structure->GetNodeList(i);
      aMoveNodes.Add(v_new,alist);
    }
    structure->ReplaceNodes(aMoveNodes);
  
    AddInShape(face, (aDef < 0.0)? attrib->GetDefFace() : aDef);
#ifndef DEB_MESH
  }
  catch(Standard_Failure)
  {
    BRep_Builder B;
    Handle(Poly_Triangulation) TNull;
    B.UpdateFace(theface,TNull);
  }
#endif // DEB_MESH
  structure.Nullify();
  myattrib.Nullify();
}

//=======================================================================
//function : Update(edge)
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_FastDiscretFace::Update(const TopoDS_Edge&  edge,
					  const TopoDS_Face&  face,
                                          const Handle(Geom2d_Curve)& C2d,
					  const Standard_Real defedge,
                                          const Standard_Real first,
                                          const Standard_Real last)
{
  TopLoc_Location l;
  Handle(Poly_Triangulation) T, TNull;
  Handle(Poly_PolygonOnTriangulation) Poly, NullPoly;

  Standard_Integer i = 1;
  Standard_Boolean found = Standard_False;
  do
  {
    BRep_Tool::PolygonOnTriangulation(edge,Poly,T,l,i);
    i++;
    if (!found && !T.IsNull() && T->HasUVNodes() && 
	!Poly.IsNull() && Poly->HasParameters())
    {
      if (Poly->Deflection() <= 1.1*defedge)
      {
        // 2d vertex indices
        TopAbs_Orientation orEdge = edge.Orientation();
        Standard_Integer iv1, iv2, ivl;
        Standard_Integer isv1, isv, isvl;
        
        // Get range on 2d curve
	Standard_Real wFirst, wLast;
	BRep_Tool::Range(edge, face, wFirst, wLast);

        // Get end points on 2d curve
	gp_Pnt2d uvFirst, uvLast;
	BRep_Tool::UVPoints(edge, face, uvFirst, uvLast);

        // Get vertices
	TopoDS_Vertex pBegin, pEnd;
	TopExp::Vertices(edge,pBegin,pEnd);

	const Standard_Boolean sameUV =
          uvFirst.IsEqual(uvLast, Precision::PConfusion());

	//Controle vertice tolerances
        BRepAdaptor_Surface  BS(face, Standard_False);
        Handle(BRepAdaptor_HSurface) gFace = new BRepAdaptor_HSurface(BS);


	gp_Pnt pFirst = gFace->Value(uvFirst.X(), uvFirst.Y());
	gp_Pnt pLast  = gFace->Value(uvLast.X(), uvLast.Y());
	
	Standard_Real mindist = 10. * Max(pFirst.Distance(BRep_Tool::Pnt(pBegin)), 
					  pLast.Distance(BRep_Tool::Pnt(pEnd)));

	if (mindist < BRep_Tool::Tolerance(pBegin) ||
	    mindist < BRep_Tool::Tolerance(pEnd) ) mindist = defedge;

	if (sameUV)
        {
	  // 1. est-ce vraiment sameUV sans etre denegere
	  gp_Pnt2d uvF, uvL;
	  C2d->D0(first, uvF);
	  C2d->D0(last, uvL);
	  if (!uvFirst.IsEqual(uvF, Precision::PConfusion())) {
	    uvFirst = uvF;
	  }
	  if (!uvLast.IsEqual(uvL, Precision::PConfusion())) {
	    uvLast = uvL; 
	  }
	}

	const TColgp_Array1OfPnt&      Nodes   = T->Nodes();
	const TColStd_Array1OfInteger& Indices = Poly->Nodes();
	Handle(TColStd_HArray1OfReal)  Param   = Poly->Parameters();

	const Standard_Integer nbnodes = Indices.Length();
	TColStd_Array1OfInteger NewNodes(1, nbnodes);
        TColStd_Array1OfInteger NewNodInStruct(1, nbnodes);

        gp_Pnt P3d;
	gp_XY theUV;

        // Process first vertex
        Standard_Integer ipf;
	if (vertices.IsBound(pBegin))
        {
	  ipf = vertices.Find(pBegin);
	}
        else
        {
	  if (sameUV && vertices.IsBound(pEnd))
          {
	    ipf = vertices.Find(pEnd);
	  }
	  else
          {
	    P3d = Nodes(Indices(1));
	    if (!l.IsIdentity())
              P3d.Transform(l.Transformation());
            nbLocat++;
            Location3d.Bind(nbLocat,P3d);
	    ipf = nbLocat;
	  }
          vertices.Bind(pBegin,ipf);
	} 
	NewNodInStruct(1) = ipf;
	theUV = FindUV(pBegin, uvFirst, ipf, gFace, mindist);
	BRepMesh_Vertex vf(theUV,ipf,MeshDS_Frontier);
        iv1 = structure->AddNode(vf);
        isv1 = myvemap.FindIndex(iv1);
        if (isv1 == 0) isv1 = myvemap.Add(iv1);
	NewNodes(1) = isv1;

        // Process last vertex
        Standard_Integer ipl;
        if (pEnd.IsSame(pBegin))
        {
          ipl = ipf;
        }
        else
        {
          if (vertices.IsBound(pEnd))
          {
            ipl = vertices.Find(pEnd);
          }
          else
          {
            if (sameUV)
            {
              ipl = ipf;
              ivl = iv1;
              isv1 = isv1;
            }
            else
            {
              nbLocat++;
              Location3d.Bind(nbLocat,Nodes(Indices(nbnodes)).Transformed(l.Transformation()));
              ipl = nbLocat;
            }
            vertices.Bind(pEnd,ipl);
          }
        }
	NewNodInStruct(nbnodes) = ipl;
	theUV = FindUV(pEnd, uvLast, ipl, gFace, mindist);
	BRepMesh_Vertex vl(theUV,ipl,MeshDS_Frontier);
        
        ivl = structure->AddNode(vl);
        isvl = myvemap.FindIndex(ivl);
        if (isvl == 0) isvl = myvemap.Add(ivl);
        
	NewNodes(nbnodes) = isvl;
	
	gp_Pnt2d uv;
	BRepMesh_Vertex v;
	
	if (BRep_Tool::SameParameter(edge))
        {
	  for (i = 2; i < Indices.Length(); i++)
          {
            // Record 3d point
	    P3d = Nodes(Indices(i));
	    if (!l.IsIdentity())
              P3d.Transform(l.Transformation());
	    nbLocat++;
	    Location3d.Bind(nbLocat, P3d);
	    NewNodInStruct(i) = nbLocat;
            // Record 2d point
	    uv = C2d->Value(Param->Value(i));
	    v.Initialize(uv.Coord(), nbLocat, MeshDS_Frontier);
            iv2 = structure->AddNode(v);
            isv = myvemap.FindIndex(iv2);
            if (isv == 0) isv = myvemap.Add(iv2);
	    NewNodes(i) = isv;
            
            //add links
            if (orEdge == TopAbs_FORWARD)
              structure->AddLink(BRepMesh_Edge(iv1,iv2,MeshDS_Frontier));
	    else if (orEdge == TopAbs_REVERSED)
	      structure->AddLink(BRepMesh_Edge(iv2,iv1,MeshDS_Frontier));
	    else if (orEdge == TopAbs_INTERNAL)
	      structure->AddLink(BRepMesh_Edge(iv1,iv2,MeshDS_Fixed));
	    iv1 = iv2;  
	  }
          
          // last point
          if (iv1 != ivl) {
	    if (orEdge == TopAbs_FORWARD)
	      structure->AddLink(BRepMesh_Edge(iv1,ivl,MeshDS_Frontier));
	    else if (orEdge == TopAbs_REVERSED)
	      structure->AddLink(BRepMesh_Edge(ivl,iv1,MeshDS_Frontier));
	    else if (orEdge == TopAbs_INTERNAL)
	      structure->AddLink(BRepMesh_Edge(iv1,ivl,MeshDS_Fixed));
	  }

          
	}
        else
        {
	  const Standard_Real wFold = Param->Value(Param->Lower());
	  const Standard_Real wLold = Param->Value(Param->Upper());

	  Standard_Real wKoef = 1.;
	  if ((wFold != wFirst || wLold != wLast) && wLold != wFold)
          {
	    wKoef = (wLast - wFirst) / (wLold - wFold);
	  }
	  
	  BRepAdaptor_Curve cons(edge, face);
	  Extrema_LocateExtPC pcos;
	  pcos.Initialize(cons, cons.FirstParameter(), 
			  cons.LastParameter(), Precision::PConfusion());

	  Standard_Real wPrev;
	  Standard_Real wCur      = wFirst;
	  Standard_Real wCurFound = wFirst;
	  for (i = 2; i < Indices.Length(); i++)
          {
            // Record 3d point
	    P3d = Nodes(Indices(i));
	    if (!l.IsIdentity())
              P3d.Transform(l.Transformation());
	    nbLocat++;
	    Location3d.Bind(nbLocat, P3d);
	    NewNodInStruct(i) = nbLocat;
            // Record 2d point
	    wPrev = wCur;
	    wCur  = wFirst + wKoef*(Param->Value(i) - wFold);
            wCurFound += (wCur - wPrev);
	    pcos.Perform(P3d, wCurFound);
	    if (pcos.IsDone()) wCurFound = pcos.Point().Parameter();
	    C2d->D0(wCurFound, uv);
	    v.Initialize(uv.Coord(), nbLocat, MeshDS_Frontier);
            iv2 = structure->AddNode(v);
            isv = myvemap.FindIndex(iv2);
            if (isv == 0) isv = myvemap.Add(iv2);
	    NewNodes(i) = isv;

            
            //add links
            if (orEdge == TopAbs_FORWARD)
              structure->AddLink(BRepMesh_Edge(iv1,iv2,MeshDS_Frontier));
	    else if (orEdge == TopAbs_REVERSED)
	      structure->AddLink(BRepMesh_Edge(iv2,iv1,MeshDS_Frontier));
	    else if (orEdge == TopAbs_INTERNAL)
	      structure->AddLink(BRepMesh_Edge(iv1,iv2,MeshDS_Fixed));
	    iv1 = iv2;              
	  }
          
          // last point
          if (iv1 != ivl) {
	    if (orEdge == TopAbs_FORWARD)
	      structure->AddLink(BRepMesh_Edge(iv1,ivl,MeshDS_Frontier));
	    else if (orEdge == TopAbs_REVERSED)
	      structure->AddLink(BRepMesh_Edge(ivl,iv1,MeshDS_Frontier));
	    else if (orEdge == TopAbs_INTERNAL)
	      structure->AddLink(BRepMesh_Edge(iv1,ivl,MeshDS_Fixed));
	  }
	}

	Handle(Poly_PolygonOnTriangulation) P1 =
          new Poly_PolygonOnTriangulation(NewNodes, Param->Array1());
	P1->Deflection(defedge);
	if (internaledges.IsBound(edge))
        {
	  BRepMesh_PairOfPolygon& pair = internaledges.ChangeFind(edge);
          if (edge.Orientation() == TopAbs_REVERSED)
            pair.Append(P1);
          else
            pair.Prepend(P1);
	}
	else
        {
	  BRepMesh_PairOfPolygon pair1;
	  pair1.Append(P1);
	  internaledges.Bind(edge, pair1);
	}

	Handle(Poly_PolygonOnTriangulation) P2 =
          new Poly_PolygonOnTriangulation(NewNodInStruct, Param->Array1());
	P2->Deflection(defedge);
	BRepMesh_PairOfPolygon pair;
	pair.Append(P2);
	edges.Bind(edge, pair);

	found = Standard_True;
      }
      else
      {
	BRep_Builder B;
	B.UpdateEdge(edge,NullPoly,T,l);
	B.UpdateFace(face,TNull);
      }
    }
    else if (!T.IsNull() && !T->HasUVNodes())
    {
      BRep_Builder B;
      B.UpdateEdge(edge,NullPoly,T,l);
      B.UpdateFace(face,TNull);
    }
  }
  while (!Poly.IsNull());

  return found;
}



//=======================================================================
//function : InternalVertices
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscretFace::InternalVertices ( const Handle(BRepAdaptor_HSurface)& caro,
                                                  BRepMesh_ListOfVertex&              InternalV,
                                                  const Standard_Real                 defface,
                                                  const BRepMesh_ClassifierPtr&       classifier)
{
  BRepMesh_Vertex newV;
  gp_Pnt2d p2d;
  gp_Pnt p3d;
  
  // travail suivant le type de surface


  const BRepAdaptor_Surface& BS = *(BRepAdaptor_Surface*)&(caro->Surface());
  GeomAbs_SurfaceType thetype = caro->GetType();

  Standard_Real umax = myattrib->GetUMax();
  Standard_Real umin = myattrib->GetUMin();
  Standard_Real vmax = myattrib->GetVMax();
  Standard_Real vmin = myattrib->GetVMin();
  Standard_Real deltaX = myattrib->GetDeltaX();
  Standard_Real deltaY = myattrib->GetDeltaY();

  if (thetype == GeomAbs_Plane && !classifier->NaturalRestriction())
  {
    // rajout d`un seul point au milieu.
    const Standard_Real U = 0.5*(umin+umax);
    const Standard_Real V = 0.5*(vmin+vmax);
    if (classifier->Perform(gp_Pnt2d(U, V)) == TopAbs_IN)
    {
      // Record 3d point
      BRepMesh_GeomTool::D0(caro, U, V, p3d);
      nbLocat++;
      Location3d.Bind(nbLocat, p3d);
      // Record 2d point
      p2d.SetCoord((U-umin)/deltaX, (V-vmin)/deltaY);
      newV.Initialize(p2d.XY(), nbLocat, MeshDS_Free);
      InternalV.Append(newV);
    }
  }
  else if (thetype == GeomAbs_Sphere)
  {
    gp_Sphere S = BS.Sphere();
    const Standard_Real R = S.Radius();

    // Calculate parameters for iteration in V direction
    Standard_Real Dv = 1.0 - (defface/R);
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
	      if (classifier->Perform(gp_Pnt2d(pasu, pasv)) == TopAbs_IN)
        {
          // Record 3d point
#ifdef DEB_MESH_CHRONO
	  D0Internal++;
#endif
	        ElSLib::D0(pasu, pasv, S, p3d);
	        nbLocat++;
	        Location3d.Bind(nbLocat, p3d);
          // Record 2d point
	        p2d.SetCoord((pasu-umin)/deltaX, (pasv-vmin)/deltaY);
	        newV.Initialize(p2d.XY(), nbLocat, MeshDS_Free);
	        InternalV.Append(newV);
	      }
      }
    }
  }
  else if (thetype == GeomAbs_Cylinder)
  {
    gp_Cylinder S = BS.Cylinder();
    const Standard_Real R = S.Radius();

    // Calculate parameters for iteration in U direction
    Standard_Real Du = 1.0 - (defface/R);
    if (Du < 0.0) Du = 0.0;
    Du = 2.0 * ACos (Du);
    if (Du > angle) Du = angle;
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
	      if (classifier->Perform(gp_Pnt2d(pasu, pasv)) == TopAbs_IN)
        {
          // Record 3d point
	        ElSLib::D0(pasu, pasv, S, p3d);
          nbLocat++;
          Location3d.Bind(nbLocat, p3d);
          // Record 2d point
	        p2d.SetCoord((pasu-umin)/deltaX, (pasv-vmin)/deltaY);
	        newV.Initialize(p2d.XY(), nbLocat, MeshDS_Free);
          InternalV.Append(newV);
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
    Du = Max(1.0e0 - (defface/R),0.0e0);
    Du  = (2.0 * ACos (Du));
    Standard_Integer nbU = (Standard_Integer) ( (umax-umin)/Du );
    Standard_Integer nbV = (Standard_Integer) ( nbU*(vmax-vmin)/((umax-umin)*R) );
    Du = (umax-umin)/(nbU+1);
    Dv = (vmax-vmin)/(nbV+1);

    Standard_Real pasvmax = vmax-Dv*0.5, pasumax = umax-Du*0.5;
    for (pasv = vmin + Dv; pasv < pasvmax; pasv += Dv) {
      for (pasu = umin + Du; pasu < pasumax; pasu += Du) {
	      if (classifier->Perform(gp_Pnt2d(pasu, pasv)) == TopAbs_IN)
        {
          // Record 3d point
	        ElSLib::D0(pasu, pasv, C, p3d);
          nbLocat++;
          Location3d.Bind(nbLocat, p3d);
          // Record 2d point
          p2d.SetCoord((pasu-umin)/deltaX, (pasv-vmin)/deltaY);
	        newV.Initialize(p2d.XY(), nbLocat, MeshDS_Free);
          InternalV.Append(newV);
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
    Dv = Max(1.0e0 - (defface/r),0.0e0) ;
    Standard_Real oldDv = 2.0 * ACos (Dv);
    oldDv = Min(oldDv, angle);
    Dv  =  0.9*oldDv; //TWOTHIRD * oldDv;
    Dv = oldDv;
    
    Standard_Integer nbV = Max((Standard_Integer)((vmax-vmin)/Dv), 2);
    Dv = (vmax-vmin)/(nbV+1);
    Standard_Real ru = R + r;
    if (ru > 1.e-16)
    {
      Du  = 2.0 * ACos(Max(1.0 - (defface/ru),0.0));
      if (angle < Du) Du = angle;
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
      // comme on recupere les points des edges.
      // dans ce cas, les points ne sont pas representatifs.
            
      //-- On choisit DeltaX et DeltaY de facon a ce qu on ne saute pas 
      //-- de points sur la grille
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
      
      // Calculate DU, sort array of parameters
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
	          if (classifier->Perform(gp_Pnt2d(pasu, pasv)) == TopAbs_IN)
            {
              // Record 3d point
	            ElSLib::D0(pasu, pasv, T, p3d);
	            nbLocat++;
	            Location3d.Bind(nbLocat, p3d);
              // Record 2d point
	            p2d.SetCoord((pasu-umin)/deltaX, (pasv-vmin)/deltaY);
	            newV.Initialize(p2d.XY(), nbLocat, MeshDS_Free);
	            InternalV.Append(newV);
	          }
	        }
	      }
      }
    }
  }
  else if (thetype == GeomAbs_BezierSurface || thetype == GeomAbs_BSplineSurface)
  {
    Standard_Integer i, j;

    Handle(TColStd_HArray1OfReal) anUKnots, anVKnots;
    Standard_Integer aNbUNkots,aNbVNkots;
    Handle(Geom_Surface) B;
    if (thetype == GeomAbs_BezierSurface) {
      anUKnots = new TColStd_HArray1OfReal(1,2);
      anVKnots = new TColStd_HArray1OfReal(1,2);
      anUKnots->SetValue(1,0.);
      anUKnots->SetValue(2,1.);
      anVKnots->SetValue(1,0.);
      anVKnots->SetValue(2,1.);
      aNbUNkots = 2;
      aNbVNkots = 2;
      B = BS.Bezier();
    }
    else {
      Handle(Geom_BSplineSurface) aSurf = BS.BSpline();
      B = aSurf;
      aNbUNkots = aSurf->NbUKnots();
      aNbVNkots = aSurf->NbVKnots();
      anUKnots = new TColStd_HArray1OfReal(1,aNbUNkots);
      anVKnots = new TColStd_HArray1OfReal(1,aNbVNkots);

      aSurf->UKnots(anUKnots->ChangeArray1());
      aSurf->VKnots(anVKnots->ChangeArray1());
    }

    Standard_Real ddu = caro->UResolution(defface)*5.;

    Standard_Real aDUmin = (umax-umin)/5.;
    if(ddu > aDUmin)
      ddu = aDUmin;
    // Sort sequence of U parameters
    TColStd_SequenceOfReal ParamU;
    Standard_Integer anUdegree = caro->UDegree();
     
    Standard_Real aU1 = anUKnots->Value(1);
    for( i = 1; i < aNbUNkots; i++)
    {
      Standard_Real aStart = anUKnots->Value(i);
      Standard_Real aEnd = anUKnots->Value(i+1);
      Standard_Integer aNbPoints = anUdegree-1;
      Standard_Real aStep = (aEnd-aStart)/(aNbPoints+1);
      for( Standard_Integer anInd = 0; anInd<= aNbPoints; anInd++)
      {
        Standard_Real aU2 = aStart+anInd*aStep;
        if((aU2 - aU1) > ddu)
        {
          ParamU.Append(aU2);
          aU1 = aU2;
        }
      }
    }
    ParamU.Append(anUKnots->Value(aNbUNkots));
    Standard_Integer ParamULength = ParamU.Length();

    Standard_Real ddv = caro->VResolution(defface)*5.;

    Standard_Real aDVmin = (vmax-vmin)/5.;
    if(ddv > aDVmin)
      ddv = aDVmin;
    // Sort sequence of V parameters
    TColStd_SequenceOfReal ParamV; 
    Standard_Integer anVdegree = caro->VDegree();
      
    Standard_Real aV1 = anVKnots->Value(1);
    for( i = 1; i < aNbVNkots; i++)
    {
      Standard_Real aStart = anVKnots->Value(i);
      Standard_Real aEnd = anVKnots->Value(i+1);
      Standard_Integer aNbPoints = anVdegree-1;
      Standard_Real aStep = (aEnd-aStart)/(aNbPoints+1);
      for( Standard_Integer anInd = 0; anInd<= aNbPoints; anInd++)
      {
        Standard_Real aV2 = aStart+anInd*aStep;
        if ((aV2 - aV1) > ddv)
        { 
          ParamV.Append(aV2);
          aV1 = aV2;
        }
      }
    }
    ParamV.Append(anVKnots->Value(aNbVNkots));
    Standard_Integer ParamVLength = ParamV.Length();

    // controle des isos U et insertion eventuelle:

    gp_Pnt P1, P2, PControl;
    Standard_Real u, v, dist, V1, V2, U1, U2;

    // precision for compare square distances
    double dPreci = Precision::Confusion()*Precision::Confusion();

    // Insert V parameters by deflection criterion
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
        if (dist > defface) {
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
        if (dist > defface) {
          // insertion 
          ParamU.InsertBefore(j, u);
          ParamULength++;
        }
        else {
          //check normal
          U1 = U2;
          P1 = P2;
          if (j < ParamULength) {
            // Classify intersection point
            if (classifier->Perform(gp_Pnt2d(U1, v)) == TopAbs_IN)
            {
              // Record 3d point
              nbLocat++;
              Location3d.Bind(nbLocat, P1);
              // Record 2d point
              p2d.SetCoord((U1-umin)/deltaX, (v-vmin)/deltaY);
              newV.Initialize(p2d.XY(), nbLocat, MeshDS_Free);
              InternalV.Append(newV);
            }
          }
          j++;
	      }
      }
    }
  }
  else {
    const Standard_Real theangle = 0.35;

    Standard_Integer i, j, nbpointsU = 10, nbpointsV = 10;
    Adaptor3d_IsoCurve tabu[10], tabv[10];

    TColStd_SequenceOfReal ParamU, ParamV;
    Standard_Real u, v, du, dv;
    Standard_Integer iu, iv;
    Standard_Real f, l;

    du = (umax-umin) / (nbpointsU+1); dv = (vmax-vmin) / (nbpointsV+1);

    for (iu = 1; iu <= nbpointsU; iu++) {
      u = umin + iu*du;
      tabu[iu-1].Load(caro);
      tabu[iu-1].Load(GeomAbs_IsoU, u);
    }

    for (iv = 1; iv <= nbpointsV; iv++) {
      v = vmin + iv*dv;
      tabv[iv-1].Load(caro);
      tabv[iv-1].Load(GeomAbs_IsoV, v);
    }

    Standard_Integer imax = 1, MaxV = 0;
    
    GCPnts_TangentialDeflection* tabGU = new GCPnts_TangentialDeflection[nbpointsU];

    for (i = 0; i <= nbpointsU-1; i++) {
      f = Max(vmin, tabu[i].FirstParameter());
      l = Min(vmax, tabu[i].LastParameter());
      GCPnts_TangentialDeflection theDeflection(tabu[i], f, l, theangle, 0.7*defface, 2);
      tabGU[i] = theDeflection;
      if (tabGU[i].NbPoints() > MaxV) {
	      MaxV = tabGU[i].NbPoints();
	      imax = i;
      }
    }
    
    // recuperation du tableau de parametres V:
    Standard_Integer NV = tabGU[imax].NbPoints();
    for (i = 1; i <= NV; i++) {
      ParamV.Append(tabGU[imax].Parameter(i));
    }
    delete [] tabGU;

    imax = 1;
    Standard_Integer MaxU = 0;

    GCPnts_TangentialDeflection* tabGV = new GCPnts_TangentialDeflection[nbpointsV];

    for (i = 0; i <= nbpointsV-1; i++) {
      f = Max(umin, tabv[i].FirstParameter());
      l = Min(umax, tabv[i].LastParameter());
      GCPnts_TangentialDeflection thedeflection2(tabv[i], f, l, theangle, 0.7*defface, 2);
      tabGV[i] = thedeflection2;
      if (tabGV[i].NbPoints() > MaxU) {
	      MaxU = tabGV[i].NbPoints();
	      imax = i;
      }
    }
    
    // recuperation du tableau de parametres U:
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
    IsoV.Load(caro);

    Standard_Integer Lu = ParamU.Length(), Lv = ParamV.Length();
    
    for (i = 2; i < Lv; i++) {
      v = ParamV.Value(i);
      IsoV.Load(GeomAbs_IsoV, v);
      for (j = 2; j < Lu; j++) {
	      u = ParamU.Value(j);
	      if (classifier->Perform(gp_Pnt2d(u, v)) == TopAbs_IN)
        {
          // Record 3d point
	        P1 = IsoV.Value(u);
	        nbLocat++;
          Location3d.Bind(nbLocat, P1);
          // Record 2d point
	        p2d.SetCoord((u-umin)/deltaX, (v-vmin)/deltaY);
	        newV.Initialize(p2d.XY(), nbLocat, MeshDS_Free);
	        InternalV.Append(newV); 
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
Standard_Real BRepMesh_FastDiscretFace::Control (const Handle(BRepAdaptor_HSurface)& caro,
                                                 const Standard_Real                 defface,
                                                 BRepMesh_ListOfVertex&              InternalV,
                                                 TColStd_ListOfInteger&              badTriangles,
                                                 TColStd_ListOfInteger&              nulTriangles,
                                                 BRepMesh_Delaun&                    trigu,
                                                 const Standard_Boolean              isfirst)
{
  //IMPORTANT: Constants used in calculations
  const Standard_Real MinimalArea2d = 1.e-9;
  const Standard_Real MinimalSqLength3d = 1.e-12;
  const Standard_Real aDef2 = defface*defface;

  // Define the number of iterations
  Standard_Integer myNbIterations = 11;
  const Standard_Integer nbPasses = (isfirst? 1 : myNbIterations);

  // Initialize stop condition
  Standard_Boolean allDegenerated = Standard_False;
  Standard_Integer nbInserted = 1;

  // Create map of links to skip already processed
  Standard_Integer nbtriangles;

  nbtriangles = structure->ElemOfDomain().Extent();
  if (nbtriangles <= 0) return -1.0;
  BRepMesh_MapOfCouple theCouples(3*nbtriangles);

  gp_XY mi2d;
  gp_XYZ vecEd1, vecEd2, vecEd3;
  gp_Pnt pDef;
  Standard_Real dv = 0, defl = 0, maxdef = -1;
  Standard_Integer pass = 1, nf = 0, nl = 0;
  Standard_Integer v1, v2, v3, e1, e2, e3;
  Standard_Boolean o1, o2, o3;
  Standard_Boolean m1, m2, m3;
  BRepMesh_Vertex InsVertex;
  Standard_Boolean caninsert;

  Standard_Real sqdefface = defface * defface;
  Standard_Real ddu = caro->UResolution(defface);
  Standard_Real ddv = caro->VResolution(defface);

  GeomAbs_SurfaceType thetype = caro->GetType();
  Handle(Geom_Surface) BSpl;
  Standard_Boolean isSpline = Standard_False;
  if (thetype == GeomAbs_BezierSurface || thetype == GeomAbs_BSplineSurface)
  {
    isSpline = Standard_True;
    if (thetype == GeomAbs_BezierSurface) 
      BSpl = caro->Bezier();
    else 
      BSpl = caro->BSpline();
  }
  
  NCollection_DataMap<Standard_Integer,gp_Dir> aNorMap;
  NCollection_DataMap<Standard_Integer,Standard_Integer> aStatMap;

  // Perform refinement passes
  for (; pass <= nbPasses && nbInserted && !allDegenerated; pass++)
  {
    InternalV.Clear();
    badTriangles.Clear();
    
    // Reset stop condition
    allDegenerated = Standard_True;
    nbInserted = 0;
    maxdef = -1.0;

    // Do not insert nodes in last pass in non-SharedMode
    caninsert = (WithShare || pass < nbPasses);

    // Read mesh size
    nbtriangles = structure->ElemOfDomain().Extent();
    if (nbtriangles <= 0) break;

    // Iterate on current triangles
    MeshDS_MapOfInteger::Iterator triDom;
    const MeshDS_MapOfInteger& TriMap = structure->ElemOfDomain();
    triDom.Initialize(TriMap);
    Standard_Integer aNbPnt = 0;
    Standard_Real umin = myattrib->GetUMin();
    Standard_Real vmin = myattrib->GetVMin();
    Standard_Real deltaX = myattrib->GetDeltaX();
    Standard_Real deltaY = myattrib->GetDeltaY();
    for (; triDom.More(); triDom.Next())
    {
      Standard_Integer TriId = triDom.Key();
      const BRepMesh_Triangle& curTri=Triangle(TriId);
      if (curTri.Movability()==MeshDS_Deleted) continue;
      Standard_Boolean o1, o2, o3;
      Standard_Integer v1 = 0, v2 = 0, v3 = 0, e1 = 0, e2 = 0, e3 = 0;
      curTri.Edges(e1, e2, e3, o1, o2, o3);
      
      const BRepMesh_Edge& edg1=Edge(e1);
      const BRepMesh_Edge& edg2=Edge(e2);
      const BRepMesh_Edge& edg3=Edge(e3);
      
      Standard_Boolean m1 = (edg1.Movability() == MeshDS_Frontier);
      Standard_Boolean m2 = (edg2.Movability() == MeshDS_Frontier);
      Standard_Boolean m3 = (edg3.Movability() == MeshDS_Frontier);
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

      const gp_XYZ& p1=Location3d(vert1.Location3d()).Coord();
      const gp_XYZ& p2=Location3d(vert2.Location3d()).Coord();
      const gp_XYZ& p3=Location3d(vert3.Location3d()).Coord();

      vecEd1 = p2 - p1;
      vecEd2 = p3 - p2;
      vecEd3 = p1 - p3;

      // Check for degenerated triangle
      if (vecEd1.SquareModulus() < MinimalSqLength3d ||
          vecEd2.SquareModulus() < MinimalSqLength3d ||
          vecEd3.SquareModulus() < MinimalSqLength3d) 
      {
        nulTriangles.Append(TriId);
        continue;
      }

      allDegenerated = Standard_False;

      gp_XY xy1(vert1.Coord().X()*deltaX+umin,vert1.Coord().Y()*deltaY+vmin);
      gp_XY xy2(vert2.Coord().X()*deltaX+umin,vert2.Coord().Y()*deltaY+vmin);
      gp_XY xy3(vert3.Coord().X()*deltaX+umin,vert3.Coord().Y()*deltaY+vmin);

      // Check triangle area in 2d
      if (Abs((xy2-xy1)^(xy3-xy1)) < MinimalArea2d)
      {
       	nulTriangles.Append(TriId);
        continue;
      }

      // Check triangle normal
      gp_XYZ normal(vecEd1^vecEd2);
      dv = normal.Modulus();
      if (dv < Precision::Confusion())
      {
       	nulTriangles.Append(TriId);
        continue;
      }
      normal /= dv;

      // Check deflection on triangle
      mi2d = (xy1+xy2+xy3)/3.0;
      caro->D0(mi2d.X(), mi2d.Y(), pDef);
      defl = pDef.SquareDistance((p1+p2+p3)/3.);
      if (defl > maxdef) maxdef = defl;
      if (defl > sqdefface)
      {
        if (isfirst) break;
        if (caninsert)
        {
          // Record new vertex
          aNbPnt++;
          nbLocat++;
          Location3d.Bind(nbLocat,pDef);
          mi2d.SetCoord((mi2d.X()-umin)/deltaX,(mi2d.Y()-vmin)/deltaY);
          InsVertex.Initialize(mi2d,nbLocat,MeshDS_Free);
          InternalV.Append(InsVertex);
        }
        badTriangles.Append(TriId);
      }
      
      if (!m2) // Not a boundary
      {
        // Check if this link was already processed
        if (v2 < v3) { nf = v2; nl = v3; } else { nf = v3; nl = v2; }
        if (theCouples.Add(BRepMesh_Couple(nf,nl)))
        {
          // Check deflection on edge 1
          mi2d = (xy2+xy3)*0.5;
          caro->D0(mi2d.X(), mi2d.Y(), pDef);
          defl = pDef.SquareDistance((p2+p3)/2.);
          if (defl > maxdef) maxdef = defl;
          if (defl > sqdefface)
          {
            if (isfirst) break;
            if (caninsert)
            {
              // Record new vertex
              aNbPnt++;              
              nbLocat++;
              Location3d.Bind(nbLocat,pDef);
              mi2d.SetCoord((mi2d.X()-umin)/deltaX,(mi2d.Y()-vmin)/deltaY);
              InsVertex.Initialize(mi2d,nbLocat,MeshDS_Free);
              InternalV.Append(InsVertex);
            }
            badTriangles.Append(TriId);
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
          caro->D0(mi2d.X(), mi2d.Y(), pDef);
          defl = pDef.SquareDistance((p1+p3)/2.);
          if (defl > maxdef) maxdef = defl;
          if (defl > sqdefface)
          {
            if (isfirst) break;
            if (caninsert)
            {
              // Record new vertex
              aNbPnt++;
              nbLocat++;
              Location3d.Bind(nbLocat,pDef);
              mi2d.SetCoord((mi2d.X()-umin)/deltaX,(mi2d.Y()-vmin)/deltaY);
              InsVertex.Initialize(mi2d,nbLocat,MeshDS_Free);
              InternalV.Append(InsVertex);
            }
            badTriangles.Append(TriId);
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
          caro->D0(mi2d.X(), mi2d.Y(), pDef);
          defl = pDef.SquareDistance((p1+p2)/2.);
          if (defl > maxdef) maxdef = defl;
          if (defl > sqdefface)
          {
            if (isfirst) break;
            if (caninsert)
            {
              // Record new vertex
              aNbPnt++;
              nbLocat++;
              Location3d.Bind(nbLocat,pDef);
              mi2d.SetCoord((mi2d.X()-umin)/deltaX,(mi2d.Y()-vmin)/deltaY);              
              InsVertex.Initialize(mi2d,nbLocat,MeshDS_Free);
              InternalV.Append(InsVertex);
            }
            badTriangles.Append(TriId);
          }
        }
      }
      
      //check normal
      if(isSpline && !BSpl.IsNull() && (badTriangles.IsEmpty() || badTriangles.Last() != TriId))      
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
          (anAngle1 > angle || anAngle2 > angle || anAngle3 > angle)) {
           if (isfirst) {
            maxdef = -1;
            break;
          }
          // Record new vertex
          aNbPnt++;
          nbLocat++;
          mi2d = (xy1+xy2+xy3)/3.;
          caro->D0(mi2d.X(), mi2d.Y(), pDef);
          Location3d.Bind(nbLocat,pDef);
          mi2d.SetCoord((mi2d.X()-umin)/deltaX,(mi2d.Y()-vmin)/deltaY);              
          InsVertex.Initialize(mi2d,nbLocat,MeshDS_Free);
          InternalV.Append(InsVertex);
          badTriangles.Append(TriId);
	      }   
      
        //In case if triangle is considerd as OK, we have to check three intermediate 
        //points to be sure that we free from wave effect. If it is OK triangle passed if not split in middle point
        if(badTriangles.IsEmpty() || badTriangles.Last() != TriId)
        {
          Bnd_Box2d aB2d;
          aB2d.Add(gp_Pnt2d(xy1)); aB2d.Add(gp_Pnt2d(xy2)); aB2d.Add(gp_Pnt2d(xy3));

          Standard_Real aXMin1, aXMax1, aYMin1, aYMax1;
          aB2d.Get(aXMin1, aYMin1, aXMax1, aYMax1);

          if(aXMax1 - aXMin1 < ddu && aYMax1 - aYMin1 < ddv)
            continue;
          
          mi2d = (xy1+xy2+xy3)/3.;
          gp_Pnt2d aP[3] = {mi2d+(xy1-mi2d)*2/3., mi2d+(xy2-mi2d)*2/3, mi2d+(xy3-mi2d)*2/3.};
          gp_Dir midDir(0,0,1);
          Standard_Integer aSt[4];
          aSt[0] = GeomLib::NormEstim(BSpl, gp_Pnt2d(mi2d), Precision::Confusion(), midDir);
          Standard_Integer i = 0;
          gp_Dir dir[3] = {gp_Dir(0,0,1), gp_Dir(0,0,1), gp_Dir(0,0,1)};
          Standard_Real anAngle[3];
          for(; i < 3; i++)
          {
            gp_Dir dir(0,0,1);
            aSt[i+1] = GeomLib::NormEstim(BSpl, aP[i], Precision::Confusion(), dir);
            anAngle[i] = dir.Angle(midDir);
          }
          
          caro->D0(mi2d.X(), mi2d.Y(), pDef);

          aB2d.Add(gp_Pnt2d(xy1)); aB2d.Add(gp_Pnt2d(xy2)); aB2d.Add(gp_Pnt2d(xy3));
          
          if(aSt[0] < 1 && aSt[1] < 1 && aSt[2] < 1 && aSt[3] < 1 &&
            (anAngle[0] > angle || anAngle[1] > angle || anAngle[2] > angle) &&
            (aXMax1 - aXMin1 > ddu || aYMax1 - aYMin1 > ddv))
          {
            if (isfirst) {
              maxdef = -1;
              break;
            }
            aNbPnt++;
            nbLocat++;
            caro->D0(mi2d.X(), mi2d.Y(), pDef);
            Location3d.Bind(nbLocat,pDef);
            mi2d.SetCoord((mi2d.X()-umin)/deltaX,(mi2d.Y()-vmin)/deltaY);              
            InsVertex.Initialize(mi2d,nbLocat,MeshDS_Free);
            InternalV.Append(InsVertex);            
            badTriangles.Append(TriId);
          }
        }
      }
    }
    
    if (!isfirst && InternalV.Extent() > 0) 
    {
      BRepMesh_Array1OfVertexOfDelaun verttab(1, InternalV.Extent());
      BRepMesh_ListIteratorOfListOfVertex itVer(InternalV);
      Standard_Integer ipn = 1;
      for (; itVer.More(); itVer.Next())
        verttab(ipn++) = itVer.Value();
        
      trigu.AddVertices(verttab);
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
void BRepMesh_FastDiscretFace::AddInShape(const TopoDS_Face&  face,
					  const Standard_Real defface)
{
//  gp_Pnt Pt;
  BRep_Builder B;
  TopLoc_Location loc = face.Location();
  Handle(Poly_Triangulation) TOld = BRep_Tool::Triangulation(face, loc);
  Handle(Poly_Triangulation) TNull;
  Handle(Poly_PolygonOnTriangulation) NullPoly;
  B.UpdateFace(face,TNull);

  try{
  MeshDS_MapOfInteger::Iterator it;

  Standard_Integer e1, e2, e3, nTri;
  Standard_Integer v1, v2, v3, iv1, iv2, iv3;
  Standard_Integer i, index;
  Standard_Boolean o1, o2, o3;
  TopAbs_Orientation orFace = face.Orientation();

  const MeshDS_MapOfInteger& TriMap = structure->ElemOfDomain();
  it.Initialize(TriMap);
    
  nTri = TriMap.Extent();

  if (nTri != 0) {
    
    Poly_Array1OfTriangle Tri(1, nTri);
    
    i = 1;
    
    for (; it.More(); it.Next()) {
      structure->GetElement(it.Key()).Edges(e1, e2, e3, o1, o2, o3);
      
      const BRepMesh_Edge& ve1=structure->GetLink(e1);
      const BRepMesh_Edge& ve2=structure->GetLink(e2);
      const BRepMesh_Edge& ve3=structure->GetLink(e3);
      
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
      
      iv1 = myvemap.FindIndex(v1);
      if (iv1 == 0) iv1 = myvemap.Add(v1);
      iv2 = myvemap.FindIndex(v2);
      if (iv2 == 0) iv2 = myvemap.Add(v2);
      iv3 = myvemap.FindIndex(v3);
      if (iv3 == 0) iv3 = myvemap.Add(v3);
      
      if (orFace == TopAbs_REVERSED) Tri(i++).Set(iv1, iv3, iv2);
      else Tri(i++).Set(iv1, iv2, iv3);
    }
    
    Standard_Integer nbVertices = myvemap.Extent();
    Handle(Poly_Triangulation) T = new Poly_Triangulation(nbVertices, nTri, Standard_True);
    Poly_Array1OfTriangle& Trian = T->ChangeTriangles();
    Trian = Tri;
    TColgp_Array1OfPnt&  Nodes = T->ChangeNodes();
    TColgp_Array1OfPnt2d& Nodes2d = T->ChangeUVNodes();
    
    for (i = 1; i <= nbVertices; i++) {
      index = myvemap.FindKey(i);
      Nodes(i) = Pnt(index);
      Nodes2d(i).SetXY(Vertex(index).Coord());
    }
    
    T->Deflection(defface);
    
    // stockage de la triangulation dans la BRep.
    BRep_Builder B1;
    //TopLoc_Location loc = face.Location();
    if (!loc.IsIdentity()) {
      gp_Trsf tr = loc.Transformation();
      tr.Invert();
      for (i = Nodes.Lower(); i <= Nodes.Upper(); i++) 
	      Nodes(i).Transform(tr);
    }
    B1.UpdateFace(face, T);

    // mise en place des polygones sur triangulation dans la face:
    BRepMesh_DataMapIteratorOfDataMapOfShapePairOfPolygon It(internaledges);

    for (; It.More(); It.Next()) {
      const BRepMesh_PairOfPolygon& pair = It.Value();
      const Handle(Poly_PolygonOnTriangulation)& NOD1 = pair.First();
      const Handle(Poly_PolygonOnTriangulation)& NOD2 = pair.Last();
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
   //   MESH_FAILURE(face);
 }
}


//=======================================================================
//function : Triangle
//purpose  : 
//=======================================================================

const BRepMesh_Triangle& BRepMesh_FastDiscretFace::Triangle
  (const Standard_Integer Index) const
{
  return structure->GetElement(Index);
}

//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

/*Standard_Integer BRepMesh_FastDiscretFace::NbEdges() const
{
  return structure->NbLinks();
}*/

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const BRepMesh_Edge& BRepMesh_FastDiscretFace::Edge(const Standard_Integer Index) const
{
  return structure->GetLink(Index);
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const BRepMesh_Vertex& BRepMesh_FastDiscretFace::Vertex
  (const Standard_Integer Index) const
{
  return structure->GetNode(Index);
}

//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

const gp_Pnt& BRepMesh_FastDiscretFace::Pnt(const Standard_Integer Index) const
{
  return Location3d(structure->GetNode(Index).Location3d());
}

//=======================================================================
//function : FindUV
//purpose  : 
//=======================================================================

gp_XY BRepMesh_FastDiscretFace::FindUV(const TopoDS_Vertex&   V,
				       const gp_Pnt2d&        XY, 
				       const Standard_Integer ip,
				       const Handle(BRepAdaptor_HSurface)& S,
				       const Standard_Real mindist) 
{
  gp_XY theUV;
  if (mylocation2d.IsBound(ip))
  {
    BRepMesh_ListOfXY& L = mylocation2d.ChangeFind(ip);
    theUV = L.First();
    if (L.Extent() != 1)
    {
      BRepMesh_ListIteratorOfListOfXY it(L);
      it.Next();
      Standard_Real dd, dmin = XY.Distance(gp_Pnt2d(theUV));
      for (; it.More(); it.Next())
      {
	dd = XY.Distance(gp_Pnt2d(it.Value()));
	if (dd < dmin)
        {
	  theUV = it.Value();
	  dmin = dd;
	}
      }
    }

    const Standard_Real tol = Min(2. * BRep_Tool::Tolerance(V), mindist);

    const Standard_Real Utol2d = .5 * (S->LastUParameter() - S->FirstUParameter());
    const Standard_Real Vtol2d = .5 * (S->LastVParameter() - S->FirstVParameter());

    const gp_Pnt p1 = S->Value(theUV.X(), theUV.Y());
    const gp_Pnt p2 = S->Value(XY.X(), XY.Y());

    if (Abs(theUV.X() - XY.X()) > Utol2d ||
        Abs(theUV.Y() - XY.Y()) > Vtol2d ||
	!p1.IsEqual(p2, tol))
    {
      theUV = XY.Coord();
      L.Append(theUV);
    }
  }
  else
  {
    theUV = XY.Coord();
    BRepMesh_ListOfXY L;
    L.Append(theUV);
    mylocation2d.Bind(ip, L);
  }
  return theUV;
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
  // On regarde dabord si il y des PointRepresentation (cas non Manifold)

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
//purpose  : method intended to addition internav vertices in triangulation.
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
  if (vertices.IsBound(theVert))
    indVert = vertices.Find(theVert);
  else
  {
    nbLocat++;
    Location3d.Bind(nbLocat, BRep_Tool::Pnt(theVert));
    indVert = nbLocat;
    vertices.Bind(theVert, indVert);
  }
  Standard_Real mindist = BRep_Tool::Tolerance(theVert);
  // gp_Pnt2d uvXY = BRep_Tool::Parameters(theVert,theFace);
  gp_XY anUV = FindUV(theVert, uvXY, indVert, thegFace, mindist);
  BRepMesh_Vertex vf(anUV, indVert, MeshDS_Fixed);
  Standard_Integer ivff = structure->AddNode(vf);
  Standard_Integer isvf = myvemap.FindIndex(ivff);
  if (isvf == 0) isvf = myvemap.Add(ivff);  
}
