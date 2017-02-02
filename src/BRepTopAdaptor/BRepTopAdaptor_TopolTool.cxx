// Copyright (c) 1995-1999 Matra Datavision
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepClass_FaceExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRepTopAdaptor_HVertex.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TopoDS.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepTopAdaptor_TopolTool,Adaptor3d_TopolTool)

static 
  void Analyse(const TColgp_Array2OfPnt& array2,
	       const Standard_Integer nbup,
	       const Standard_Integer nbvp,
	       Standard_Integer& myNbSamplesU,
	       Standard_Integer& myNbSamplesV); 
//=======================================================================
//function : BRepTopAdaptor_TopolTool
//purpose  : 
//=======================================================================
  BRepTopAdaptor_TopolTool::BRepTopAdaptor_TopolTool () : myFClass2d(NULL)
{
  myNbSamplesU=-1;
}

//=======================================================================
//function : BRepTopAdaptor_TopolTool
//purpose  : 
//=======================================================================
  BRepTopAdaptor_TopolTool::BRepTopAdaptor_TopolTool(const Handle(Adaptor3d_HSurface)& S) 
: myFClass2d(NULL)
{
  Initialize(S);
  //myS = S;
}
//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
  void BRepTopAdaptor_TopolTool::Initialize()
{
  throw Standard_NotImplemented("BRepTopAdaptor_TopolTool::Initialize()");
}
//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
  void BRepTopAdaptor_TopolTool::Initialize(const Handle(Adaptor3d_HSurface)& S)
{
  Handle(BRepAdaptor_HSurface) brhs = 
    Handle(BRepAdaptor_HSurface)::DownCast(S);
  if (brhs.IsNull()) {throw Standard_ConstructionError();}
  TopoDS_Shape s_wnt = ((BRepAdaptor_Surface *)&(brhs->Surface()))->Face();
  s_wnt.Orientation(TopAbs_FORWARD);
  myFace = TopoDS::Face(s_wnt);
  if(myFClass2d != NULL) { 
    delete (BRepTopAdaptor_FClass2d *)myFClass2d;
  } 
  myFClass2d = NULL;
  myNbSamplesU=-1;
  myS = S;
  myCurves.Clear();
  TopExp_Explorer ex(myFace,TopAbs_EDGE);
  for (; ex.More(); ex.Next()) {
    Handle(BRepAdaptor_HCurve2d) aCurve = new BRepAdaptor_HCurve2d
      (BRepAdaptor_Curve2d(TopoDS::Edge(ex.Current()),myFace));
    myCurves.Append(aCurve);
  }
  myCIterator = TColStd_ListIteratorOfListOfTransient();
}
//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
  void BRepTopAdaptor_TopolTool::Initialize(const Handle(Adaptor2d_HCurve2d)& C)
{
  myCurve = Handle(BRepAdaptor_HCurve2d)::DownCast(C);
  if (myCurve.IsNull()) {throw Standard_ConstructionError();}
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void BRepTopAdaptor_TopolTool::Init ()
{
  myCIterator.Initialize(myCurves);
}
//=======================================================================
//function : More
//purpose  : 
//=======================================================================
  Standard_Boolean BRepTopAdaptor_TopolTool::More ()
{
  return myCIterator.More();
}
//=======================================================================
//function : Next
//purpose  : 
//=======================================================================
  void BRepTopAdaptor_TopolTool::Next()
{
  myCIterator.Next();
}
//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
  Handle(Adaptor2d_HCurve2d) BRepTopAdaptor_TopolTool::Value ()
{
  return Handle(Adaptor2d_HCurve2d)::DownCast(myCIterator.Value());
}
//modified by NIZNHY-PKV Tue Mar 27 14:23:40 2001 f
//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
  Standard_Address BRepTopAdaptor_TopolTool::Edge () const
{
  Handle(BRepAdaptor_HCurve2d) aHCurve =
    Handle(BRepAdaptor_HCurve2d)::DownCast(myCIterator.Value());
  const BRepAdaptor_Curve2d& aCurve = (const BRepAdaptor_Curve2d&)aHCurve->Curve2d();
  return Standard_Address (& aCurve.Edge());
}

//modified by NIZNHY-PKV Tue Mar 27 14:23:43 2001 t
//=======================================================================
//function : InitVertexIterator
//purpose  : 
//=======================================================================
  void BRepTopAdaptor_TopolTool::InitVertexIterator ()
{
  myVIterator.Init(((BRepAdaptor_Curve2d *)&(myCurve->Curve2d()))->Edge(),TopAbs_VERTEX);
}
//=======================================================================
//function : NextVertex
//purpose  : 
//=======================================================================
  Standard_Boolean BRepTopAdaptor_TopolTool::MoreVertex ()
{
  return myVIterator.More();
}

  void BRepTopAdaptor_TopolTool::NextVertex ()
{
  myVIterator.Next();
}
//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================
  Handle(Adaptor3d_HVertex) BRepTopAdaptor_TopolTool::Vertex ()
{
  return new 
    BRepTopAdaptor_HVertex(TopoDS::Vertex(myVIterator.Current()),myCurve);
}
  
//=======================================================================
//function : Classify
//purpose  : 
//=======================================================================
  TopAbs_State BRepTopAdaptor_TopolTool::Classify(const gp_Pnt2d& P,
						  const Standard_Real Tol,
						  const Standard_Boolean RecadreOnPeriodic)
{
  if(myFace.IsNull())
    return TopAbs_UNKNOWN;
  if(myFClass2d == NULL) { 
    myFClass2d = (void *) new BRepTopAdaptor_FClass2d(myFace,Tol);
  } 
  return(((BRepTopAdaptor_FClass2d *)myFClass2d)->Perform(P,RecadreOnPeriodic));
}

//=======================================================================
//function : IsThePointOn
//purpose  : 
//=======================================================================
  Standard_Boolean BRepTopAdaptor_TopolTool::IsThePointOn(const gp_Pnt2d& P,
							  const Standard_Real Tol,
							  const Standard_Boolean RecadreOnPeriodic)
{
  if(myFClass2d == NULL) { 
    myFClass2d = (void *) new BRepTopAdaptor_FClass2d(myFace,Tol);
  } 
  return(TopAbs_ON==((BRepTopAdaptor_FClass2d *)myFClass2d)->TestOnRestriction(P,Tol,RecadreOnPeriodic));
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
  void BRepTopAdaptor_TopolTool::Destroy() 
{ 
  if(myFClass2d != NULL) { 
    delete (BRepTopAdaptor_FClass2d *)myFClass2d;
    myFClass2d=NULL;
  }
}
//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================
  TopAbs_Orientation BRepTopAdaptor_TopolTool::Orientation  (const Handle(Adaptor2d_HCurve2d)& C)
{
  Handle(BRepAdaptor_HCurve2d) brhc =
    Handle(BRepAdaptor_HCurve2d)::DownCast(C);
  return ((BRepAdaptor_Curve2d *)&(brhc->Curve2d()))->Edge().Orientation(); 
}
//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================
  TopAbs_Orientation BRepTopAdaptor_TopolTool::Orientation  (const Handle(Adaptor3d_HVertex)& C)
{
 return Adaptor3d_TopolTool::Orientation(C); 
}
//-- ============================================================
//-- methods  used for samples
//-- ============================================================ 

//=======================================================================
//function : Analyse
//purpose  : 
//=======================================================================
void Analyse(const TColgp_Array2OfPnt& array2,
	     const Standard_Integer nbup,
	     const Standard_Integer nbvp,
	     Standard_Integer& myNbSamplesU,
	     Standard_Integer& myNbSamplesV) 
{ 
  gp_Vec Vi,Vip1;
  Standard_Integer sh,nbch,i,j;
  
  sh = 1;
  nbch = 0;
  if(nbvp>2) { 
    for(i=2;i<nbup;i++) { 
      const gp_Pnt& A=array2.Value(i,1);
      const gp_Pnt& B=array2.Value(i,2);
      const gp_Pnt& C=array2.Value(i,3);
      Vi.SetCoord(C.X()-B.X()-B.X()+A.X(),
		  C.Y()-B.Y()-B.Y()+A.Y(),
		  C.Z()-B.Z()-B.Z()+A.Z());
      Standard_Integer locnbch=0;
      for(j=3; j<nbvp;j++) {  //-- test
	const gp_Pnt& Ax=array2.Value(i,j-1);
	const gp_Pnt& Bx=array2.Value(i,j);
	const gp_Pnt& Cx=array2.Value(i,j+1);
	Vip1.SetCoord(Cx.X()-Bx.X()-Bx.X()+Ax.X(),
		      Cx.Y()-Bx.Y()-Bx.Y()+Ax.Y(),
		      Cx.Z()-Bx.Z()-Bx.Z()+Ax.Z());
	Standard_Real pd = Vi.Dot(Vip1);
	Vi=Vip1;
	if(pd>1.0e-7 || pd<-1.0e-7) {  
	  if(pd>0) {  if(sh==-1) {   sh=1; locnbch++; 	}  }
	  else { 	if(sh==1) {  sh=-1; locnbch++; 	}  }
	}
      }
      if(locnbch>nbch) { 
	nbch=locnbch; 
      }
    }
  }
  myNbSamplesV = nbch+5;
  

  nbch=0;
  if(nbup>2) { 
    for(j=2;j<nbvp;j++) { 
      const gp_Pnt& A=array2.Value(1,j);
      const gp_Pnt& B=array2.Value(2,j);
      const gp_Pnt& C=array2.Value(3,j);
      Vi.SetCoord(C.X()-B.X()-B.X()+A.X(),
		  C.Y()-B.Y()-B.Y()+A.Y(),
		  C.Z()-B.Z()-B.Z()+A.Z());
      Standard_Integer locnbch=0;
      for(i=3; i<nbup;i++) {  //-- test
	const gp_Pnt& Ax=array2.Value(i-1,j);
	const gp_Pnt& Bx=array2.Value(i,j);
	const gp_Pnt& Cx=array2.Value(i+1,j);
	Vip1.SetCoord(Cx.X()-Bx.X()-Bx.X()+Ax.X(),
		    Cx.Y()-Bx.Y()-Bx.Y()+Ax.Y(),
		    Cx.Z()-Bx.Z()-Bx.Z()+Ax.Z());
	Standard_Real pd = Vi.Dot(Vip1);
	Vi=Vip1;
	if(pd>1.0e-7 || pd<-1.0e-7) {  
	  if(pd>0) {  if(sh==-1) {   sh=1; locnbch++; 	}  }
	  else { 	if(sh==1) {  sh=-1; locnbch++; 	}  }
	}
      }
      if(locnbch>nbch) nbch=locnbch;
    }  
  }
  myNbSamplesU = nbch+5;
}  




//=======================================================================
//function : ComputeSamplePoints
//purpose  : 
//=======================================================================
  void BRepTopAdaptor_TopolTool::ComputeSamplePoints() 
{ 
  Standard_Real uinf,usup,vinf,vsup;
  uinf = myS->FirstUParameter();  usup = myS->LastUParameter();
  vinf = myS->FirstVParameter();  vsup = myS->LastVParameter();
  if (usup < uinf) { Standard_Real temp=uinf; uinf=usup; usup=temp; }
  if (vsup < vinf) { Standard_Real temp=vinf; vinf=vsup; vsup=temp; }
  if (uinf == RealFirst() && usup == RealLast()) { uinf=-1.e5; usup=1.e5; }
  else if (uinf == RealFirst()) { uinf=usup-2.e5; }
  else if (usup == RealLast()) {  usup=uinf+2.e5; }
  
  if (vinf == RealFirst() && vsup == RealLast()) { vinf=-1.e5; vsup=1.e5; }
  else if (vinf == RealFirst()) { vinf=vsup-2.e5;  }
  else if (vsup == RealLast()) {  vsup=vinf+2.e5;  }
  
  Standard_Integer nbsu,nbsv;
  GeomAbs_SurfaceType typS = myS->GetType();
  switch(typS) { 
  case GeomAbs_Plane:          { nbsv=2; nbsu=2; } break;
  case GeomAbs_BezierSurface:  { nbsv=3+myS->NbVPoles(); nbsu=3+myS->NbUPoles();  } break;
  case GeomAbs_BSplineSurface: {
    nbsv = myS->NbVKnots();     nbsv*= myS->VDegree();     if(nbsv < 4) nbsv=4;    
    nbsu = myS->NbUKnots();     nbsu*= myS->UDegree();     if(nbsu < 4) nbsu=4;
  }
    break;
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
  case GeomAbs_Torus:                 { 
    //-- Set 15 for 2pi
    //-- Not enough ->25 for 2pi
    nbsu = (Standard_Integer)(8*(usup-uinf));
    nbsv = (Standard_Integer)(7*(vsup-vinf));
    if(nbsu<5) nbsu=5;
    if(nbsv<5) nbsv=5;
    if(nbsu>30) nbsu=30; //modif HRT buc60462
    if(nbsv>15) nbsv=15;
    //-- printf("\n nbsu=%d nbsv=%d\n",nbsu,nbsv);
  }     break;
  case GeomAbs_SurfaceOfRevolution:
  case GeomAbs_SurfaceOfExtrusion:    { nbsv = 15; nbsu=25; }     break;
  default:                            { nbsu = 10; nbsv=10; }    break;
  }
  
  //-- If the number of points is too great, analyze 
  //-- 
  //-- 
  
  if(nbsu<10) nbsu=10;
  if(nbsv<10) nbsv=10;
  
  myNbSamplesU = nbsu;
  myNbSamplesV = nbsv;
  
  //-- printf("\n BRepTopAdaptor_TopolTool NbSu=%d NbSv=%d ",nbsu,nbsv);
  if(nbsu>10 || nbsv>10) { 
    if(typS == GeomAbs_BSplineSurface) { 
      const Handle(Geom_BSplineSurface)& Bspl = myS->BSpline();
      Standard_Integer nbup = Bspl->NbUPoles();
      Standard_Integer nbvp = Bspl->NbVPoles();
      TColgp_Array2OfPnt array2(1,nbup,1,nbvp);
      Bspl->Poles(array2);
      Analyse(array2,nbup,nbvp,myNbSamplesU,myNbSamplesV);
      nbsu=myNbSamplesU;
      nbsv=myNbSamplesV;
      //-- printf("\n Apres analyse BSPline  NbSu=%d NbSv=%d ",myNbSamplesU,myNbSamplesV);
    }
    else if(typS == GeomAbs_BezierSurface) { 
      const Handle(Geom_BezierSurface)& Bez = myS->Bezier();
      Standard_Integer nbup = Bez->NbUPoles();
      Standard_Integer nbvp = Bez->NbVPoles();
      TColgp_Array2OfPnt array2(1,nbup,1,nbvp);
      Bez->Poles(array2);
      Analyse(array2,nbup,nbvp,myNbSamplesU,myNbSamplesV);
      nbsu=myNbSamplesU;
      nbsv=myNbSamplesV;
      //-- printf("\n Apres analyse Bezier  NbSu=%d NbSv=%d ",myNbSamplesU,myNbSamplesV);
    }
  }
 
  if(nbsu<10) nbsu=10;
  if(nbsv<10) nbsv=10;
  
  myNbSamplesU = nbsu;
  myNbSamplesV = nbsv; 
  
  myU0 = uinf;
  myV0 = vinf;
  
  myDU = (usup-uinf)/(myNbSamplesU+1);
  myDV = (vsup-vinf)/(myNbSamplesV+1);
}
//=======================================================================
//function : NbSamplesU
//purpose  : 
//=======================================================================
  Standard_Integer BRepTopAdaptor_TopolTool::NbSamplesU() 
{ 
  if(myNbSamplesU <0) { 
    ComputeSamplePoints();
  }
  return(myNbSamplesU);
}
//=======================================================================
//function : NbSamplesV
//purpose  : 
//=======================================================================
  Standard_Integer BRepTopAdaptor_TopolTool::NbSamplesV() 
{ 
  if(myNbSamplesU <0) { 
    ComputeSamplePoints();    
  }
  return(myNbSamplesV);
}
//=======================================================================
//function : NbSamples
//purpose  : 
//=======================================================================
  Standard_Integer BRepTopAdaptor_TopolTool::NbSamples()
{ 
  if(myNbSamplesU <0) { 
    ComputeSamplePoints();    
  }
  return(myNbSamplesU*myNbSamplesV);
}

//=======================================================================
//function : SamplePoint
//purpose  : 
//=======================================================================
  void BRepTopAdaptor_TopolTool::SamplePoint(const Standard_Integer i,
					     gp_Pnt2d& P2d,
					     gp_Pnt& P3d)
{ 
  Standard_Integer iv = 1 + i/myNbSamplesU;
  Standard_Integer iu = 1+ i-(iv-1)*myNbSamplesU;
  Standard_Real u=myU0+iu*myDU;
  Standard_Real v=myV0+iv*myDV;
  P2d.SetCoord(u,v);
  P3d=myS->Value(u,v);
}
//=======================================================================
//function : DomainIsInfinite
//purpose  : 
//=======================================================================
  Standard_Boolean BRepTopAdaptor_TopolTool::DomainIsInfinite() 
{
  Standard_Real uinf,usup,vinf,vsup;
  uinf = myS->FirstUParameter();  usup = myS->LastUParameter();
  vinf = myS->FirstVParameter();  vsup = myS->LastVParameter();
  if(Precision::IsNegativeInfinite(uinf)) return(Standard_True);
  if(Precision::IsPositiveInfinite(usup)) return(Standard_True);
  if(Precision::IsNegativeInfinite(vinf)) return(Standard_True);
  if(Precision::IsPositiveInfinite(vsup)) return(Standard_True);
  return(Standard_False);  
}

//=======================================================================
//function : Has3d
//purpose  : 
//=======================================================================

Standard_Boolean BRepTopAdaptor_TopolTool::Has3d() const
{
  return Standard_True;
}

//=======================================================================
//function : Tol3d
//purpose  : 
//=======================================================================

Standard_Real BRepTopAdaptor_TopolTool::Tol3d(const Handle(Adaptor2d_HCurve2d)& C) const
{
  Handle(BRepAdaptor_HCurve2d) brhc = Handle(BRepAdaptor_HCurve2d)::DownCast(C);
  if (brhc.IsNull())
    throw Standard_DomainError("BRepTopAdaptor_TopolTool: arc has no 3d representation");
  const BRepAdaptor_Curve2d& brc = (const BRepAdaptor_Curve2d &)brhc->Curve2d();
  const TopoDS_Edge& edge = brc.Edge();
  if (edge.IsNull())
    throw Standard_DomainError("BRepTopAdaptor_TopolTool: arc has no 3d representation");
  return BRep_Tool::Tolerance(edge);
}

//=======================================================================
//function : Tol3d
//purpose  : 
//=======================================================================

Standard_Real BRepTopAdaptor_TopolTool::Tol3d(const Handle(Adaptor3d_HVertex)& V) const
{
  Handle(BRepTopAdaptor_HVertex) brhv = Handle(BRepTopAdaptor_HVertex)::DownCast(V);
  if (brhv.IsNull())
    throw Standard_DomainError("BRepTopAdaptor_TopolTool: vertex has no 3d representation");
  const TopoDS_Vertex& ver = brhv->Vertex();
  if (ver.IsNull())
    throw Standard_DomainError("BRepTopAdaptor_TopolTool: vertex has no 3d representation");
  return BRep_Tool::Tolerance(ver);
}

//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

gp_Pnt BRepTopAdaptor_TopolTool::Pnt(const Handle(Adaptor3d_HVertex)& V) const
{
  Handle(BRepTopAdaptor_HVertex) brhv = Handle(BRepTopAdaptor_HVertex)::DownCast(V);
  if (brhv.IsNull())
    throw Standard_DomainError("BRepTopAdaptor_TopolTool: vertex has no 3d representation");
  const TopoDS_Vertex& ver = brhv->Vertex();
  if (ver.IsNull())
    throw Standard_DomainError("BRepTopAdaptor_TopolTool: vertex has no 3d representation");
  return BRep_Tool::Pnt(ver);
}
