// File:	BRepMesh_FastDiscret.cxx
// Created:	Tue Feb 27 16:39:53 1996
// Author:	Ekaterina SMIRNOVA
// Copyright: Open CASCADE SAS 2008

#include <BRepMesh_FastDiscret.ixx>

#include <BRepMesh_FastDiscretFace.hxx>
#include <BRepMesh_FaceAttribute.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_ClassifierPtr.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <BRepMesh_PairOfPolygon.hxx>
#include <BRepMesh_DataMapOfShapePairOfPolygon.hxx>
#include <BRepMesh_DataMapIteratorOfDataMapOfShapePairOfPolygon.hxx>
#include <GeomAbs_IsoType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <TopAbs.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Precision.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Connect.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Precision.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepTools.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <Geom2d_Curve.hxx>

#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <BRepMesh_ShapeTool.hxx>
#include <ElSLib.hxx>
#include <Geom_Surface.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <BRepMesh_IndexedMapOfVertex.hxx>
#include <Extrema_LocateExtPC.hxx>

#include <BRepMesh_ListOfXY.hxx>
#include <BRepMesh_ListIteratorOfListOfXY.hxx>

#include <TColStd_Array1OfInteger.hxx>
#include <BRepMesh_IDMapOfNodeOfDataStructureOfDelaun.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
//#include <TColStd_DataMapOfInteger.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <NCollection_IncAllocator.hxx>

#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_TVertex.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <SortTools_ShellSortOfReal.hxx>
#include <TCollection_CompareOfReal.hxx>

#include <TopTools_HArray1OfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <vector>

// NOTE: replaced by more correct check
// #if defined(WNT) || defined(LIN)
// #define HAVE_TBB 1
// #endif

// paralleling with Intel TBB
#ifdef HAVE_TBB
#include <tbb/parallel_for_each.h>
#endif

inline Standard_Real MaxFaceTol (const TopoDS_Face& theFace)
{
  Standard_Real T, TMax = BRep_Tool::Tolerance(theFace);
  TopExp_Explorer Ex;

  for (Ex.Init(theFace,TopAbs_EDGE); Ex.More(); Ex.Next())
  {
    T = BRep_Tool::Tolerance(TopoDS::Edge(Ex.Current()));
    if (T > TMax) TMax = T;
  }

  for (Ex.Init(theFace,TopAbs_VERTEX); Ex.More(); Ex.Next())
  {
    T = BRep_Tool::Tolerance(TopoDS::Vertex(Ex.Current()));
    if (T > TMax) TMax = T;
  }

  return TMax;
}


//=======================================================================
//function : BRepMesh_FastDiscret
//purpose  : 
//=======================================================================
BRepMesh_FastDiscret::BRepMesh_FastDiscret(const Standard_Real    defle,
                                           const Standard_Real    angl,
                                           const Bnd_Box&         Box,
                                           const Standard_Boolean ws,
                                           const Standard_Boolean inshape,
                                           const Standard_Boolean relative,
                                           const Standard_Boolean shapetrigu): 
  myangle(angl), mydeflection(defle),myWithShare(ws), nbLocat(0), 
  myrelative(relative), myshapetrigu(shapetrigu), myinshape(inshape),
  myInternalVerticesMode(Standard_True)
{
  myAllocator = new NCollection_IncAllocator(64000);
  if (relative)
  {
    Standard_Real TXmin, TYmin, TZmin, TXmax, TYmax, TZmax;
    Box.Get(TXmin, TYmin, TZmin, TXmax, TYmax, TZmax);
    mydtotale = TXmax-TXmin;
    const Standard_Real dy = TYmax-TYmin;
    const Standard_Real dz = TZmax-TZmin;
    if (dy > mydtotale) mydtotale = dy;
    if (dz > mydtotale) mydtotale = dz;
  }
}

//=======================================================================
//function : BRepMesh_FastDiscret
//purpose  : 
//=======================================================================

BRepMesh_FastDiscret::BRepMesh_FastDiscret(const Standard_Real    defle,
                                           const TopoDS_Shape&    shape,
                                           const Bnd_Box&         Box,
                                           const Standard_Real    angl,
                                           const Standard_Boolean ws,
                                           const Standard_Boolean inshape,
                                           const Standard_Boolean relative,
                                           const Standard_Boolean shapetrigu): 
 myangle(angl), mydeflection(defle),myWithShare(ws), nbLocat(0), 
 myrelative(relative), myshapetrigu(shapetrigu), myinshape(inshape),
 myInternalVerticesMode(Standard_True)
{
  myAllocator = new NCollection_IncAllocator(64000);
  if (relative)
  {
    Standard_Real TXmin, TYmin, TZmin, TXmax, TYmax, TZmax;
    Box.Get(TXmin, TYmin, TZmin, TXmax, TYmax, TZmax);
    mydtotale = TXmax-TXmin;
    const Standard_Real dy = TYmax-TYmin;
    const Standard_Real dz = TZmax-TZmin;
    if (dy > mydtotale) mydtotale = dy;
    if (dz > mydtotale) mydtotale = dz;
  }
  Perform(shape);
}

//=======================================================================
//function : Perform(shape)
//purpose  : 
//=======================================================================

void BRepMesh_FastDiscret::Perform(const TopoDS_Shape& shape)
{
  std::vector<TopoDS_Face> aFaces;
  for (TopExp_Explorer ex(shape, TopAbs_FACE); ex.More(); ex.Next()) {
    TopoDS_Face aF = TopoDS::Face(ex.Current());
    Add(aF);
    aFaces.push_back(aF);
  }
  
  // mesh faces in parallel threads using TBB
#ifdef HAVE_TBB
  if (Standard::IsReentrant())
    tbb::parallel_for_each (aFaces.begin(), aFaces.end(), *this);
  else
#endif
  for (std::vector<TopoDS_Face>::iterator it(aFaces.begin()); it != aFaces.end(); it++)
    Process (*it);
}


//=======================================================================
//function : Process
//purpose  : 
//=======================================================================

void BRepMesh_FastDiscret::Process(const TopoDS_Face& theFace) const
{
  //cout << "START face " << theFace.TShape().operator->() << endl << flush;
  Handle(BRepMesh_FaceAttribute) fattribute;
  if ( GetFaceAttribute (theFace, fattribute) ) 
  {
    BRepMesh_FastDiscretFace aTool (GetAngle(), WithShare(), InShape(), ShapeTrigu());
    aTool.Add (theFace, fattribute, GetMapOfDefEdge());
  }
  //cout << "END   face " << theFace.TShape().operator->() << endl << flush;
}

//=======================================================================
//function : Add(face)
//purpose  : 
//=======================================================================

#define MESH_FAILURE(theface)        \
{                                    \
  myfacestate = BRepMesh_Failure;    \
  mynottriangulated.Append(theface); \
  return;                            \
}

void BRepMesh_FastDiscret::Add(const TopoDS_Face& theface)
{
#ifndef DEB_MESH
  try
  {
    OCC_CATCH_SIGNALS
#endif
  TopoDS_Face face = theface;
  BRepTools::Update(face);
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
  Handle(Poly_Triangulation) T;
  TopLoc_Location loc;

  if (!myWithShare) {          
    vertices.Clear();
    edges.Clear();
  }

  myvemap.Clear();
  mylocation2d.Clear();
  internaledges.Clear();

  Standard_Integer i;
  i = 1;

  Standard_Real defedge, defface;
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, dx, dy, dz;
  Standard_Integer nbEdge = 0;
  Standard_Real savangle = myangle;
  Standard_Real cdef;
  Standard_Real maxdef = 2.* MaxFaceTol(theface);
  defface = 0.;

  if (!myrelative) defface = Max(mydeflection, maxdef);

  TColStd_SequenceOfReal aFSeq, aLSeq;
  TColGeom2d_SequenceOfCurve aCSeq;
  TopTools_SequenceOfShape aShSeq;

  TopoDS_Iterator exW(face);

  for (; exW.More(); exW.Next()) {
    const TopoDS_Shape& aWire = exW.Value();
    if (aWire.ShapeType() != TopAbs_WIRE)
      continue;
    TopoDS_Iterator ex(aWire);
    for(; ex.More(); ex.Next()) {
      const TopoDS_Edge& edge = TopoDS::Edge(ex.Value());
      nbEdge++;
      if (!mapdefle.IsBound(edge)) {
        if (myrelative) {
          if (edges.IsBound(edge)) {
	    const BRepMesh_PairOfPolygon& pair = edges.Find(edge);
	    const Handle(Poly_PolygonOnTriangulation)& P = pair.First();
	    defedge = P->Deflection();
	  }
          else {
            Bnd_Box B;
            BRepBndLib::Add(edge, B);
            B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
            dx = aXmax-aXmin;
            dy = aYmax-aYmin;
            dz = aZmax-aZmin;
            defedge = dx;
            if (defedge < dy) defedge = dy;
            if (defedge < dz) defedge = dz;
            // adjusted in relation to the total size:
            cdef = mydtotale/(2*defedge);
            if (cdef < 0.5) cdef = 0.5;
            if (cdef > 2.) cdef = 2.;
            defedge = cdef * defedge * mydeflection;
            myangle = savangle * cdef;
          }
          defface = defface + defedge;
 	  defface = Max(maxdef, defface);
        }
        else defedge = mydeflection;
    
        defedge = Max(maxdef, defedge);
        defedge = Max(1.e-05 , defedge);
        mapdefle.Bind(edge, defedge);
      }
      else{
        defedge = mapdefle(edge);
        if (myrelative) {defface = defface + defedge; defface = Max(maxdef, defface);}
      }
      Standard_Real f1,l1;
      Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(edge, face, f1, l1);
      if (C.IsNull()) continue;

      aFSeq.Append(f1);
      aLSeq.Append(l1);
      aCSeq.Append(C);
      aShSeq.Append(edge);
      Add(edge, face, gFace, C, defedge, f1, l1);
      myangle = savangle;
    }
  }

  if (nbEdge == 0 || myvemap.Extent() < 3)
  {
      MESH_FAILURE(theface);
  }

  if (myrelative ) defface = defface / nbEdge;
  else             defface = mydeflection;

  if (myWithShare) defface = Max(maxdef, defface);
  
  T = BRep_Tool::Triangulation(face, loc);

  if (!myshapetrigu || T.IsNull()) {
    
    Standard_Real xCur, yCur;
    Standard_Real maxX, minX, maxY, minY;
    minX=minY=1.e100;
    maxX=maxY=-1.e100;
    
    Standard_Integer ipn = 0;
    Standard_Integer i1 =1;
    for (i1 = 1; i1 <= myvemap.Extent(); i1++) {
      const BRepMesh_Vertex& aV = structure->GetNode(myvemap.FindKey(i1));
      ipn++;
      xCur=aV.Coord().X();
      yCur=aV.Coord().Y();
      minX=Min(xCur, minX);
      maxX=Max(xCur, maxX);
      minY=Min(yCur, minY);
      maxY=Max(yCur, maxY);
    }
    Standard_Real myumin = minX;
    Standard_Real myumax = maxX;
    Standard_Real myvmin = minY;
    Standard_Real myvmax = maxY;
    
    const Standard_Real umin = BS.FirstUParameter();
    const Standard_Real umax = BS.LastUParameter();
    const Standard_Real vmin = BS.FirstVParameter();
    const Standard_Real vmax = BS.LastVParameter();

    if (myumin < umin || myumax > umax)
    {
      if (BS.IsUPeriodic())
      {
        if ((myumax - myumin) > (umax - umin))
        {
          myumax = myumin + (umax - umin);
        }
      }
      else
      {
        if (umin > myumin) myumin = umin;
        if (umax < myumax) myumax = umax;
      }
    }

    if (myvmin < vmin || myvmax > vmax)
    {
      if (BS.IsVPeriodic())
      {
        if ((myvmax - myvmin) > (vmax - vmin))
        {
          myvmax = myvmin + (vmax - vmin);
        }
      }
      else
      {
        if (vmin > myvmin) myvmin = vmin;
        if (vmax < myvmax) myvmax = vmax;
      }
    }

    // fast verification of the validity of calculated limits. If wrong, 
    // sure a problem of pcurve.
    if (thetype == GeomAbs_BezierSurface &&
        (myumin < -0.5 || myumax > 1.5 || myvmin < -0.5 || myvmax > 1.5))
    {
      MESH_FAILURE(theface);
    }
 
    //define parameters for correct parametrics
    
    Standard_Real deltaX = 1.0;
    Standard_Real deltaY = 1.0;
    Standard_Integer nbVertices = myvemap.Extent();
    const Standard_Real tolclass = Precision::PConfusion(); //0.03*Max(myumax-myumin, myvmax-myvmin);
    
    BRepMesh_ClassifierPtr classifier ( 
      new BRepMesh_Classifier(face, tolclass,	internaledges, myvemap, 
                              structure, myumin, myumax, myvmin, myvmax) );   
  
    myfacestate = classifier->State();
    if (myfacestate == BRepMesh_SelfIntersectingWire)
    {
      Standard_Integer nbmaill = 0;
      Standard_Real eps = Precision::Confusion();
      while (nbmaill < 5 && myfacestate != BRepMesh_ReMesh)
      {
        nbmaill++;
        
        //clear the structure of links
        structure.Nullify();
        structure = new BRepMesh_DataStructureOfDelaun(anAlloc);
        
        myvemap.Clear();
        mylocation2d.Clear();
        internaledges.Clear();

        Standard_Integer j1;
        for(j1 = 1; j1 <= aShSeq.Length(); j1++)
        {
          const TopoDS_Edge& edge = TopoDS::Edge(aShSeq.Value(j1));
          if (edges.IsBound(edge))
          {
            edges.UnBind(edge);
            internaledges.UnBind(edge);
          }
        }
        
        
        for( j1 = 1; j1 <= aShSeq.Length(); j1++)
        {
          const TopoDS_Edge& edge = TopoDS::Edge(aShSeq.Value(j1));
          defedge = mapdefle(edge) / 3.;
          defedge = Max(defedge, eps);
          mapdefle.Bind(edge, defedge);
          const Handle(Geom2d_Curve)& C = aCSeq.Value(j1);
          Add(edge, face, gFace, C, defedge, aFSeq.Value(j1), aLSeq.Value(j1));
        }
	  
        classifier.Nullify();

        classifier = new BRepMesh_Classifier(face, tolclass, internaledges, myvemap,
                                             structure, myumin, myumax, myvmin, myvmax);

        if (classifier->State() == BRepMesh_NoError)
        {
          myfacestate = BRepMesh_ReMesh;
	}
        nbVertices = myvemap.Extent();
      }
    }
    
    if (myfacestate != BRepMesh_NoError && myfacestate != BRepMesh_ReMesh)
    {
      mynottriangulated.Append(face);
      classifier.Nullify();
      return;
    }
    
    //??
    //add internal vertices after self-intersection check
    /*if(myInternalVerticesMode) {
      for(TopExp_Explorer ex(face,TopAbs_VERTEX ,TopAbs_EDGE); ex.More(); ex.Next()) {
        const TopoDS_Vertex& aVert = TopoDS::Vertex(ex.Current());
        Add(aVert,face,gFace);
      }
      nbVertices = myvemap.Extent();
    }*/
    

    
    // try to find the real length:
    // akm (bug OCC16) : We must calculate these measures in non-singular
    //     parts of face. Let's try to compute average value of three
    //     (umin, (umin+umax)/2, umax), and respectively for v.
    //                 vvvvv
    Standard_Real longu = 0.0, longv = 0.0; //, last , first;
    gp_Pnt P11, P12, P21, P22, P31, P32;
    
    Standard_Real du = (myumax-myumin)/20;
    Standard_Real dv = (myvmax-myvmin)/20;
    Standard_Real dfuave=(myumin+myumax)/2, dfucur;
    Standard_Real dfvave=(myvmin+myvmax)/2, dfvcur;
    // U loop
    BS.D0 (myumin, myvmin, P11);
    BS.D0 (myumin, dfvave, P21);
    BS.D0 (myumin, myvmax, P31);
    for (i1=0, dfucur=myumin; i1 <= 20; i1++, dfucur+=du)  {
      BS.D0 (dfucur, myvmin, P12);
      BS.D0 (dfucur, dfvave, P22);
      BS.D0 (dfucur, myvmax, P32);
      longu += ( P11.Distance(P12) + P21.Distance(P22) + P31.Distance(P32) );
      P11 = P12;
      P21 = P22;
      P31 = P32;
    }
    // V loop
    BS.D0(myumin, myvmin, P11);
    BS.D0(dfuave, myvmin, P21);
    BS.D0(myumax, myvmin, P31);
    for (i1=0, dfvcur=myvmin; i1 <= 20; i1++, dfvcur+=dv) {
      BS.D0 (myumin, dfvcur, P12);
      BS.D0 (dfuave, dfvcur, P22);
      BS.D0 (myumax, dfvcur, P32);
      longv += ( P11.Distance(P12) + P21.Distance(P22) + P31.Distance(P32) );
      P11 = P12;
      P21 = P22;
      P31 = P32;
    }
    longu /= 3.;
    longv /= 3.;
    // akm (bug OCC16) ^^^^^
    
    if (longu <= 1.e-16 || longv <= 1.e-16) {
      //yes, it is seen!!!
#ifdef DEB_MESH_CHRONO
      chMaillEdges.Stop();
      MESH_CHRONO_TSTOP(thetype);
#endif
      MESH_FAILURE(theface);
    }


    if (thetype == GeomAbs_Torus)  {
      gp_Torus Tor = BS.Torus();
      Standard_Real r = Tor.MinorRadius(), R = Tor.MajorRadius();
      Standard_Real Du, Dv;//, pasu, pasv;
      
      Dv = Max(1.0e0 - (defface/r),0.0e0) ;
      Standard_Real oldDv = 2.0 * ACos (Dv);
      oldDv = Min(oldDv, myangle);
      Dv  =  0.9*oldDv; //TWOTHIRD * oldDv;
      Dv = oldDv;
      
      Standard_Integer nbV = Max((Standard_Integer)((myvmax-myvmin)/Dv), 2);
      Dv = (myvmax-myvmin)/(nbV+1);
      
      Standard_Real ru = R + r;
      if (ru > 1.e-16) {
        Du = Max(1.0e0 - (defface/ru),0.0e0);
        Du  = (2.0 * ACos (Du));
        Du = Min(Du, myangle);
        Standard_Real aa = sqrt(Du*Du + oldDv*oldDv);
        if(aa < gp::Resolution())
          return; 

        Du = Du * Min(oldDv, Du) / aa;
      }
      else Du = Dv;     
	
      Standard_Integer nbU = Max((Standard_Integer)((myumax-myumin)/Du), 2);
      nbU = Max(nbU, (Standard_Integer)(nbV*(myumax-myumin)*R/((myvmax-myvmin)*r)/5.));
      
      Du = (myumax-myumin)/(nbU+1);
      //-- DeltaX and DeltaY are chosen so that to avoid "jumping" 
      //-- of points on the grid
      deltaX = Du;
      deltaY = Dv;
    }
    else if (thetype == GeomAbs_Cylinder) {
      /*Standard_Real aMax = Max(myumax,myvmax);
      deltaX = 0.01;  //1./aMax; //0.01;
      deltaY = 1.0;*/
      gp_Cylinder Cyl = BS.Cylinder();
      Standard_Real R = Cyl.Radius();
      // Calculate parameters for iteration in U direction
      Standard_Real Du = 1.0 - (defface/R);
      if (Du < 0.0) Du = 0.0;
      Du = 2.0 * ACos (Du);
      if (Du > GetAngle()) Du = GetAngle();
      deltaX = Du/longv;
      deltaY = 1.;
    }
    else {
      deltaX = (myumax-myumin)/longu;
      deltaY = (myvmax-myvmin)/longv;
    }    

    if(!mapattrib.IsBound(theface))
    {
      Handle(BRepMesh_FaceAttribute) aFA = new BRepMesh_FaceAttribute();
      aFA->GetDefFace() = defface;
      aFA->GetUMax() = myumax;
      aFA->GetVMax() = myvmax;
      aFA->GetUMin() = myumin;
      aFA->GetVMin() = myvmin;
      aFA->GetDeltaX() = deltaX;
      aFA->GetDeltaY() = deltaY;
      aFA->GetMinX() = minX;
      aFA->GetMinY() = minY;
      aFA->GetClassifier() = classifier;
      mapattrib.Bind(theface, aFA);
    }

    //Standard_Integer nbVertices = myvemap.Extent();
    T = new Poly_Triangulation(nbVertices, 1, Standard_True);
    TColgp_Array1OfPnt&  Nodes = T->ChangeNodes();
    TColgp_Array1OfPnt2d& Nodes2d = T->ChangeUVNodes();
    
    Standard_Integer index;
    for (i = 1; i <= nbVertices; i++) {
      index = myvemap.FindKey(i);
      Nodes(i) = Pnt(index);
      Nodes2d(i).SetXY(Vertex(index).Coord());
    }

    // storage of triangulation in the BRep.
    //TopLoc_Location loc = face.Location();
    if (!loc.IsIdentity()) {
      gp_Trsf tr = loc.Transformation();
      tr.Invert();
      for (i = Nodes.Lower(); i <= Nodes.Upper(); i++) 
	      Nodes(i).Transform(tr);
    }

    BRep_Builder B;
    B.UpdateFace(face, T);

    BRepMesh_DataMapIteratorOfDataMapOfShapePairOfPolygon It(internaledges);
    for (; It.More(); It.Next()) {
      const BRepMesh_PairOfPolygon& pair = It.Value();
      const Handle(Poly_PolygonOnTriangulation)& NOD1 = pair.First();
      const Handle(Poly_PolygonOnTriangulation)& NOD2 = pair.Last();
      if ( NOD1 == NOD2 )
	B.UpdateEdge(TopoDS::Edge(It.Key()), NOD1, T, loc);
      else
	B.UpdateEdge(TopoDS::Edge(It.Key()), NOD1, NOD2, T, loc);
    }
  }

#ifndef DEB_MESH
  }
  catch(Standard_Failure)
  {
    MESH_FAILURE(theface);
  }
#endif // DEB_MESH
  structure.Nullify();
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::Add(const TopoDS_Edge&                  edge, 
		           const TopoDS_Face&                  face, 
			   const Handle(BRepAdaptor_HSurface)& gFace,
                           const Handle(Geom2d_Curve)&         C2d,
			   const Standard_Real                 defedge,
                           const Standard_Real                 first,
                           const Standard_Real                 last)
{
  const TopAbs_Orientation orEdge = edge.Orientation();
  if (orEdge == TopAbs_EXTERNAL) return;

  const Standard_Boolean isEdgeBound = edges.IsBound(edge);

  if (!isEdgeBound)
  {
    if (Update(edge, face, C2d, defedge, first, last))
    {
      return;
    }
  }

  TopoDS_Vertex pBegin, pEnd;
  TopExp::Vertices(edge, pBegin, pEnd);
  if (pBegin.IsNull() || pEnd.IsNull())
  {
    return;
  }

  Standard_Real wFirst, wLast;
  BRep_Tool::Range(edge, face, wFirst, wLast);

  gp_Pnt2d uvFirst, uvLast;
  BRep_Tool::UVPoints(edge, face, uvFirst, uvLast);

  const Standard_Boolean sameUV = uvFirst.IsEqual(uvLast, Precision::PConfusion());

  //Control vertexes tolerances
  gp_Pnt pFirst = gFace->Value(uvFirst.X(), uvFirst.Y());
  gp_Pnt pLast = gFace->Value(uvLast.X(), uvLast.Y());

  Standard_Real mindist = 10. * Max(pFirst.Distance(BRep_Tool::Pnt(pBegin)), 
				    pLast.Distance(BRep_Tool::Pnt(pEnd)));

  if(mindist < BRep_Tool::Tolerance(pBegin) ||
     mindist < BRep_Tool::Tolerance(pEnd) ) mindist = defedge;

  // control of degenerated non-coded edges.

  Standard_Boolean degener = BRep_Tool::Degenerated(edge);

  if (!degener)
  {
    if (pBegin.IsSame(pEnd))
    {
      // calculation of the length of the edge in 3D
      Standard_Real longueur = 0.0;
      Standard_Real du = (wLast-wFirst)/20;
      gp_Pnt P1, P2;
      BRepAdaptor_Curve BC(edge);
      BC.D0(wFirst, P1);
      Standard_Real tolV = BRep_Tool::Tolerance(pBegin);
      Standard_Real tolV2 = 1.2*tolV;
      for (Standard_Integer l = 1; l <= 20; l++) {
	BC.D0(wFirst + l*du, P2);
	longueur += P1.Distance(P2);
	if (longueur > tolV2) break;
	P1 = P2;
      }

      if (longueur < tolV2)
      {
	degener = Standard_True;
      }
    }
  }

  // Correct UV points
  if (sameUV)
  {
    // 1. is it really sameUV without being degenerated
    gp_Pnt2d uvF, uvL;
    C2d->D0(first, uvF);
    C2d->D0(last, uvL);
    if (!uvFirst.IsEqual(uvF, Precision::PConfusion()))
    {
      uvFirst = uvF;
    }
    if (!uvLast.IsEqual(uvL, Precision::PConfusion()))
    {
      uvLast = uvL;
    }
  }

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
      nbLocat++;
      ipf = nbLocat;
      Location3d.Bind(ipf, BRep_Tool::Pnt(pBegin));
    }
    vertices.Bind(pBegin, ipf);
  }
  theUV = FindUV(pBegin, uvFirst, ipf, gFace, mindist);
  BRepMesh_Vertex vf(theUV, ipf, MeshDS_Frontier);
  Standard_Integer ivf = structure->AddNode(vf);

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
      }
      else
      {
        nbLocat++;
      	ipl = nbLocat;
        Location3d.Bind(ipl, BRep_Tool::Pnt(pEnd));
      }
      vertices.Bind(pEnd,ipl);
    }
  }
  theUV = FindUV(pEnd, uvLast, ipl, gFace, mindist);
  BRepMesh_Vertex vl(theUV, ipl, MeshDS_Frontier);
  Standard_Integer ivl= structure->AddNode(vl);

  Standard_Integer isvf = myvemap.FindIndex(ivf);
  if (isvf == 0) isvf = myvemap.Add(ivf);
  Standard_Integer isvl = myvemap.FindIndex(ivl);
  if (isvl == 0) isvl = myvemap.Add(ivl);
  
  Standard_Real otherdefedge = 0.5*defedge;
  gp_Pnt2d uv;
  BRepMesh_Vertex v2;
  gp_Pnt   P3d;

  Handle(Poly_PolygonOnTriangulation) P1;

  if (!isEdgeBound)
  {
    Handle(Poly_PolygonOnTriangulation) P2;

    if (degener)
    {
      // creation anew:
      TColStd_Array1OfInteger Nodes(1, 2), NodInStruct(1, 2);
      TColStd_Array1OfReal Param(1, 2);
      
      NodInStruct(1) = ipf;
      Nodes(1) = isvf;
      Param(1) = wFirst;
      
      NodInStruct(2) = ipl;
      Nodes(2) = isvl;
      Param(2) = wLast;

      P1 = new Poly_PolygonOnTriangulation(Nodes, Param);
      P2 = new Poly_PolygonOnTriangulation(NodInStruct, Param);
    }
    else
    {
      if (orEdge == TopAbs_INTERNAL) otherdefedge *= 0.5;
      
      BRepAdaptor_Curve cons;
      if (BRep_Tool::SameParameter(edge))
      {
	      cons.Initialize(edge);
      }
      else
      {
	      cons.Initialize(edge, face);
      }

      TopLoc_Location L;
      Standard_Integer nbpmin = 2;
      if (cons.GetType() == GeomAbs_Circle) nbpmin = 4; //OCC287
      BRepMesh_GeomTool GT(cons, wFirst, wLast, 0.5*myangle, otherdefedge, nbpmin);

      // PTv, chl/922/G9, Take into account internal vertices
      // it is necessary for internal edges, which does not split other edges, by their vertex
      TopoDS_Iterator exV(edge);
      for ( ; exV.More(); exV.Next() )
      {
	      TopoDS_Vertex aIntV = TopoDS::Vertex(exV.Value());
	      if ( aIntV.Orientation() == TopAbs_INTERNAL )
         GT.AddPoint(BRep_Tool::Pnt(aIntV),
	  	      BRep_Tool::Parameter(aIntV, edge),
		        Standard_True);
      }

      // Creation of polygons on triangulation:
      Standard_Real puv;
      Standard_Integer i;      
      Standard_Real aEdgeLen = 0., a2dEdgeLen = 0.;
      gp_Pnt P3dPrev;
      gp_Pnt2d uvPrev;
      GT.Value(cons, gFace, 1, puv, P3dPrev, uvPrev);
      for (i = 2; i <= GT.NbPoints(); i++)
      {
        // Record 3d point
	      GT.Value(cons, gFace, i, puv, P3d, uv);
        aEdgeLen += P3d.SquareDistance(P3dPrev);
        a2dEdgeLen += uv.SquareDistance(uvPrev);
        P3dPrev = P3d;
        uvPrev = uv;
      }

      Standard_Integer nbnodes = GT.NbPoints();
      Standard_Real aMinToler = Min(defedge, mydeflection);
      if(aEdgeLen < aMinToler*aMinToler)
      {
        //check 2D resolution
        Standard_Real ddu = gFace->UResolution(aMinToler);
        Standard_Real ddv = gFace->VResolution(aMinToler);
        Standard_Real aMin = Min(ddu, ddv);

        if(a2dEdgeLen < aMin*aMin)
          nbnodes = 2;
      }
      TColStd_Array1OfInteger Nodes(1, nbnodes);
      TColStd_Array1OfInteger NodInStruct(1, nbnodes);
      TColStd_Array1OfReal Param(1, nbnodes);

      // processing of the 1st point:
      Nodes(1) = isvf;
      NodInStruct(1) = ipf;
      Param(1) = wFirst;
      
      // last point:
      Nodes(nbnodes) = isvl;
      NodInStruct(nbnodes) = ipl;
      Param(nbnodes) = wLast;

      if(nbnodes > 2)
      {
        Standard_Integer iv2;
        for (i = 2; i < GT.NbPoints(); i++)
        {
          // Record 3d point
	        GT.Value(cons, gFace, i, puv, P3d, uv);
        	nbLocat++;
        	Location3d.Bind(nbLocat, P3d);
        	NodInStruct(i) = nbLocat;
          // Record 2d point
	        v2.Initialize(uv.Coord(), nbLocat, MeshDS_OnCurve);
          iv2=structure->AddNode(v2);
          
          Standard_Integer isv = myvemap.FindIndex(iv2);
          if (isv == 0) isv = myvemap.Add(iv2);
          Nodes(i) = isv;
          NodInStruct(i) = nbLocat;
          Param(i) = puv;

          if (orEdge == TopAbs_FORWARD)
            structure->AddLink(BRepMesh_Edge(ivf, iv2, MeshDS_Frontier));
          else if (orEdge == TopAbs_REVERSED)
            structure->AddLink(BRepMesh_Edge(iv2, ivf, MeshDS_Frontier));
          else if (orEdge == TopAbs_INTERNAL)
            structure->AddLink(BRepMesh_Edge(ivf, iv2, MeshDS_Fixed));
          ivf = iv2;
        }
      }

      P1 = new Poly_PolygonOnTriangulation(Nodes, Param);
      P2 = new Poly_PolygonOnTriangulation(NodInStruct, Param);
    }

    P2->Deflection(otherdefedge);
    BRepMesh_PairOfPolygon pair;
    pair.Append(P2);
    edges.Bind(edge, pair);

    if (ivf != ivl) {
      if (orEdge == TopAbs_FORWARD)
	      structure->AddLink(BRepMesh_Edge(ivf, ivl, MeshDS_Frontier));
      else if (orEdge == TopAbs_REVERSED)
	      structure->AddLink(BRepMesh_Edge(ivl, ivf, MeshDS_Frontier));
      else if (orEdge == TopAbs_INTERNAL)
        structure->AddLink(BRepMesh_Edge(ivf, ivl, MeshDS_Fixed));
    }
      

  }
  // If this Edge has been already checked and it is not degenerated, 
  // the points of the polygon calculated at the first check are retrieved :
  else
  {
    if (degener)
    {
      // Create 2d polygon for degenerated edge
      TColStd_Array1OfInteger Nodes(1, 2);
      TColStd_Array1OfReal PPar(1, 2);
      
      Nodes(1) = isvf;
      PPar(1) = wFirst;
      
      Nodes(2) = isvl;
      PPar(2) = wLast;
      
      P1 = new Poly_PolygonOnTriangulation(Nodes, PPar);
    }
    else
    {
      // retrieve the polygone:
      const BRepMesh_PairOfPolygon& pair = edges.Find(edge);
      const Handle(Poly_PolygonOnTriangulation)& P = pair.First();
      const TColStd_Array1OfInteger& NOD = P->Nodes();
      Handle(TColStd_HArray1OfReal) Par = P->Parameters();

      // creation anew:
      const Standard_Integer nbnodes = NOD.Length();
      TColStd_Array1OfInteger Nodes(1, nbnodes);
      TColStd_Array1OfReal PPar(1, nbnodes);
      Standard_Integer iv2;
      
      Nodes(1) = isvf;
      PPar(1) = wFirst;
      
      Nodes(nbnodes) = isvl;
      PPar(nbnodes) = wLast;
      
      if (nbnodes > 2)
      {
        Standard_Integer i;
	if (BRep_Tool::SameParameter(edge))
        {
	  for (i = 2; i < nbnodes; i++)
          {
            const Standard_Real puv = Par->Value(i);
	    C2d->D0(puv, uv);
	    v2.Initialize(uv.Coord(), NOD(i), MeshDS_OnCurve);
            iv2 = structure->AddNode(v2);
	
	    Standard_Integer isv = myvemap.FindIndex(iv2);
	    if (isv == 0) isv = myvemap.Add(iv2);
            Nodes(i) = isv;
	    PPar(i) = puv;
            
            if (orEdge==TopAbs_FORWARD)
	      structure->AddLink(BRepMesh_Edge(ivf, iv2, MeshDS_Frontier));
	    else if (orEdge == TopAbs_REVERSED)
	      structure->AddLink(BRepMesh_Edge(iv2, ivf, MeshDS_Frontier));
            else if (orEdge == TopAbs_INTERNAL)
	      structure->AddLink(BRepMesh_Edge(ivf, iv2, MeshDS_Fixed));
	  
            ivf = iv2;
	  }
	}
        else
        {
	  const Standard_Real wFold = Par->Value(Par->Lower());
	  const Standard_Real wLold = Par->Value(Par->Upper());

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
	  for (i = 2; i < nbnodes; i++)
          {
	    P3d = Location3d(NOD(i));
            // Record 2d point
	    wPrev = wCur;
	    wCur  = wFirst + wKoef*(Par->Value(i) - wFold);
            wCurFound += (wCur - wPrev);
	    pcos.Perform(P3d, wCurFound);
	    if (pcos.IsDone()) wCurFound = pcos.Point().Parameter();
	    C2d->D0(wCurFound, uv);
	    v2.Initialize(uv.Coord(), NOD(i), MeshDS_OnCurve);
            iv2 = structure->AddNode(v2);
            
	    Standard_Integer isv = myvemap.FindIndex(iv2);
	    if (isv == 0) isv = myvemap.Add(iv2); 
            Nodes(i) = isv;
	    PPar(i) = wCurFound;
            
            if (orEdge==TopAbs_FORWARD)
	      structure->AddLink(BRepMesh_Edge(ivf, iv2, MeshDS_Frontier));
            else if (orEdge == TopAbs_REVERSED)
	      structure->AddLink(BRepMesh_Edge(iv2, ivf, MeshDS_Frontier));
            else if (orEdge == TopAbs_INTERNAL)
	      structure->AddLink(BRepMesh_Edge(ivf, iv2, MeshDS_Fixed));
	  
            ivf = iv2;
	  }
	}
      }

      P1 = new Poly_PolygonOnTriangulation(Nodes, PPar);
    
    if (ivf != ivl) {
      if (orEdge == TopAbs_FORWARD) 
	structure->AddLink(BRepMesh_Edge(ivf, ivl, MeshDS_Frontier));
      else if (orEdge == TopAbs_REVERSED)
	structure->AddLink(BRepMesh_Edge(ivl, ivf, MeshDS_Frontier));
      else if (orEdge == TopAbs_INTERNAL)
	structure->AddLink(BRepMesh_Edge(ivf, ivl, MeshDS_Fixed));
      }
    }
  }

  P1->Deflection(defedge);
  if (internaledges.IsBound(edge))
  {
    BRepMesh_PairOfPolygon& pair = internaledges.ChangeFind(edge);
    if (orEdge == TopAbs_REVERSED)
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
}


//=======================================================================
//function : Update(edge)
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_FastDiscret::Update(const TopoDS_Edge&  edge,
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
	  // 1. is it really sameUV without being degenerated
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
//function : output
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_FastDiscret::NbTriangles() const
{
  return structure->NbElements();
}

//=======================================================================
//function : Triangle
//purpose  : 
//=======================================================================

const BRepMesh_Triangle& BRepMesh_FastDiscret::Triangle
  (const Standard_Integer Index) const
{
  return structure->GetElement(Index);
}

//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

Standard_Integer BRepMesh_FastDiscret::NbEdges() const
{
  return structure->NbLinks();
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const BRepMesh_Edge& BRepMesh_FastDiscret::Edge(const Standard_Integer Index) const
{
  return structure->GetLink(Index);
}

//=======================================================================
//function : NbVertices
//purpose  : 
//=======================================================================

Standard_Integer BRepMesh_FastDiscret::NbVertices() const
{
  return structure->NbNodes();
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const BRepMesh_Vertex& BRepMesh_FastDiscret::Vertex
  (const Standard_Integer Index) const
{
  return structure->GetNode(Index);
}

//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

const gp_Pnt& BRepMesh_FastDiscret::Pnt(const Standard_Integer Index) const
{
  return Location3d(structure->GetNode(Index).Location3d());
}

//=======================================================================
//function : VerticesOfDomain
//purpose  : 
//=======================================================================

void BRepMesh_FastDiscret::VerticesOfDomain(MeshDS_MapOfInteger&  Indices) const 
{ 
  Indices.Clear();
  
  // recuperate from the map of edges.
  const MeshDS_MapOfInteger& edmap = structure->LinkOfDomain();

  // iterator on edges.
  MeshDS_MapOfInteger::Iterator iter(edmap);
  
  Standard_Integer ind_edge;
  for (iter.Reset(); iter.More(); iter.Next()) {
    ind_edge = iter.Key();
    const BRepMesh_Edge& Ed = Edge(ind_edge);
    Indices.Add(Ed.FirstNode());
    Indices.Add(Ed.LastNode());
  }
}

//=======================================================================
//function : FindUV
//purpose  : 
//=======================================================================

gp_XY BRepMesh_FastDiscret::FindUV(const TopoDS_Vertex&   V,
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


BRepMesh_Status BRepMesh_FastDiscret::CurrentFaceStatus() const
{
  return myfacestate;
}

//=======================================================================
//function : GetFaceAttribute
//purpose  : 
//=======================================================================

Standard_Boolean BRepMesh_FastDiscret::GetFaceAttribute(const TopoDS_Face& theFace,
                                                        Handle(BRepMesh_FaceAttribute)& fattrib) const
{
  if(mapattrib.IsBound(theFace))
  {
    fattrib = mapattrib(theFace);
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : RemoveFaceAttribute
//purpose  : 
//=======================================================================

void BRepMesh_FastDiscret::RemoveFaceAttribute(const TopoDS_Face& theFace)
{
  if(mapattrib.IsBound(theFace))
    mapattrib.UnBind(theFace);
}
