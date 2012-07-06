// Created on: 1996-02-27
// Created by: Ekaterina SMIRNOVA
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BRepMesh_FastDiscret.ixx>

#include <BRepMesh_FastDiscretFace.hxx>
#include <BRepMesh_FaceAttribute.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_ClassifierPtr.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <BRepMesh_PairOfPolygon.hxx>
#include <BRepMesh_DataMapOfShapePairOfPolygon.hxx>
#include <BRepMesh_DataMapIteratorOfDataMapOfShapePairOfPolygon.hxx>
#include <Geom_Plane.hxx>
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

#ifdef HAVE_TBB
  // paralleling using Intel TBB
  #include <tbb/parallel_for_each.h>
#endif

#define UVDEFLECTION 1.e-05

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
BRepMesh_FastDiscret::BRepMesh_FastDiscret(const Standard_Real    theDefle,
                                           const Standard_Real    theAngl,
                                           const Bnd_Box&         theBox,
                                           const Standard_Boolean theWithShare,
                                           const Standard_Boolean theInshape,
                                           const Standard_Boolean theRelative,
                                           const Standard_Boolean theShapetrigu) :
  myAngle (theAngl),
  myDeflection (theDefle),
  myWithShare (theWithShare),
  myInParallel (Standard_False),
  myNbLocat (0),
  myRelative (theRelative),
  myShapetrigu (theShapetrigu), 
  myInshape (theInshape)
{
  myAllocator = new NCollection_IncAllocator(64000);
  if(myRelative)
    BoxMaxDimension(theBox, myDtotale);
}

//=======================================================================
//function : BRepMesh_FastDiscret
//purpose  : 
//=======================================================================

BRepMesh_FastDiscret::BRepMesh_FastDiscret(const Standard_Real    theDefle,
                                           const TopoDS_Shape&    theShape,
                                           const Bnd_Box&         theBox,
                                           const Standard_Real    theAngl,
                                           const Standard_Boolean theWithShare,
                                           const Standard_Boolean theInshape,
                                           const Standard_Boolean theRelative,
                                           const Standard_Boolean theShapetrigu): 
  myAngle (theAngl),
  myDeflection (theDefle),
  myWithShare (theWithShare),
  myInParallel (Standard_False),
  myNbLocat (0),
  myRelative (theRelative),
  myShapetrigu (theShapetrigu),
  myInshape (theInshape)
{
  myAllocator = new NCollection_IncAllocator(64000);
  if(myRelative)
    BoxMaxDimension(theBox, myDtotale);
  Perform(theShape);
}

//=======================================================================
//function : SetParallel
//purpose  :
//=======================================================================
void BRepMesh_FastDiscret::SetParallel (const Standard_Boolean theInParallel)
{
  myInParallel = theInParallel;
}

//=======================================================================
//function : IsParallel
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_FastDiscret::IsParallel() const
{
  return myInParallel;
}

//=======================================================================
//function : BoxMaxDimension
//purpose  : 
//=======================================================================

void BRepMesh_FastDiscret::BoxMaxDimension(const Bnd_Box& theBox, Standard_Real& theMaxDim)
{
  if(theBox.IsVoid())
    return;
  Standard_Real TXmin, TYmin, TZmin, TXmax, TYmax, TZmax;
  theBox.Get(TXmin, TYmin, TZmin, TXmax, TYmax, TZmax);
  theMaxDim = TXmax-TXmin;
  const Standard_Real dy = TYmax-TYmin;
  const Standard_Real dz = TZmax-TZmin;
  if (dy > theMaxDim) theMaxDim = dy;
  if (dz > theMaxDim) theMaxDim = dz;
}

//=======================================================================
//function : RelativeEdgeDeflection
//purpose  : 
//=======================================================================

Standard_Real BRepMesh_FastDiscret::RelativeEdgeDeflection(const TopoDS_Edge& theEdge,
	                                          						   const Standard_Real theDefle,
							                                             const Standard_Real theDTotale,
                                                           Standard_Real& theDefCoef)
{
  theDefCoef = 1.;
  Standard_Real defedge = theDefle;
  if(theEdge.IsNull())
    return defedge;

  Bnd_Box B;
  BRepBndLib::Add(theEdge, B);
  BoxMaxDimension(B, defedge);
            
  // adjusted in relation to the total size:
  theDefCoef = theDTotale/(2*defedge);
  if (theDefCoef < 0.5) theDefCoef = 0.5;
  if (theDefCoef > 2.) theDefCoef = 2.;
  defedge = theDefCoef * defedge * theDefle;

  return defedge;
}

//=======================================================================
//function : Perform(shape)
//purpose  : 
//=======================================================================

void BRepMesh_FastDiscret::Perform(const TopoDS_Shape& theShape)
{
  TopTools_IndexedDataMapOfShapeListOfShape anAncestors;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, anAncestors);
  std::vector<TopoDS_Face> aFaces;
  for (TopExp_Explorer ex(theShape, TopAbs_FACE); ex.More(); ex.Next()) {
    TopoDS_Face aF = TopoDS::Face(ex.Current());
    Add(aF, anAncestors);
    aFaces.push_back(aF);
  }

  if (myInParallel)
  {
  #ifdef HAVE_TBB
    CreateMutexesForSubShapes(theShape, TopAbs_EDGE);
    // mesh faces in parallel threads using TBB
    tbb::parallel_for_each (aFaces.begin(), aFaces.end(), *this);
  #else
    // alternative parallelization not yet available
    for (std::vector<TopoDS_Face>::iterator it(aFaces.begin()); it != aFaces.end(); it++)
      Process (*it);
  #endif
    RemoveAllMutexes();
  }
  else
  {
    for (std::vector<TopoDS_Face>::iterator it(aFaces.begin()); it != aFaces.end(); it++)
      Process (*it);
  }
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
    BRepMesh_FastDiscretFace aTool (GetAngle(), WithShare());
    aTool.Add (theFace, fattribute, GetMapOfDefEdge(), myMutexProvider);
  }
  //cout << "END   face " << theFace.TShape().operator->() << endl << flush;
}

//=======================================================================
//function : Add(face)
//purpose  : 
//=======================================================================

#define MESH_FAILURE(theface)        \
{                                    \
  myFacestate = BRepMesh_Failure;    \
  myNottriangulated.Append(theface); \
  return;                            \
}

void BRepMesh_FastDiscret::Add(const TopoDS_Face& theface,
                               const TopTools_IndexedDataMapOfShapeListOfShape& theAncestors)
{
#ifndef DEB_MESH
  try
  {
    OCC_CATCH_SIGNALS
#endif
  TopoDS_Face face = theface;
  BRepTools::Update(face);
  face.Orientation(TopAbs_FORWARD);
  myStructure.Nullify();
  Handle(NCollection_IncAllocator) anAlloc = Handle(NCollection_IncAllocator)::DownCast(myAllocator);
  anAlloc->Reset(Standard_False);  
  myStructure=new BRepMesh_DataStructureOfDelaun(anAlloc);

  Standard_Real aUmin, aVmin, aUmax, aVmax;
  BRepTools::UVBounds (theface, aUmin, aUmax, aVmin, aVmax);
  Standard_Real aTolU = (aUmax - aUmin) * UVDEFLECTION;
  Standard_Real aTolV = (aVmax - aVmin) * UVDEFLECTION;
  myStructure->Data().SetCellSize ( 14 * aTolU, 14 * aTolV );
  myStructure->Data().SetTolerance( aTolU, aTolV );

  BRepAdaptor_Surface  BS(face, Standard_False);
  Handle(BRepAdaptor_HSurface) gFace = new BRepAdaptor_HSurface(BS);
  
  GeomAbs_SurfaceType thetype;
  thetype = BS.GetType();

  gp_Pnt2d uvFirst, uvLast;

  Handle(Poly_Triangulation) T;
  TopLoc_Location loc;

  if (!myWithShare) {          
    myVertices.Clear();
    myEdges.Clear();
  }

  myVemap.Clear();
  myLocation2d.Clear();
  myInternaledges.Clear();

  Standard_Integer i;
  i = 1;

  Standard_Real defedge, defface;
  Standard_Integer nbEdge = 0;
  Standard_Real savangle = myAngle;
  Standard_Real cdef;
  Standard_Real maxdef = 2.* MaxFaceTol(theface);
  defface = 0.;

  if (!myRelative) defface = Max(myDeflection, maxdef);

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
      if (!myMapdefle.IsBound(edge)) {
        if (myRelative) {
          if (myEdges.IsBound(edge)) {
            const BRepMesh_PairOfPolygon& pair = myEdges.Find(edge);
            const Handle(Poly_PolygonOnTriangulation)& P = pair.First();
            defedge = P->Deflection();
          }
          else {
            defedge = RelativeEdgeDeflection(edge, myDeflection, myDtotale, cdef);
            myAngle = savangle * cdef;
          }
          defface = defface + defedge;
          defface = Max(maxdef, defface);
        }
        else defedge = myDeflection;
    
        defedge = Max(maxdef, defedge);
        defedge = Max(UVDEFLECTION , defedge);
        myMapdefle.Bind(edge, defedge);
      }
      else{
        defedge = myMapdefle(edge);
        if (myRelative) {defface = defface + defedge; defface = Max(maxdef, defface);}
      }
      Standard_Real f1,l1;
      Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(edge, face, f1, l1);
      if (C.IsNull()) continue;

      aFSeq.Append(f1);
      aLSeq.Append(l1);
      aCSeq.Append(C);
      aShSeq.Append(edge);
      Add(edge, face, gFace, C, theAncestors, defedge, f1, l1);
      myAngle = savangle;
    }
  }

  if (nbEdge == 0 || myVemap.Extent() < 3)
  {
      MESH_FAILURE(theface);
  }

  if (myRelative ) defface = defface / nbEdge;
  else             defface = myDeflection;

  if (myWithShare) defface = Max(maxdef, defface);
  
  T = BRep_Tool::Triangulation(face, loc);

  if (!myShapetrigu || T.IsNull()) {
    
    Standard_Real xCur, yCur;
    Standard_Real maxX, minX, maxY, minY;
    minX=minY=1.e100;
    maxX=maxY=-1.e100;
    
    Standard_Integer ipn = 0;
    Standard_Integer i1 =1;
    for (i1 = 1; i1 <= myVemap.Extent(); i1++) {
      const BRepMesh_Vertex& aV = myStructure->GetNode(myVemap.FindKey(i1));
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
    Standard_Integer nbVertices = myVemap.Extent();
    const Standard_Real tolclass = Precision::PConfusion(); //0.03*Max(myumax-myumin, myvmax-myvmin);
    
    BRepMesh_ClassifierPtr classifier ( 
      new BRepMesh_Classifier(face, tolclass,  myInternaledges, myVemap, 
                              myStructure, myumin, myumax, myvmin, myvmax) );   
  
    myFacestate = classifier->State();
    if (myFacestate == BRepMesh_SelfIntersectingWire)
    {
      Standard_Integer nbmaill = 0;
      Standard_Real eps = Precision::Confusion();
      while (nbmaill < 5 && myFacestate != BRepMesh_ReMesh)
      {
        nbmaill++;
        
        //clear the structure of links
        myStructure.Nullify();
        myStructure = new BRepMesh_DataStructureOfDelaun(anAlloc);
        
        myVemap.Clear();
        myLocation2d.Clear();
        myInternaledges.Clear();

        Standard_Integer j1;
        for(j1 = 1; j1 <= aShSeq.Length(); j1++)
        {
          const TopoDS_Edge& edge = TopoDS::Edge(aShSeq.Value(j1));
          if (myEdges.IsBound(edge))
          {
            myEdges.UnBind(edge);
            myInternaledges.UnBind(edge);
          }
        }
        
        
        for( j1 = 1; j1 <= aShSeq.Length(); j1++)
        {
          const TopoDS_Edge& edge = TopoDS::Edge(aShSeq.Value(j1));
          defedge = myMapdefle(edge) / 3.;
          defedge = Max(defedge, eps);
          myMapdefle.Bind(edge, defedge);
          const Handle(Geom2d_Curve)& C = aCSeq.Value(j1);
          Add(edge, face, gFace, C, theAncestors, defedge, aFSeq.Value(j1), aLSeq.Value(j1));
        }

        classifier.Nullify();

        classifier = new BRepMesh_Classifier(face, tolclass, myInternaledges, myVemap,
                                             myStructure, myumin, myumax, myvmin, myvmax);

        if (classifier->State() == BRepMesh_NoError)
        {
          myFacestate = BRepMesh_ReMesh;
        }
        nbVertices = myVemap.Extent();
      }
    }
    
    if (myFacestate != BRepMesh_NoError && myFacestate != BRepMesh_ReMesh)
    {
      myNottriangulated.Append(face);
      classifier.Nullify();
      return;
    }
    
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
    for (i1=1, dfucur=myumin+du; i1 <= 20; i1++, dfucur+=du)  {
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
    for (i1=1, dfvcur=myvmin+dv; i1 <= 20; i1++, dfvcur+=dv) {    
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
      //yes, it is seen!!
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
      oldDv = Min(oldDv, myAngle);
      Dv  =  0.9*oldDv; //TWOTHIRD * oldDv;
      Dv = oldDv;
      
      Standard_Integer nbV = Max((Standard_Integer)((myvmax-myvmin)/Dv), 2);
      Dv = (myvmax-myvmin)/(nbV+1);
      
      Standard_Real ru = R + r;
      if (ru > 1.e-16) {
        Du = Max(1.0e0 - (defface/ru),0.0e0);
        Du  = (2.0 * ACos (Du));
        Du = Min(Du, myAngle);
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

    if(!myMapattrib.IsBound(theface))
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
      myMapattrib.Bind(theface, aFA);
    }

    //Standard_Integer nbVertices = myVemap.Extent();
    T = new Poly_Triangulation(nbVertices, 1, Standard_True);
    TColgp_Array1OfPnt&  Nodes = T->ChangeNodes();
    TColgp_Array1OfPnt2d& Nodes2d = T->ChangeUVNodes();
    
    Standard_Integer index;
    for (i = 1; i <= nbVertices; i++) {
      index = myVemap.FindKey(i);
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

    BRepMesh_DataMapIteratorOfDataMapOfShapePairOfPolygon It(myInternaledges);
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
  myStructure.Nullify();
}

//=======================================================================
//function : splitSegment
//purpose  :
//=======================================================================
static void splitSegment( BRepMesh_GeomTool&    theGT, 
                          const Handle(Geom_Surface)& theSurf, 
                          const Handle(Geom2d_Curve)& theCurve2d,
                          const BRepAdaptor_Curve&    theBAC,
                          const Standard_Real         theSquareEDef,
                          const Standard_Real         theFirst,
                          const Standard_Real         theLast,
                          const Standard_Integer      theNbIter)
{
  //limit ineration depth
  if(theNbIter > 10)
    return;
  gp_Pnt2d uvf, uvl, uvm;
  gp_Pnt   P3dF, P3dL, midP3d, midP3dFromSurf;
  Standard_Real midpar;
  
  if(Abs(theLast - theFirst) < 2*Precision::PConfusion())
    return;

  theCurve2d->D0(theFirst, uvf);
  theCurve2d->D0(theLast, uvl);

  P3dF = theSurf->Value(uvf.X(), uvf.Y());
  P3dL = theSurf->Value(uvl.X(), uvl.Y());
  
  if(P3dF.SquareDistance(P3dL) < theSquareEDef)
    return;

  uvm = gp_Pnt2d((uvf.XY() + uvl.XY())*0.5);
  midP3dFromSurf = theSurf->Value(uvm.X(), uvm.Y());

  gp_XYZ aVec = P3dL.XYZ()-P3dF.XYZ();
  aVec.Normalize();

  gp_XYZ Vec1 = midP3dFromSurf.XYZ() - P3dF.XYZ();
  Standard_Real aModulus = Vec1.Dot(aVec);
  gp_XYZ aProj = aVec*aModulus;
  gp_XYZ aDist = Vec1 - aProj;
    
  if(aDist.SquareModulus() < theSquareEDef)
    return;

  midpar = (theFirst + theLast) * 0.5;
  theBAC.D0(midpar, midP3d);
  theGT.AddPoint(midP3d, midpar, Standard_False);

  splitSegment(theGT, theSurf, theCurve2d, theBAC, theSquareEDef, theFirst, midpar, theNbIter+1);
  splitSegment(theGT, theSurf, theCurve2d, theBAC, theSquareEDef, midpar, theLast,  theNbIter+1); 
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::Add( const TopoDS_Edge&                  theEdge, 
                                const TopoDS_Face&                  theFace, 
                                const Handle(BRepAdaptor_HSurface)& theGFace,
                                const Handle(Geom2d_Curve)&         theC2d,
                                const TopTools_IndexedDataMapOfShapeListOfShape& theAncestors,
                                const Standard_Real                 theDefEdge,
                                const Standard_Real                 theFirst,
                                const Standard_Real                 theLast)
{
  const TopAbs_Orientation orEdge = theEdge.Orientation();
  if (orEdge == TopAbs_EXTERNAL) return;

  const Standard_Boolean isEdgeBound = myEdges.IsBound(theEdge);

  if (!isEdgeBound)
  {
    if (Update(theEdge, theFace, theC2d, theDefEdge, theFirst, theLast))
    {
      return;
    }
  }

  TopoDS_Vertex pBegin, pEnd;
  TopExp::Vertices(theEdge, pBegin, pEnd);
  if (pBegin.IsNull() || pEnd.IsNull())
  {
    return;
  }

  Standard_Real wFirst, wLast;
  BRep_Tool::Range(theEdge, theFace, wFirst, wLast);

  gp_Pnt2d uvFirst, uvLast;
  BRep_Tool::UVPoints(theEdge, theFace, uvFirst, uvLast);

  const Standard_Boolean sameUV = uvFirst.IsEqual(uvLast, Precision::PConfusion());

  //Control vertexes tolerances
  gp_Pnt pFirst = theGFace->Value(uvFirst.X(), uvFirst.Y());
  gp_Pnt pLast  = theGFace->Value(uvLast.X(), uvLast.Y());

  Standard_Real mindist = 10. * Max(pFirst.Distance(BRep_Tool::Pnt(pBegin)), 
                                    pLast.Distance(BRep_Tool::Pnt(pEnd)));

  if(mindist < BRep_Tool::Tolerance(pBegin) ||
     mindist < BRep_Tool::Tolerance(pEnd) ) mindist = theDefEdge;

  // control of degenerated non-coded edges.

  Standard_Boolean degener = BRep_Tool::Degenerated(theEdge);

  if (!degener)
  {
    if (pBegin.IsSame(pEnd))
    {
      // calculation of the length of the edge in 3D
      Standard_Real longueur = 0.0;
      Standard_Real du = (wLast-wFirst)/20;
      gp_Pnt P1, P2;
      BRepAdaptor_Curve BC(theEdge);
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
    theC2d->D0(theFirst, uvF);
    theC2d->D0(theLast, uvL);
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
      myNbLocat++;
      ipf = myNbLocat;
      myLocation3d.Bind(ipf, BRep_Tool::Pnt(pBegin));
    }
    myVertices.Bind(pBegin, ipf);
  }
  theUV = BRepMesh_FastDiscretFace::FindUV(pBegin, uvFirst, ipf, theGFace, mindist, myLocation2d);
  BRepMesh_Vertex vf(theUV, ipf, BRepMesh_Frontier);
  Standard_Integer ivf = myStructure->AddNode(vf);

  // Process last vertex
  Standard_Integer ipl;
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
      }
      else
      {
        myNbLocat++;
        ipl = myNbLocat;
        myLocation3d.Bind(ipl, BRep_Tool::Pnt(pEnd));
      }
      myVertices.Bind(pEnd,ipl);
    }
  }
  theUV = BRepMesh_FastDiscretFace::FindUV(pEnd, uvLast, ipl, theGFace, mindist, myLocation2d);
  BRepMesh_Vertex vl(theUV, ipl, BRepMesh_Frontier);
  Standard_Integer ivl= myStructure->AddNode(vl);

  Standard_Integer isvf = myVemap.FindIndex(ivf);
  if (isvf == 0) isvf = myVemap.Add(ivf);
  Standard_Integer isvl = myVemap.FindIndex(ivl);
  if (isvl == 0) isvl = myVemap.Add(ivl);
  
  Standard_Real otherdefedge = 0.5*theDefEdge;
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
      Standard_Boolean isSameParam = BRep_Tool::SameParameter(theEdge);
      if (isSameParam)
      {
        cons.Initialize(theEdge);
      }
      else
      {
        cons.Initialize(theEdge, theFace);
      }

      TopLoc_Location L;
      Standard_Integer nbpmin = 2;
      if (cons.GetType() == GeomAbs_Circle) nbpmin = 4; //OCC287
      BRepMesh_GeomTool GT(cons, wFirst, wLast, 0.5*myAngle, otherdefedge, nbpmin);

      // PTv, chl/922/G9, Take into account internal vertices
      // it is necessary for internal edges, which do not split other edges, by their vertex
      TopoDS_Iterator exV(theEdge);
      for ( ; exV.More(); exV.Next() )
      {
        TopoDS_Vertex aIntV = TopoDS::Vertex(exV.Value());
        if ( aIntV.Orientation() == TopAbs_INTERNAL )
         GT.AddPoint(BRep_Tool::Pnt(aIntV),
                     BRep_Tool::Parameter(aIntV, theEdge),
                     Standard_True);
      }

      Standard_Integer i; 
      Standard_Integer nbnodes = GT.NbPoints();
      //Check deflection in 2d space for improvement of edge tesselation.
      if( isSameParam && nbnodes > 1)
      {
        Standard_Real aSquareEdgeDef = otherdefedge * otherdefedge;
        const TopTools_ListOfShape& lf = theAncestors.FindFromKey(theEdge);
        TopTools_ListIteratorOfListOfShape itl(lf);
        for (; itl.More(); itl.Next()) {
          const TopoDS_Face& aFace = TopoDS::Face (itl.Value());          

          TopLoc_Location aLoc;
          Standard_Real aF, aL;
          Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace, aLoc);
          const Handle(Standard_Type)& aType = aSurf->DynamicType();
          if(aType == STANDARD_TYPE(Geom_Plane))
            continue;
          Handle(Geom2d_Curve) aCurve2d = BRep_Tool::CurveOnSurface(theEdge, aFace, aF, aL);
          if(Abs(aF-wFirst)>Precision::PConfusion()||Abs(aL-wLast)>Precision::PConfusion())
            continue;
          
          gp_Pnt2d uvf;
          Standard_Real parf;
          nbnodes = GT.NbPoints();
          TColStd_Array1OfReal aParamArray(1, nbnodes);
          for (i = 1; i <= nbnodes; i++)
          {          
            GT.Value(cons, theGFace, i, parf, P3d, uvf);            
            aParamArray.SetValue(i, parf);
          }
          for (i = 1; i < nbnodes; i++)
          {
            splitSegment(GT, aSurf, aCurve2d, cons, aSquareEdgeDef, aParamArray(i), aParamArray(i+1), 1);
          }
        }
      }

      // Creation of polygons on triangulation:
      Standard_Real puv;
      nbnodes = GT.NbPoints();

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
          GT.Value(cons, theGFace, i, puv, P3d, uv);
          myNbLocat++;
          myLocation3d.Bind(myNbLocat, P3d);
          NodInStruct(i) = myNbLocat;
          // Record 2d point
          v2.Initialize(uv.Coord(), myNbLocat, BRepMesh_OnCurve);
          iv2=myStructure->AddNode(v2);
          
          Standard_Integer isv = myVemap.FindIndex(iv2);
          if (isv == 0) isv = myVemap.Add(iv2);
          Nodes(i) = isv;
          NodInStruct(i) = myNbLocat;
          Param(i) = puv;
    
          if (orEdge == TopAbs_FORWARD)
            myStructure->AddLink(BRepMesh_Edge(ivf, iv2, BRepMesh_Frontier));
          else if (orEdge == TopAbs_REVERSED)
            myStructure->AddLink(BRepMesh_Edge(iv2, ivf, BRepMesh_Frontier));
          else if (orEdge == TopAbs_INTERNAL)
            myStructure->AddLink(BRepMesh_Edge(ivf, iv2, BRepMesh_Fixed));
          ivf = iv2;
        }
      }

      P1 = new Poly_PolygonOnTriangulation(Nodes, Param);
      P2 = new Poly_PolygonOnTriangulation(NodInStruct, Param);
    }

    P2->Deflection(otherdefedge);
    BRepMesh_PairOfPolygon pair;
    pair.Append(P2);
    myEdges.Bind(theEdge, pair);

    if (ivf != ivl) {
      if (orEdge == TopAbs_FORWARD)
        myStructure->AddLink(BRepMesh_Edge(ivf, ivl, BRepMesh_Frontier));
      else if (orEdge == TopAbs_REVERSED)
        myStructure->AddLink(BRepMesh_Edge(ivl, ivf, BRepMesh_Frontier));
      else if (orEdge == TopAbs_INTERNAL)
        myStructure->AddLink(BRepMesh_Edge(ivf, ivl, BRepMesh_Fixed));
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
      const BRepMesh_PairOfPolygon& pair = myEdges.Find(theEdge);
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
        if (BRep_Tool::SameParameter(theEdge))
        {
          for (i = 2; i < nbnodes; i++)
          {
            const Standard_Real puv = Par->Value(i);
            theC2d->D0(puv, uv);
            v2.Initialize(uv.Coord(), NOD(i), BRepMesh_OnCurve);
            iv2 = myStructure->AddNode(v2);
  
            Standard_Integer isv = myVemap.FindIndex(iv2);
            if (isv == 0) isv = myVemap.Add(iv2);
            Nodes(i) = isv;
            PPar(i) = puv;
            
            if (orEdge==TopAbs_FORWARD)
              myStructure->AddLink(BRepMesh_Edge(ivf, iv2, BRepMesh_Frontier));
            else if (orEdge == TopAbs_REVERSED)
              myStructure->AddLink(BRepMesh_Edge(iv2, ivf, BRepMesh_Frontier));
            else if (orEdge == TopAbs_INTERNAL)
              myStructure->AddLink(BRepMesh_Edge(ivf, iv2, BRepMesh_Fixed));
    
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
          
          BRepAdaptor_Curve cons(theEdge, theFace);
          Extrema_LocateExtPC pcos;
          pcos.Initialize(cons, cons.FirstParameter(), 
              cons.LastParameter(), Precision::PConfusion());

          Standard_Real wPrev;
          Standard_Real wCur      = wFirst;
          Standard_Real wCurFound = wFirst;
          for (i = 2; i < nbnodes; i++)
          {
            P3d = myLocation3d(NOD(i));
            // Record 2d point
            wPrev = wCur;
            wCur  = wFirst + wKoef*(Par->Value(i) - wFold);
                  wCurFound += (wCur - wPrev);
            pcos.Perform(P3d, wCurFound);
            if (pcos.IsDone()) wCurFound = pcos.Point().Parameter();
            theC2d->D0(wCurFound, uv);
            v2.Initialize(uv.Coord(), NOD(i), BRepMesh_OnCurve);
            iv2 = myStructure->AddNode(v2);
                  
            Standard_Integer isv = myVemap.FindIndex(iv2);
            if (isv == 0) isv = myVemap.Add(iv2); 
                  Nodes(i) = isv;
            PPar(i) = wCurFound;
                  
            if (orEdge==TopAbs_FORWARD)
              myStructure->AddLink(BRepMesh_Edge(ivf, iv2, BRepMesh_Frontier));
            else if (orEdge == TopAbs_REVERSED)
              myStructure->AddLink(BRepMesh_Edge(iv2, ivf, BRepMesh_Frontier));
            else if (orEdge == TopAbs_INTERNAL)
              myStructure->AddLink(BRepMesh_Edge(ivf, iv2, BRepMesh_Fixed));
    
            ivf = iv2;
          }
        }
      }

    P1 = new Poly_PolygonOnTriangulation(Nodes, PPar);
    
    if (ivf != ivl) {
      if (orEdge == TopAbs_FORWARD) 
        myStructure->AddLink(BRepMesh_Edge(ivf, ivl, BRepMesh_Frontier));
      else if (orEdge == TopAbs_REVERSED)
        myStructure->AddLink(BRepMesh_Edge(ivl, ivf, BRepMesh_Frontier));
      else if (orEdge == TopAbs_INTERNAL)
        myStructure->AddLink(BRepMesh_Edge(ivf, ivl, BRepMesh_Fixed));
      }
    }
  }

  P1->Deflection(theDefEdge);
  if (myInternaledges.IsBound(theEdge))
  {
    BRepMesh_PairOfPolygon& pair = myInternaledges.ChangeFind(theEdge);
    if (orEdge == TopAbs_REVERSED)
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
}


//=======================================================================
//function : Update(edge)
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_FastDiscret::Update(const TopoDS_Edge&          theEdge,
                                              const TopoDS_Face&          theFace,
                                              const Handle(Geom2d_Curve)& theC2d,
                                              const Standard_Real         theDefEdge,
                                              const Standard_Real         theFirst,
                                              const Standard_Real         theLast)
{
  TopLoc_Location Loc;
  Handle(Poly_Triangulation) T, TNull;
  Handle(Poly_PolygonOnTriangulation) Poly, NullPoly;

  Standard_Integer i = 1;
  Standard_Boolean found = Standard_False;
  do
  {
    BRep_Tool::PolygonOnTriangulation(theEdge,Poly,T,Loc,i);
    i++;
    if (!found && !T.IsNull() && T->HasUVNodes() && 
        !Poly.IsNull() && Poly->HasParameters())
    {
      if (Poly->Deflection() <= 1.1*theDefEdge)
      {
        // 2d vertex indices
        TopAbs_Orientation orEdge = theEdge.Orientation();
        Standard_Integer iv1, iv2, ivl;
        Standard_Integer isv1, isv, isvl;
        
        // Get range on 2d curve
        Standard_Real wFirst, wLast;
        BRep_Tool::Range(theEdge, theFace, wFirst, wLast);
        
        // Get end points on 2d curve
        gp_Pnt2d uvFirst, uvLast;
        BRep_Tool::UVPoints(theEdge, theFace, uvFirst, uvLast);

        // Get vertices
        TopoDS_Vertex pBegin, pEnd;
        TopExp::Vertices(theEdge,pBegin,pEnd);
        
        const Standard_Boolean sameUV =
          uvFirst.IsEqual(uvLast, Precision::PConfusion());
        
        //Controle vertice tolerances
        BRepAdaptor_Surface  BS(theFace, Standard_False);
        Handle(BRepAdaptor_HSurface) gFace = new BRepAdaptor_HSurface(BS);
        

        gp_Pnt pFirst = gFace->Value(uvFirst.X(), uvFirst.Y());
        gp_Pnt pLast  = gFace->Value(uvLast.X(), uvLast.Y());
        
        Standard_Real mindist = 10. * Max(pFirst.Distance(BRep_Tool::Pnt(pBegin)), 
                                          pLast.Distance(BRep_Tool::Pnt(pEnd)));

        if (mindist < BRep_Tool::Tolerance(pBegin) ||
            mindist < BRep_Tool::Tolerance(pEnd) ) mindist = theDefEdge;
        
        if (sameUV)
        {
          // 1. is it really sameUV without being degenerated
          gp_Pnt2d uvF, uvL;
          theC2d->D0(theFirst, uvF);
          theC2d->D0(theLast, uvL);
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
            if (!Loc.IsIdentity())
              P3d.Transform(Loc.Transformation());
            myNbLocat++;
            myLocation3d.Bind(myNbLocat,P3d);
            ipf = myNbLocat;
          }
          myVertices.Bind(pBegin,ipf);
        }
        NewNodInStruct(1) = ipf;
        theUV = BRepMesh_FastDiscretFace::FindUV(pBegin, uvFirst, ipf, gFace, mindist, myLocation2d);
        BRepMesh_Vertex vf(theUV,ipf,BRepMesh_Frontier);
        iv1 = myStructure->AddNode(vf);
        isv1 = myVemap.FindIndex(iv1);
        if (isv1 == 0) isv1 = myVemap.Add(iv1);
        NewNodes(1) = isv1;
        
        // Process last vertex
        Standard_Integer ipl;
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
              isv1 = isv1;
            }
            else
            {
              myNbLocat++;
              myLocation3d.Bind(myNbLocat,Nodes(Indices(nbnodes)).Transformed(Loc.Transformation()));
              ipl = myNbLocat;
            }
            myVertices.Bind(pEnd,ipl);
          }
        }
        NewNodInStruct(nbnodes) = ipl;
        theUV = BRepMesh_FastDiscretFace::FindUV(pEnd, uvLast, ipl, gFace, mindist, myLocation2d);
        BRepMesh_Vertex vl(theUV,ipl,BRepMesh_Frontier);
        
        ivl = myStructure->AddNode(vl);
        isvl = myVemap.FindIndex(ivl);
        if (isvl == 0) isvl = myVemap.Add(ivl);
        
        NewNodes(nbnodes) = isvl;
  
        gp_Pnt2d uv;
        BRepMesh_Vertex v;
  
        if (BRep_Tool::SameParameter(theEdge))
        {
          for (i = 2; i < Indices.Length(); i++)
          {
            // Record 3d point
            P3d = Nodes(Indices(i));
            if (!Loc.IsIdentity())
              P3d.Transform(Loc.Transformation());
            myNbLocat++;
            myLocation3d.Bind(myNbLocat, P3d);
            NewNodInStruct(i) = myNbLocat;
            // Record 2d point
            uv = theC2d->Value(Param->Value(i));
            v.Initialize(uv.Coord(), myNbLocat, BRepMesh_Frontier);
            iv2 = myStructure->AddNode(v);
            isv = myVemap.FindIndex(iv2);
            if (isv == 0) isv = myVemap.Add(iv2);
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
    
          BRepAdaptor_Curve cons(theEdge, theFace);
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
            if (!Loc.IsIdentity())
              P3d.Transform(Loc.Transformation());
            myNbLocat++;
            myLocation3d.Bind(myNbLocat, P3d);
            NewNodInStruct(i) = myNbLocat;
            // Record 2d point
            wPrev = wCur;
            wCur  = wFirst + wKoef*(Param->Value(i) - wFold);
            wCurFound += (wCur - wPrev);
            pcos.Perform(P3d, wCurFound);
            if (pcos.IsDone()) wCurFound = pcos.Point().Parameter();
            theC2d->D0(wCurFound, uv);
            v.Initialize(uv.Coord(), myNbLocat, BRepMesh_Frontier);
            iv2 = myStructure->AddNode(v);
            isv = myVemap.FindIndex(iv2);
            if (isv == 0) isv = myVemap.Add(iv2);
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
        }
        
        Handle(Poly_PolygonOnTriangulation) P1 =
          new Poly_PolygonOnTriangulation(NewNodes, Param->Array1());
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
        
        Handle(Poly_PolygonOnTriangulation) P2 =
          new Poly_PolygonOnTriangulation(NewNodInStruct, Param->Array1());
        P2->Deflection(theDefEdge);
        BRepMesh_PairOfPolygon pair;
        pair.Append(P2);
        myEdges.Bind(theEdge, pair);
        
        found = Standard_True;
      }
      else
      {
        BRep_Builder B;
        B.UpdateEdge(theEdge,NullPoly,T,Loc);
        B.UpdateFace(theFace,TNull);
      }
    }
    else if (!T.IsNull() && !T->HasUVNodes())
    {
      BRep_Builder B;
      B.UpdateEdge(theEdge,NullPoly,T,Loc);
      B.UpdateFace(theFace,TNull);
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
  return myStructure->NbElements();
}

//=======================================================================
//function : Triangle
//purpose  : 
//=======================================================================

const BRepMesh_Triangle& BRepMesh_FastDiscret::Triangle
  (const Standard_Integer Index) const
{
  return myStructure->GetElement(Index);
}

//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

Standard_Integer BRepMesh_FastDiscret::NbEdges() const
{
  return myStructure->NbLinks();
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const BRepMesh_Edge& BRepMesh_FastDiscret::Edge(const Standard_Integer Index) const
{
  return myStructure->GetLink(Index);
}

//=======================================================================
//function : NbVertices
//purpose  : 
//=======================================================================

Standard_Integer BRepMesh_FastDiscret::NbVertices() const
{
  return myStructure->NbNodes();
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const BRepMesh_Vertex& BRepMesh_FastDiscret::Vertex
  (const Standard_Integer Index) const
{
  return myStructure->GetNode(Index);
}

//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

const gp_Pnt& BRepMesh_FastDiscret::Pnt(const Standard_Integer Index) const
{
  return myLocation3d(myStructure->GetNode(Index).Location3d());
}

//=======================================================================
//function : VerticesOfDomain
//purpose  : 
//=======================================================================

void BRepMesh_FastDiscret::VerticesOfDomain(BRepMesh_MapOfInteger&  Indices) const 
{ 
  Indices.Clear();
  
  // recuperate from the map of edges.
  const BRepMesh_MapOfInteger& edmap = myStructure->LinkOfDomain();

  // iterator on edges.
  BRepMesh_MapOfInteger::Iterator iter(edmap);
  
  Standard_Integer ind_edge;
  for (iter.Reset(); iter.More(); iter.Next()) {
    ind_edge = iter.Key();
    const BRepMesh_Edge& Ed = Edge(ind_edge);
    Indices.Add(Ed.FirstNode());
    Indices.Add(Ed.LastNode());
  }
}

BRepMesh_Status BRepMesh_FastDiscret::CurrentFaceStatus() const
{
  return myFacestate;
}

//=======================================================================
//function : GetFaceAttribute
//purpose  : 
//=======================================================================

Standard_Boolean BRepMesh_FastDiscret::GetFaceAttribute(const TopoDS_Face&              theFace,
                                                        Handle(BRepMesh_FaceAttribute)& theFattrib) const
{
  if(myMapattrib.IsBound(theFace))
  {
    theFattrib = myMapattrib(theFace);
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
  if(myMapattrib.IsBound(theFace))
    myMapattrib.UnBind(theFace);
}

//=======================================================================
//function : CreateMutexesForSubShapes
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::CreateMutexesForSubShapes(const TopoDS_Shape& theShape,
                                                     const TopAbs_ShapeEnum theType)
{
  myMutexProvider.CreateMutexesForSubShapes(theShape, theType);
}

//=======================================================================
//function : RemoveAllMutexes
//purpose  : 
//=======================================================================
void BRepMesh_FastDiscret::RemoveAllMutexes()
{
  myMutexProvider.RemoveAllMutexes();
}