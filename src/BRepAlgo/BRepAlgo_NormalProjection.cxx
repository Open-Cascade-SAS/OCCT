// Created on: 1997-10-13
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Adaptor3d_Curve.hxx>
#include <Approx_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo_NormalProjection.hxx>
#include <BRepAlgo_SequenceOfSequenceOfInteger.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAdaptor.hxx>
#include <Precision.hxx>
#include <ProjLib_CompProjectedCurve.hxx>
#include <ProjLib_HCompProjectedCurve.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#ifdef OCCT_DEBUG_CHRONO
#include <OSD_Timer.hxx>

OSD_Chronometer chr_total, chr_init, chr_approx, chr_booltool;

Standard_Real t_total, t_init, t_approx, t_booltool;
Standard_IMPORT Standard_Real t_init_point, t_dicho_bound;
Standard_IMPORT Standard_Integer init_point_count, dicho_bound_count;

void InitChron(OSD_Chronometer& ch)
{ 
    ch.Reset();
    ch.Start();
}

void ResultChron( OSD_Chronometer & ch, Standard_Real & time) 
{
    Standard_Real tch ;
    ch.Stop();
    ch.Show(tch);
    time=time +tch;
}

#endif
//=======================================================================
//function : BRepAlgo_NormalProjection
//purpose  : 
//=======================================================================

 BRepAlgo_NormalProjection::BRepAlgo_NormalProjection() 
   : myWith3d(Standard_True)

{
  BRep_Builder BB;
  BB.MakeCompound(TopoDS::Compound(myToProj));
  myFaceBounds=Standard_True;
  SetDefaultParams();
  myMaxDist = -1;
}

//=======================================================================
//function : BRepAlgo_NormalProjection
//purpose  : 
//=======================================================================

 BRepAlgo_NormalProjection::BRepAlgo_NormalProjection(const TopoDS_Shape& S) 
    : myWith3d(Standard_True)
{
  BRep_Builder BB;
  BB.MakeCompound(TopoDS::Compound(myToProj));
  SetDefaultParams();
  myMaxDist = -1;
  Init(S);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

 void BRepAlgo_NormalProjection::Init(const TopoDS_Shape& S) 
{
  myShape = S;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

 void BRepAlgo_NormalProjection::Add(const TopoDS_Shape& ToProj) 
{
  BRep_Builder BB;
  BB.Add(myToProj, ToProj);
}

//=======================================================================
//function : SetParams
//purpose  : 
//=======================================================================

void BRepAlgo_NormalProjection::SetParams(const Standard_Real Tol3D,
                                           const Standard_Real Tol2D,
                                           const GeomAbs_Shape InternalContinuity,
                                           const Standard_Integer MaxDegree,
                                           const Standard_Integer MaxSeg) 
{
  myTol3d = Tol3D;
  myTol2d = Tol2D;
  myContinuity = InternalContinuity;
  myMaxDegree = MaxDegree;
  myMaxSeg = MaxSeg;
}

//=======================================================================
//function : SetDefaultParams
//purpose  : 
//=======================================================================

void BRepAlgo_NormalProjection::SetDefaultParams()
{
  myTol3d = 1.e-4;
  myTol2d = Pow(myTol3d, 2./3);
  myContinuity = GeomAbs_C2;
  myMaxDegree = 14;
  myMaxSeg    = 16;
}

//=======================================================================
//function : SetLimits
//purpose  : 
//=======================================================================

 void BRepAlgo_NormalProjection::SetLimit(const Standard_Boolean FaceBounds)
{
  myFaceBounds = FaceBounds;
}

//=======================================================================
//function : SetMaxDistance
//purpose  : 
//=======================================================================

 void BRepAlgo_NormalProjection::SetMaxDistance(const Standard_Real MaxDist)
{
  myMaxDist = MaxDist;
}

//=======================================================================
//function : Compute3d
//purpose  : 
//=======================================================================

 void BRepAlgo_NormalProjection::Compute3d(const Standard_Boolean With3d)
{
  myWith3d = With3d;
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

 void BRepAlgo_NormalProjection::Build() 
{
#ifdef OCCT_DEBUG_CHRONO
  Standard_Integer init_count = 0, approx_count = 0, booltool_count = 0;
  t_total = 0;
  t_init = 0;
  t_approx = 0;
  t_booltool = 0;
  
  t_init_point = 0;
  init_point_count = 0;
  
  t_dicho_bound = 0;
  dicho_bound_count = 0;
  
  InitChron(chr_total);
#endif
  myIsDone = Standard_False;
  ProjLib_CompProjectedCurve Projector;
  Handle(TopTools_HSequenceOfShape) Edges = new TopTools_HSequenceOfShape();
  Handle(TopTools_HSequenceOfShape) Faces = new TopTools_HSequenceOfShape();
  TopTools_ListOfShape DescenList;
  Standard_Integer NbEdges = 0, NbFaces = 0, i, j, k;
  TopExp_Explorer ExpOfWire, ExpOfShape;
  Standard_Real Udeb, Ufin;
  TopoDS_Shape VertexRes;
  Standard_Boolean Only3d, Only2d, Elementary;
  
  // for isoparametric cases
  TColgp_Array1OfPnt2d Poles(1, 2);
  TColStd_Array1OfReal Knots(1, 2);
  TColStd_Array1OfInteger Mults(1,2);
  Standard_Integer Deg;
  Deg = 1;
  Mults(1) = Deg + 1;
  Mults(2) = Deg + 1;
  //
  
  for(ExpOfWire.Init(myToProj, TopAbs_EDGE); 
      ExpOfWire.More(); 
      ExpOfWire.Next(), NbEdges++) {
    Edges->Append(ExpOfWire.Current());
  }
  
  for(ExpOfShape.Init(myShape, TopAbs_FACE); 
      ExpOfShape.More(); 
      ExpOfShape.Next(), NbFaces++) {
    Faces->Append(ExpOfShape.Current());
  }
  
  BRep_Builder BB;
  BB.MakeCompound(TopoDS::Compound(myRes));
  BB.MakeCompound(TopoDS::Compound(VertexRes));
  Standard_Boolean YaVertexRes = Standard_False;
  
  for(i = 1; i <= NbEdges; i++){
    DescenList.Clear();
    BRepAdaptor_Curve cur(TopoDS::Edge(Edges->Value(i)));
    Handle(BRepAdaptor_HCurve) hcur = new BRepAdaptor_HCurve();
    hcur->Set(cur);
    Elementary = IsElementary(cur);
    for(j = 1; j <= NbFaces; j++){
      BRepAdaptor_Surface sur(TopoDS::Face(Faces->Value(j)));
      Handle(BRepAdaptor_HSurface) hsur = new BRepAdaptor_HSurface();
      hsur->Set(sur);
      
      // computation of  TolU and TolV
      
      Standard_Real  TolU, TolV;
      
      TolU = hsur->UResolution(myTol3d)/20;
      TolV = hsur->VResolution(myTol3d)/20;
      // Projection
#ifdef OCCT_DEBUG_CHRONO
      InitChron(chr_init);
#endif
      Projector = 
	ProjLib_CompProjectedCurve(hsur, hcur, TolU, TolV, myMaxDist);
#ifdef OCCT_DEBUG_CHRONO
      ResultChron(chr_init,t_init);
      init_count++;
#endif
      //
      Handle(ProjLib_HCompProjectedCurve) HProjector = 
	new ProjLib_HCompProjectedCurve();
      HProjector->Set(Projector);
      TopoDS_Shape prj;
      Standard_Boolean Degenerated = Standard_False;
      gp_Pnt2d P2d, Pdeb, Pfin;
      gp_Pnt P;
      Standard_Real UIso, VIso;
      
      Handle(Adaptor2d_HCurve2d) HPCur;
      Handle(Geom2d_Curve) PCur2d; // Only for isoparametric projection
      
      for(k = 1; k <= Projector.NbCurves(); k++){
	if(Projector.IsSinglePnt(k, P2d)){
#ifdef OCCT_DEBUG
	  cout << "Projection of edge "<<i<<" on face "<<j;
	  cout << " is punctual"<<endl<<endl;
#endif
	  Projector.GetSurface()->D0(P2d.X(), P2d.Y(), P);
	  prj = BRepLib_MakeVertex(P).Shape();
	  DescenList.Append(prj);
	  BB.Add(VertexRes, prj);
	  YaVertexRes = Standard_True;
	  
	  myAncestorMap.Bind(prj, Edges->Value(i));
	}
	else {
	  Only2d = Only3d = Standard_False;
	  Projector.Bounds(k, Udeb, Ufin);
	  
	  /**************************************************************/
	  if (Projector.IsUIso(k, UIso)) {
#ifdef OCCT_DEBUG
	    cout << "Projection of edge "<<i<<" on face "<<j;
	    cout << " is U-isoparametric"<<endl<<endl;
#endif
	    Projector.D0(Udeb, Pdeb);
	    Projector.D0(Ufin, Pfin);
	    Poles(1) = Pdeb;
	    Poles(2) = Pfin;
	    Knots(1) = Udeb;
	    Knots(2) = Ufin;
	    Handle(Geom2d_BSplineCurve) BS2d = 
	      new Geom2d_BSplineCurve(Poles, Knots, Mults, Deg);
	    PCur2d = new Geom2d_TrimmedCurve( BS2d, Udeb, Ufin);
	    HPCur = new Geom2dAdaptor_HCurve(PCur2d);
	    Only3d = Standard_True;
	  }
	  else if (Projector.IsVIso(k, VIso)) {
#ifdef OCCT_DEBUG
	    cout << "Projection of edge "<<i<<" on face "<<j;
	    cout << " is V-isoparametric"<<endl<<endl;
#endif
	    Projector.D0(Udeb, Pdeb);
	    Projector.D0(Ufin, Pfin);
	    Poles(1) = Pdeb;
	    Poles(2) = Pfin;
	    Knots(1) = Udeb;
	    Knots(2) = Ufin;
	    Handle(Geom2d_BSplineCurve) BS2d = 
	      new Geom2d_BSplineCurve(Poles, Knots, Mults, Deg);
	    PCur2d = new Geom2d_TrimmedCurve(BS2d, Udeb, Ufin);
	    HPCur = new Geom2dAdaptor_HCurve(PCur2d);
	    Only3d = Standard_True;
	  }
	  else HPCur = HProjector;
	  
	  if((myWith3d == Standard_False || Elementary) && 
	     (Projector.MaxDistance(k) <= myTol3d)        )
	    Only2d = Standard_True;
	  
	  if(Only2d && Only3d) {
	    BRepLib_MakeEdge MKed(GeomAdaptor::MakeCurve(hcur->Curve()), 
				  Ufin, Udeb);
	    prj = MKed.Edge();
	    BB.UpdateEdge(TopoDS::Edge(prj), 
			  PCur2d, 
			  TopoDS::Face(Faces->Value(j)),
			  myTol3d);
            BB.UpdateVertex(TopExp::FirstVertex(TopoDS::Edge(prj)),myTol3d);
            BB.UpdateVertex(TopExp::LastVertex(TopoDS::Edge(prj)),myTol3d);
	  }
	  else {
#ifdef OCCT_DEBUG_CHRONO
	    InitChron(chr_approx);
#endif
	    Approx_CurveOnSurface appr(HPCur, hsur, Udeb, Ufin, myTol3d, 
				       myContinuity, myMaxDegree, myMaxSeg, 
				       Only3d, Only2d);
#ifdef OCCT_DEBUG_CHRONO
	    ResultChron(chr_approx,t_approx);
	    approx_count++;
	    
	    cout<<"Approximation.IsDone = "<<appr.IsDone()<<endl;
	    if(!Only2d)
	      cout<<"MaxError3d = "<<appr.MaxError3d()<<endl<<endl;
	    if(!Only3d) {
	      cout<<"MaxError2dU = "<<appr.MaxError2dU()<<endl;
	      cout<<"MaxError2dV = "<<appr.MaxError2dV()<<endl<<endl;
	    }
#endif

	    
	    if(!Only3d) PCur2d = appr.Curve2d();
	    if(Only2d) {
	      BRepLib_MakeEdge MKed(GeomAdaptor::MakeCurve(hcur->Curve()), 
				    Udeb, Ufin);
	      prj = MKed.Edge();
	    }
	    else  {
            // It is tested if the solution is not degenerated to set the
            // flag on edge, one takes several points, checks if the cloud of 
            // points has less diameter than the tolerance 3D
              Degenerated = Standard_True;
              Standard_Real Dist;
              Handle(Geom_BSplineCurve) BS3d = appr.Curve3d();
              gp_Pnt P1(0.,0.,0.),PP; // skl : I change "P" to "PP"
              Standard_Integer NbPoint,ii ; // skl : I change "i" to "ii"
              Standard_Real Par,DPar;
              // start from 3 points to reject non degenerated edges 
              // very fast
              NbPoint =3;
              DPar = (BS3d->LastParameter()-BS3d->FirstParameter())/(NbPoint-1);
              for (ii=0;ii<NbPoint;ii++)
	      {
                 Par=BS3d->FirstParameter()+ii*DPar;
                 PP=BS3d->Value(Par);
                 P1.SetXYZ(P1.XYZ() + PP.XYZ()/NbPoint);
	      }
              for (ii=0;ii<NbPoint && Degenerated ;ii++)   
              {
                 Par=BS3d->FirstParameter()+ii*DPar;
                 PP=BS3d->Value(Par);                 
                 Dist=P1.Distance(PP);
                 if(Dist > myTol3d) {
                     Degenerated = Standard_False;
                     break;
		 }
              }             
              // if the test passes a more exact test with 10 points
              if (Degenerated) {
                 P1.SetCoord(0.,0.,0.);
                 NbPoint =10;
                 DPar = (BS3d->LastParameter()-BS3d->FirstParameter())/(NbPoint-1);
                 for (ii=0;ii<NbPoint;ii++)
	         {
		   Par=BS3d->FirstParameter()+ii*DPar;
		   PP=BS3d->Value(Par);
		   P1.SetXYZ(P1.XYZ() + PP.XYZ()/NbPoint);
		 }
                 for (ii=0;ii<NbPoint && Degenerated ;ii++)   
                 {
		   Par=BS3d->FirstParameter()+ii*DPar;
		   PP=BS3d->Value(Par);                 
		   Dist=P1.Distance(PP);
		   if(Dist > myTol3d) {
                     Degenerated = Standard_False;
                     break;
		   }
                 }             
              }
	      if (Degenerated) {
#ifdef OCCT_DEBUG
	          cout << "Projection of edge "<<i<<" on face "<<j;
	          cout << " is degenerated "<<endl<<endl;
#endif
		TopoDS_Vertex VV;
		BB.MakeVertex(VV);
		BB.UpdateVertex(VV,P1,myTol3d);
		BB.MakeEdge(TopoDS::Edge(prj));
		BB.Add(TopoDS::Edge(prj),VV.Oriented(TopAbs_FORWARD));
		BB.Add(TopoDS::Edge(prj),VV.Oriented(TopAbs_REVERSED));
		BB.Degenerated(TopoDS::Edge(prj), Standard_True);
	      }
	      else {
		prj = BRepLib_MakeEdge(BS3d).Edge();
	      }
	    }
	    
	    BB.UpdateEdge(TopoDS::Edge(prj), 
			  PCur2d,
			  TopoDS::Face(Faces->Value(j)), 
			  appr.MaxError3d());
            BB.UpdateVertex(TopExp::FirstVertex(TopoDS::Edge(prj)),appr.MaxError3d());
            BB.UpdateVertex(TopExp::LastVertex(TopoDS::Edge(prj)),appr.MaxError3d());
	    if (Degenerated) {
	      BB.Range(TopoDS::Edge(prj),
		       TopoDS::Face(Faces->Value(j)),
		       Udeb,Ufin);
	    }
	  }
	  
	  if(myFaceBounds) {
	    // Trimming edges by face bounds 
            // if the solution is degenerated, use of BoolTool is avoided
#ifdef OCCT_DEBUG_CHRONO
	    InitChron(chr_booltool);
#endif
            if (!Degenerated) {
              // Perform Boolean COMMON operation to get parts of projected edge
              // inside the face
              BRepAlgoAPI_Section aSection(Faces->Value(j), prj);
              if (aSection.IsDone()) {
                const TopoDS_Shape& aRC = aSection.Shape();
                //
                TopExp_Explorer aExpE(aRC, TopAbs_EDGE);
                for (; aExpE.More(); aExpE.Next()) {
                  const TopoDS_Shape& aE = aExpE.Current();
                  BB.Add(myRes, aE);
                  myAncestorMap.Bind(aE, Edges->Value(i));
                  myCorresp.Bind(aE, Faces->Value(j));
                }
              }
              else {
                // if the common operation has failed, try to classify the part
	         BRepTopAdaptor_FClass2d classifier(TopoDS::Face(Faces->Value(j)),
			  		  	    Precision::Confusion());
	         gp_Pnt2d Puv;
	         Standard_Real f = PCur2d->FirstParameter();
	         Standard_Real l = PCur2d->LastParameter();
	         Standard_Real pmil = (f + l )/2;
	         PCur2d->D0(pmil, Puv);
	         TopAbs_State state;
	         state = classifier.Perform(Puv);
	         if(state == TopAbs_IN || state  == TopAbs_ON) {
		   BB.Add(myRes, prj);
		   DescenList.Append(prj);
		   myAncestorMap.Bind(prj, Edges->Value(i));   
		   myCorresp.Bind(prj, Faces->Value(j));
	         }
	       }
            }
            else {
#ifdef OCCT_DEBUG
                 cout << " BooleanOperations : no solution " << endl;
#endif

	      BRepTopAdaptor_FClass2d classifier(TopoDS::Face(Faces->Value(j)),
			  		  	 Precision::Confusion());
	      gp_Pnt2d Puv;
	      Standard_Real f = PCur2d->FirstParameter();
	      Standard_Real l = PCur2d->LastParameter();
	      Standard_Real pmil = (f + l )/2;
	      PCur2d->D0(pmil, Puv);
	      TopAbs_State state;
	      state = classifier.Perform(Puv);
	      if(state == TopAbs_IN || state  == TopAbs_ON) {
		 BB.Add(myRes, prj);
		 DescenList.Append(prj);
		 myAncestorMap.Bind(prj, Edges->Value(i));   
		 myCorresp.Bind(prj, Faces->Value(j));
	      }
#ifdef OCCT_DEBUG_CHRONO
	       ResultChron(chr_booltool,t_booltool);
	       booltool_count++;
#endif
            }
	  }
	  else {
	    BB.Add(myRes, prj);
	    DescenList.Append(prj);
	    myAncestorMap.Bind(prj, Edges->Value(i));   
	    myCorresp.Bind(prj, Faces->Value(j));
	  }
	}
      }
    }
    myDescendants.Bind(Edges->Value(i), DescenList);
  }
// JPI : eventual wire creation is reported in a specific method 
//       BuilWire that can be called by the user. Otherwise, the 
//       relations of map myAncestorMap, myCorresp will be lost.  

  if(YaVertexRes) BB.Add(myRes, VertexRes);
  
  myIsDone = Standard_True; 
  
#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_total,t_total);
  
  cout<<"Build - Total time  : "<<t_total<<" includes:" <<endl;
  cout<<"- Projection           : "<<t_init<<endl;
  cout<<"  -- Initial point search : "<<t_init_point<<endl;
  cout<<"  -- DichoBound search : "<<t_dicho_bound<<endl;
  cout<<"- Approximation        : "<<t_approx<<endl;
  cout<<"- Boolean operation    : "<<t_booltool<<endl;
  cout<<"- Rest of time         : "<<t_total-(t_init + t_approx + t_booltool )<<endl<<endl;
  if (init_count != 0)
    t_init /= init_count;
  if (init_point_count != 0)
    t_init_point /= init_point_count;
  if (dicho_bound_count != 0)
    t_dicho_bound /= dicho_bound_count;
  if (approx_count != 0)
    t_approx /= approx_count;
  if (booltool_count != 0)
    t_booltool /= booltool_count;
  
  cout<<"Unitary average time  : "<<endl;
  cout<<"- Projection          : "<<t_init<<endl;
  cout<<"  -- Initial point search: "<<t_init_point<<endl;
  cout<<"  -- DichoBound search : "<<t_dicho_bound<<endl;
  cout<<"- Approximation       : "<<t_approx<<endl;
  cout<<"- Boolean operation   :"<<t_booltool<<endl;
  cout<<endl<<"Number of initial point computations is "<<init_point_count<<endl<<endl;
#endif
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

 Standard_Boolean BRepAlgo_NormalProjection::IsDone() const
{
  return myIsDone;
}

//=======================================================================
//function : Projection
//purpose  : 
//=======================================================================

 const TopoDS_Shape& BRepAlgo_NormalProjection::Projection() const
{
  return myRes;
}

//=======================================================================
//function : Ancestor
//purpose  : 
//=======================================================================

 const TopoDS_Shape& BRepAlgo_NormalProjection::Ancestor(const TopoDS_Edge& E) const
{
  return myAncestorMap.Find(E);
}

//=======================================================================
//function : Couple
//purpose  : 
//=======================================================================

 const TopoDS_Shape& BRepAlgo_NormalProjection::Couple(const TopoDS_Edge& E) const
{
  return myCorresp.Find(E);
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

 const TopTools_ListOfShape& BRepAlgo_NormalProjection::Generated(const TopoDS_Shape& S)
{
  return myDescendants.Find(S);
}

//=======================================================================
//function : IsElementary
//purpose  : 
//=======================================================================

 Standard_Boolean BRepAlgo_NormalProjection::IsElementary(const Adaptor3d_Curve& C) const
{
  GeomAbs_CurveType type;
  type = C.GetType();
  switch(type) {
  case GeomAbs_Line:
  case GeomAbs_Circle:
  case GeomAbs_Ellipse:
  case GeomAbs_Hyperbola:
  case GeomAbs_Parabola: return Standard_True;
  default: return Standard_False;
  }
}
//=======================================================================
//function : BuildWire
//purpose  : 
//=======================================================================
 
 Standard_Boolean BRepAlgo_NormalProjection::BuildWire(TopTools_ListOfShape& ListOfWire) const
{
  TopExp_Explorer ExpOfWire, ExpOfShape; 
  Standard_Boolean IsWire=Standard_False;
  ExpOfShape.Init(myRes, TopAbs_EDGE);
  if(ExpOfShape.More()) 
  {
    TopTools_ListOfShape List;

    for ( ; ExpOfShape.More(); ExpOfShape.Next()) 
    {
      const TopoDS_Shape& CurE = ExpOfShape.Current();
      List.Append(CurE);
    }
    BRepLib_MakeWire MW;
    MW.Add(List);
    if (MW.IsDone()) 
    {
      const TopoDS_Shape& Wire = MW.Shape();
      // If the resulting wire contains the same edge as at the beginning OK
      // otherwise the result really consists of several wires.
      TopExp_Explorer exp2(Wire,TopAbs_EDGE);
      Standard_Integer NbEdges = 0;
      for (;exp2.More(); exp2.Next()) NbEdges++;
      if ( NbEdges == List.Extent()) 
      {
          ListOfWire.Append(Wire);
          IsWire = Standard_True;
      }
    }
  }
   return IsWire;
}
