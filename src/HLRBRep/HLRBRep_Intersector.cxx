// Created on: 1992-10-22
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef No_Exception
#define No_Exception
#endif


#include <Bnd_Box.hxx>
#include <ElCLib.hxx>
#include <gp.hxx>
#include <gp_Lin.hxx>
#include <HLRBRep_Curve.hxx>
#include <HLRBRep_CurveTool.hxx>
#include <HLRBRep_EdgeData.hxx>
#include <HLRBRep_Intersector.hxx>
#include <HLRBRep_SurfaceTool.hxx>
#include <HLRBRep_ThePolygonOfInterCSurf.hxx>
#include <HLRBRep_ThePolyhedronOfInterCSurf.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntCurveSurface_IntersectionSegment.hxx>
#include <IntImpParGen.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <IntRes2d_Position.hxx>
#include <IntRes2d_Transition.hxx>
#include <StdFail_UndefinedDerivative.hxx>

#include <stdio.h>
//#define PERF

#ifdef PERF
static Standard_Integer NbIntersCS=0;
static Standard_Integer NbIntersCSVides=0;
static Standard_Integer NbIntersAuto=0;
static Standard_Integer NbIntersSimulate=0;
 static Standard_Integer NbInters=0;
 static Standard_Integer NbIntersVides=0;
 static Standard_Integer NbInters1Segment=0;
 static Standard_Integer NbInters1Point=0;
 static Standard_Integer NbIntersNPoints=0;
 static Standard_Integer NbIntersNSegments=0;
 static Standard_Integer NbIntersPointEtSegment=0;
#endif

//=======================================================================
//function : HLRBRep_Intersector
//purpose  : 
//=======================================================================

HLRBRep_Intersector::HLRBRep_Intersector () :
myPolyhedron(NULL)
{
#ifdef PERF
  if(NbInters) { 
    printf("\n--------------------------------------");
    printf("\nNbIntersSimulate  : %6d",NbIntersSimulate);
    printf("\nNbIntersCrvSurf   : %6d",NbIntersCS);
    printf("\n      -> vide     : %6d",NbIntersCSVides);
    printf("\nNbAutoInters      : %6d\n",NbIntersAuto);
    printf("\nNbInters          : %6d",NbInters);
    printf("\n        Vides     : %6d",NbIntersVides);
    printf("\n        1 Segment : %6d",NbInters1Segment);
    printf("\n        1 Point   : %6d",NbInters1Point);
    printf("\n       >1 Point   : %6d",NbIntersNPoints);
    printf("\n       >1 Segment : %6d",NbIntersNSegments);
    printf("\n     >1 Pt et Seg : %6d",NbIntersPointEtSegment);
    printf("\n--------------------------------------\n");
  }
  NbIntersSimulate=NbIntersAuto=NbIntersCS
  =NbInters=NbIntersVides=NbInters1Segment=NbInters1Point=NbIntersNPoints
  = NbIntersNSegments=NbIntersPointEtSegment=NbIntersCSVides=0;
#endif

  // Set minimal number of samples in case of HLR polygonal intersector.
  const Standard_Integer aMinNbHLRSamples = 4;
  myIntersector.SetMinNbSamples(aMinNbHLRSamples);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void  HLRBRep_Intersector::Perform (const Standard_Address A1,
				    const Standard_Real da1,
				    const Standard_Real db1)
{
#ifdef PERF
  NbIntersAuto++;
#endif


  Standard_Address myC1 = ((HLRBRep_EdgeData*) A1)->Curve();

  myTypePerform = 1;

  gp_Pnt2d pa,pb;//,pa1,pb1;
  Standard_Real a,b,d,tol;
  Standard_ShortReal ta,tb;
  
  ((HLRBRep_EdgeData*) A1)->Status().Bounds(a,ta,b,tb);
  d = b - a;
  if (da1 != 0) a = a + d * da1;
  if (db1 != 0) b = b - d * db1;
  ((HLRBRep_Curve*)myC1)->D0(a,pa);
  ((HLRBRep_Curve*)myC1)->D0(b,pb);
  a = ((HLRBRep_Curve*)myC1)->Parameter2d(a);
  b = ((HLRBRep_Curve*)myC1)->Parameter2d(b);
  IntRes2d_Domain D1(pa,a,(Standard_Real)ta,pb,b,(Standard_Real)tb);

  //modified by jgv, 18.04.2016 for OCC27341
  //tol = (Standard_Real)(((HLRBRep_EdgeData*) A1)->Tolerance());
  tol = Precision::Confusion();
  //////////////////////////////////////////

  myIntersector.Perform(myC1,D1,tol,tol);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void  HLRBRep_Intersector::Perform (const Standard_Integer /*nA*/,
				    const Standard_Address A1,
				    const Standard_Real da1,
				    const Standard_Real db1,
				    const Standard_Integer /*nB*/,
				    const Standard_Address A2,
				    const Standard_Real da2,
				    const Standard_Real db2,
				    const Standard_Boolean EnBout)
{


//  if(EnBout) { 
//    myTypePerform=43; 
//    return;
//  }

  Standard_Address myC1 = ((HLRBRep_EdgeData*) A1)->Curve();
  Standard_Address myC2 = ((HLRBRep_EdgeData*) A2)->Curve();

  myTypePerform = 1;

  gp_Pnt2d pa1,pb1,pa2,pb2;
  gp_Vec2d va1,vb1,va2,vb2;
  Standard_Real a1,b1,a2,b2,d,dd,tol,tol1,tol2;
  Standard_ShortReal ta,tb;

  //modified by jgv, 18.04.2016 for OCC27341
  //tol1 = (Standard_Real)(((HLRBRep_EdgeData*) A1)->Tolerance());
  //tol2 = (Standard_Real)(((HLRBRep_EdgeData*) A2)->Tolerance());
  tol1 = Precision::Confusion();
  tol2 = Precision::Confusion();
  //////////////////////////////////////////
  if (tol1 > tol2) tol = tol1;
  else             tol = tol2;


  Standard_Boolean PasBon;
  Standard_Real    decalagea1=100.0;
  Standard_Real    decalagea2=100.0;
  Standard_Real    decalageb1=100.0;
  Standard_Real    decalageb2=100.0;
  do { 
    PasBon=Standard_False;
    ((HLRBRep_EdgeData*) A1)->Status().Bounds(a1,ta,b1,tb); //--   -> Parametres 3d 
    Standard_Real mtol = tol;
    if(mtol<ta) mtol=ta;
    if(mtol<tb) mtol=tb;
    d = b1 - a1;

    Standard_Real    pdist = tol;
    if(pdist<0.0000001) pdist = 0.0000001;
    

    if (da1 != 0)  { 
      //-- a = a + d * da1;
      ((HLRBRep_Curve*)myC1)->D1(a1,pa1,va1);
      Standard_Real qwe=va1.Magnitude();
      if(qwe>1e-12) { 
	dd=pdist*decalagea1/qwe;
	if(dd<d*0.4) { 
	  a1+=dd;
	}
	else { 
	  a1+= d * da1; decalagea1=-1;
	}
      }
      else { 
	a1+=d * da1;  decalagea1=-1;
      }
    }
    
    if (db1 != 0) { 
      //-- b = b - d * db1;
      ((HLRBRep_Curve*)myC1)->D1(b1,pb1,vb1);
      Standard_Real qwe=vb1.Magnitude();
      if(qwe>1e-12) { 
	dd=pdist*decalageb1/qwe;
	if(dd<d*0.4) { 
	  b1-=dd;
	}
	else { 
	  b1-= d * db1; decalageb1=-1;
	}
      }
      else { 
	b1-=d * db1; decalageb1=-1;
      }
    }


//    if(EnBout) {  //-- ************************************************************
//      Standard_Real d=b1-a1;
//      a1+=d*0.45;
//      b1-=d*0.45;
//    }


    
    ((HLRBRep_Curve*)myC1)->D0(a1,pa1);
    ((HLRBRep_Curve*)myC1)->D0(b1,pb1);
    
    a1 = ((HLRBRep_Curve*)myC1)->Parameter2d(a1);
    b1 = ((HLRBRep_Curve*)myC1)->Parameter2d(b1);
    
    if(EnBout) { 
      ta=tb=-1.;
    }
    
    if(ta>tol) ta=(Standard_ShortReal) tol;
    if(tb>tol) tb=(Standard_ShortReal) tol;
    
    IntRes2d_Domain D1(pa1,a1,(Standard_Real)ta,pb1,b1,(Standard_Real)tb);  
    
    ((HLRBRep_EdgeData*) A2)->Status().Bounds(a2,ta,b2,tb);
    mtol = tol;
    if(mtol<ta) mtol=ta;
    if(mtol<tb) mtol=tb;
    
    d = b2-a2;
    
    if (da2 != 0)  { 
      //-- a = a + d * da2;
      ((HLRBRep_Curve*)myC2)->D1(a2,pa2,va2);
      Standard_Real qwe=va2.Magnitude();
      if(qwe>1e-12) { 
	dd=pdist*decalagea2/qwe;
	if(dd<d*0.4) { 
	  a2+=dd;
	}
	else { 
	  a2+= d * da2;	  decalagea2=-1;
	}
      }
      else { 
	a2+=d * da2; 	  decalagea2=-1;
      }
    }
    
    if (db2 != 0) { 
      //-- b = b - d * db2;
      ((HLRBRep_Curve*)myC2)->D1(b2,pb2,vb2);
      Standard_Real qwe=vb2.Magnitude();
      if(qwe>1e-12) { 
	dd=pdist*decalageb2/qwe;
	if(dd<d*0.4) { 
	  b2-=dd;
	}
	else { 
	  b2-= d * db2; 	  decalageb2=-1;
	}
      }
      else { 
	b2-=d * db2; 	  decalageb2=-1;
      }
    }



//    if(EnBout) { //-- ************************************************************
//      Standard_Real d=b2-a2;
//      a2+=d*0.45;
//      b2-=d*0.45;
//    }





    
    ((HLRBRep_Curve*)myC2)->D0(a2,pa2);
    ((HLRBRep_Curve*)myC2)->D0(b2,pb2);
    
    a2 = ((HLRBRep_Curve*)myC2)->Parameter2d(a2);
    b2 = ((HLRBRep_Curve*)myC2)->Parameter2d(b2);
    
    if(EnBout) { 
      ta=tb=-1.;
    }
    
    if(ta>tol) ta=(Standard_ShortReal) tol;
    if(tb>tol) tb=(Standard_ShortReal) tol;
    
    IntRes2d_Domain D2(pa2,a2,(Standard_Real)ta,pb2,b2,(Standard_Real)tb);
    
    
    if(EnBout) { 
      Standard_Real a1a2 = (da1 || da2)? pa1.Distance(pa2) : RealLast();
      Standard_Real a1b2 = (da1 || db2)? pa1.Distance(pb2) : RealLast();
      Standard_Real b1a2 = (db1 || da2)? pb1.Distance(pa2) : RealLast();
      Standard_Real b1b2 = (db1 || db2)? pb1.Distance(pb2) : RealLast();
      
      Standard_Integer cote=1;
      Standard_Real mindist = a1a2;      //-- cas 1 
      if(mindist>a1b2) { mindist = a1b2; cote=2; } 
      if(mindist>b1a2) { mindist = b1a2; cote=3; } 
      if(mindist>b1b2) { mindist = b1b2; cote=4; } 

      

      //--printf("\n----- Edge %3d  %3d   [%7.5g  %7.5g] [%7.5g  %7.5g] Mindist:%8.5g   1000*Tol:%8.5g\n",
      //--     nA,nB,decalagea1,decalageb1,decalagea2,decalageb2,mindist,1000.0*tol);
      
      
      if(mindist < tol*1000) { 
	PasBon=Standard_True;
	switch (cote) { 
	case 1:  { decalagea1*=2; decalagea2*=2; break; } 
	case 2:  { decalagea1*=2; decalageb2*=2; break; } 
	case 3:  { decalageb1*=2; decalagea2*=2; break; } 
	default: { decalageb1*=2; decalageb2*=2; break; } 
	}
	if(decalagea1<0.0 || decalagea2<0.0 || decalageb1<0.0 || decalageb2<=0.0) { 
	  PasBon=Standard_False; 
	}
      }
    }
    if(PasBon==Standard_False) { 
      myIntersector.Perform(myC1,D1,myC2,D2,tol,tol);
    }
  }
  while(PasBon);
  

  
  
#ifdef PERF
  NbInters++;
  if(myIntersector.NbPoints()==1) { 
    if(myIntersector.NbSegments()==0) { 
      NbInters1Point++;
    }
    else { 
      NbIntersPointEtSegment++;
    }
  }
  else if(myIntersector.NbPoints()==0) { 
    if(myIntersector.NbSegments()==0) {
      NbIntersVides++;
    }
    else if(myIntersector.NbSegments()==1) {
      NbInters1Segment++;
    }
    else { 
      NbIntersNSegments++;
    }
  }
  else {
    if(myIntersector.NbSegments()==0) { 
      NbIntersNPoints++;
    }
    else { 
      NbIntersPointEtSegment++;
    }
  } 
#endif
}

//=======================================================================
//function : SimulateOnePoint
//purpose  : 
//=======================================================================

void  HLRBRep_Intersector::SimulateOnePoint(const Standard_Address A1,
					    const Standard_Real    u,
					    const Standard_Address A2,
					    const Standard_Real    v) { 
#ifdef PERF
  NbIntersSimulate++;
#endif
  Standard_Address myC1 = ((HLRBRep_EdgeData*) A1)->Curve();
  Standard_Address myC2 = ((HLRBRep_EdgeData*) A2)->Curve();

  Standard_Real u3= ((HLRBRep_Curve*)myC1)->Parameter3d(u);
  Standard_Real v3= ((HLRBRep_Curve*)myC2)->Parameter3d(v);
  gp_Pnt2d P13,P23;   
  gp_Vec2d T13,T23;
  ((HLRBRep_Curve*)myC1)->D1(u3,P13,T13);
  ((HLRBRep_Curve*)myC2)->D1(v3,P23,T23);
  
  IntRes2d_Transition Tr1,Tr2;
  IntRes2d_Position Pos1,Pos2;
  Pos1=Pos2=IntRes2d_Middle;
  
  IntImpParGen::DetermineTransition(Pos1,T13,Tr1,Pos2,T23,Tr2,0.0);
  myTypePerform = 0;
  mySinglePoint.SetValues(P13,u,v,Tr1,Tr2,Standard_False);
}



//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void  HLRBRep_Intersector::Load (Standard_Address& A)
{
  mySurface = A;
  if (myPolyhedron != NULL) { 
    delete myPolyhedron;
    myPolyhedron = NULL;
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void  HLRBRep_Intersector::Perform (const gp_Lin& L,
				    const Standard_Real P)
{
  myTypePerform = 2;
  GeomAbs_SurfaceType typ = HLRBRep_SurfaceTool::GetType(mySurface);
  switch (typ) {
  case GeomAbs_Plane :
  case GeomAbs_Cylinder :
  case GeomAbs_Cone :
  case GeomAbs_Sphere :
  case GeomAbs_Torus :
    myCSIntersector.Perform(L,mySurface);
    break;
    default :
    {
      if (myPolyhedron == NULL) {
	Standard_Integer nbsu,nbsv;
	Standard_Real u1,v1,u2,v2;
	u1   = HLRBRep_SurfaceTool::FirstUParameter(mySurface);
	v1   = HLRBRep_SurfaceTool::FirstVParameter(mySurface);
	u2   = HLRBRep_SurfaceTool::LastUParameter(mySurface);
	v2   = HLRBRep_SurfaceTool::LastVParameter(mySurface);   
	nbsu = HLRBRep_SurfaceTool::NbSamplesU(mySurface,u1,u2);
	nbsv = HLRBRep_SurfaceTool::NbSamplesV(mySurface,v1,v2);
	myPolyhedron =
    new HLRBRep_ThePolyhedronOfInterCSurf(mySurface,nbsu,nbsv,u1,v1,u2,v2);
      }
      Standard_Real x0,y0,z0,x1,y1,z1,pmin,pmax;//,pp;
      myPolyhedron->Bounding().Get(x0,y0,z0,x1,y1,z1);
#if 0
      pmax = pmin = ElCLib::Parameter(L, gp_Pnt((x1+x0)*0.5,
						(y1+y0)*0.5,
						(z1+z0)*0.5));
      Standard_Real d = (x1-x0) + (y1-y0) + (z1-z0);
      pmin -= d;
      pmax += d;
      if (pmin > P) pmin = P - d;
      if (pmax > P) pmax = P;
      HLRBRep_ThePolygonOfInterCSurf Polygon(L,pmin,pmax,3);
      myCSIntersector.Perform(L,Polygon,mySurface,
			      *((HLRBRep_ThePolyhedronOfInterCSurf*)
				myPolyhedron));
      break;
#else 
      //-- On va rejeter tous les points de parametres > P
      Standard_Real p;
      p = ElCLib::Parameter(L, gp_Pnt(x0,y0,z0));  pmin=pmax=p;
      p = ElCLib::Parameter(L, gp_Pnt(x0,y0,z1));  if(pmin>p) pmin=p; if(pmax<p) pmax=p;

      p = ElCLib::Parameter(L, gp_Pnt(x1,y0,z0));  if(pmin>p) pmin=p; if(pmax<p) pmax=p;
      p = ElCLib::Parameter(L, gp_Pnt(x1,y0,z1));  if(pmin>p) pmin=p; if(pmax<p) pmax=p;

      p = ElCLib::Parameter(L, gp_Pnt(x0,y1,z0));  if(pmin>p) pmin=p; if(pmax<p) pmax=p;
      p = ElCLib::Parameter(L, gp_Pnt(x0,y1,z1));  if(pmin>p) pmin=p; if(pmax<p) pmax=p;

      p = ElCLib::Parameter(L, gp_Pnt(x1,y1,z0));  if(pmin>p) pmin=p; if(pmax<p) pmax=p;
      p = ElCLib::Parameter(L, gp_Pnt(x1,y1,z1));  if(pmin>p) pmin=p; if(pmax<p) pmax=p;
      pmin-=0.000001; pmax+=0.000001;

      if(pmin>P) { pmin=pmax+1; pmax=pmax+2; } //-- on va rejeter avec les boites
      else { 
	if(pmax>P) pmax=P+0.0000001;
      }
      HLRBRep_ThePolygonOfInterCSurf Polygon(L,pmin,pmax,3);
      myCSIntersector.Perform(L,Polygon,mySurface,
			      *((HLRBRep_ThePolyhedronOfInterCSurf*)
				myPolyhedron));
      
      break;
#endif

    }
  }
#ifdef PERF
  NbIntersCS++;
  if(myCSIntersector.NbPoints()==0) { 
    NbIntersCSVides++;
  }
#endif
 
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean HLRBRep_Intersector::IsDone () const
{ 
  if(myTypePerform == 1) 
    return myIntersector  .IsDone();
  else if(myTypePerform ==2) 
    return myCSIntersector.IsDone();
  else 
    return(Standard_True);
}

//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================

Standard_Integer HLRBRep_Intersector::NbPoints() const
{
  if(myTypePerform==43) return(0);

  if (myTypePerform == 1) 
    return myIntersector  .NbPoints();
  else if(myTypePerform == 2) 
    return myCSIntersector.NbPoints();
  else 
    return(1);
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

const IntRes2d_IntersectionPoint &
HLRBRep_Intersector::Point (const Standard_Integer N) const
{
  if(myTypePerform==0) 
    return(mySinglePoint);
  else 
    return myIntersector.Point(N);
}

//=======================================================================
//function : CSPoint
//purpose  : 
//=======================================================================

const IntCurveSurface_IntersectionPoint &
 HLRBRep_Intersector::CSPoint (const Standard_Integer N) const
{
  return myCSIntersector.Point(N);
}

//=======================================================================
//function : NbSegments
//purpose  : 
//=======================================================================

Standard_Integer HLRBRep_Intersector::NbSegments () const
{
  if(myTypePerform == 1) 
    return myIntersector  .NbSegments();
  else if(myTypePerform==2) 
    return myCSIntersector.NbSegments();
  else 
    return(0);
}

//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

const IntRes2d_IntersectionSegment &
HLRBRep_Intersector::Segment (const Standard_Integer N) const
{
  return myIntersector .Segment(N);
}

//=======================================================================
//function : CSSegment
//purpose  : 
//=======================================================================

const IntCurveSurface_IntersectionSegment & 
HLRBRep_Intersector::CSSegment (const Standard_Integer N) const
{
  return myCSIntersector.Segment(N);
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void HLRBRep_Intersector::Destroy ()
{
  if (myPolyhedron != NULL)
    delete myPolyhedron;
}










/* ********************************************************************************
   
   sauvegarde de l etat du 23 janvier 98 
   
   
void  HLRBRep_Intersector::Perform (const Standard_Integer nA,
				    const Standard_Address A1,
				    const Standard_Real da1,
				    const Standard_Real db1,
				    const Standard_Integer nB,
				    const Standard_Address A2,
				    const Standard_Real da2,
				    const Standard_Real db2,
				    const Standard_Boolean EnBout)
{
  Standard_Address myC1 = ((HLRBRep_EdgeData*) A1)->Curve();
  Standard_Address myC2 = ((HLRBRep_EdgeData*) A2)->Curve();

  myTypePerform = 1;

  gp_Pnt2d pa,pb;
  Standard_Real a,b,d,tol,tol1,tol2;
  Standard_ShortReal ta,tb;

  tol1 = (Standard_Real)(((HLRBRep_EdgeData*) A1)->Tolerance());
  tol2 = (Standard_Real)(((HLRBRep_EdgeData*) A2)->Tolerance());
  if (tol1 > tol2) tol = tol1;
  else             tol = tol2;

  ((HLRBRep_EdgeData*) A1)->Status().Bounds(a,ta,b,tb); //--   -> Parametres 3d 
  Standard_Real mtol = tol;
  if(mtol<ta) mtol=ta;
  if(mtol<tb) mtol=tb;
  d = b - a;
  if (da1 != 0) a = a + d * da1;
  if (db1 != 0) b = b - d * db1;
  ((HLRBRep_Curve*)myC1)->D0(a,pa);
  ((HLRBRep_Curve*)myC1)->D0(b,pb);

  a = ((HLRBRep_Curve*)myC1)->Parameter2d(a);
  b = ((HLRBRep_Curve*)myC1)->Parameter2d(b);

  if(EnBout) { 
    ta=tb=0;
  }
  
  if(ta>tol) ta=tol;
  if(tb>tol) tb=tol;


  IntRes2d_Domain D1(pa,a,(Standard_Real)ta,pb,b,(Standard_Real)tb);  

  ((HLRBRep_EdgeData*) A2)->Status().Bounds(a,ta,b,tb);
  mtol = tol;
  if(mtol<ta) mtol=ta;
  if(mtol<tb) mtol=tb;

  d = b - a;
  if (da2 != 0) a = a + d * da2;
  if (db2 != 0) b = b - d * db2;
  ((HLRBRep_Curve*)myC2)->D0(a,pa);
  ((HLRBRep_Curve*)myC2)->D0(b,pb);

  a = ((HLRBRep_Curve*)myC2)->Parameter2d(a);
  b = ((HLRBRep_Curve*)myC2)->Parameter2d(b);
 
  if(EnBout) { 
    ta=tb=0;
  }

  if(ta>tol) ta=tol;
  if(tb>tol) tb=tol;


 IntRes2d_Domain D2(pa,a,(Standard_Real)ta,pb,b,(Standard_Real)tb);

  myIntersector.Perform(myC1,D1,myC2,D2,tol,tol);
  
  
  
  
  
#ifdef PERF
  NbInters++;
  if(myIntersector.NbPoints()==1) { 
    if(myIntersector.NbSegments()==0) { 
      NbInters1Point++;
    }
    else { 
      NbIntersPointEtSegment++;
    }
  }
  else if(myIntersector.NbPoints()==0) { 
    if(myIntersector.NbSegments()==0) {
      NbIntersVides++;
    }
    else if(myIntersector.NbSegments()==1) {
      NbInters1Segment++;
    }
    else { 
      NbIntersNSegments++;
    }
  }
  else {
    if(myIntersector.NbSegments()==0) { 
      NbIntersNPoints++;
    }
    else { 
      NbIntersPointEtSegment++;
    }
  } 
#endif
}
******************************************************************************** */
