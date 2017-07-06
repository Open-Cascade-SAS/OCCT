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
#include <Adaptor2d_HLine2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Cone.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Adaptor3d_TopolTool,Standard_Transient)

#define myInfinite Precision::Infinite()

static void GetConeApexParam(const gp_Cone& C, Standard_Real& U, Standard_Real& V)
{
  const gp_Ax3& Pos = C.Position();
  Standard_Real Radius = C.RefRadius();
  Standard_Real SAngle = C.SemiAngle();
  const gp_Pnt& P = C.Apex();
 
  gp_Trsf T;
  T.SetTransformation (Pos);
  gp_Pnt Ploc = P.Transformed (T);

  if(Ploc.X() ==0.0  &&  Ploc.Y()==0.0 ) {
    U = 0.0;
  }
  else if ( -Radius > Ploc.Z()* Tan(SAngle) ) {
    // the point is at the `wrong` side of the apex
    U = atan2(-Ploc.Y(), -Ploc.X());
  }
  else {
    U = atan2(Ploc.Y(),Ploc.X());
  }
  if      (U < -1.e-16)  U += (M_PI+M_PI);
  else if (U < 0)        U = 0;

  V =  sin(SAngle) * ( Ploc.X() * cos(U) + Ploc.Y() * sin(U) - Radius)
    + cos(SAngle) * Ploc.Z();
}


Adaptor3d_TopolTool::Adaptor3d_TopolTool () : myNbSamplesU(-1),nbRestr(0),idRestr(0)
                                              
{
}

Adaptor3d_TopolTool::Adaptor3d_TopolTool (const Handle(Adaptor3d_HSurface)& S)
{
  Initialize(S);
}


void Adaptor3d_TopolTool::Initialize ()
{
 throw Standard_NotImplemented("Adaptor3d_TopolTool::Initialize ()");
}

void Adaptor3d_TopolTool::Initialize (const Handle(Adaptor3d_HSurface)& S)
{
  Standard_Real pinf,psup,deltap;
  //Adaptor2d_Line2d  * Line2dPtr ;

  myNbSamplesU=-1;
  Uinf = S->FirstUParameter(); // where UIntervalFirst ??
  Vinf = S->FirstVParameter();
  Usup = S->LastUParameter();
  Vsup = S->LastVParameter();
  nbRestr = 0;
  idRestr = 0;

  Standard_Boolean Uinfinfinite = Precision::IsNegativeInfinite(Uinf);
  Standard_Boolean Usupinfinite = Precision::IsPositiveInfinite(Usup);
  Standard_Boolean Vinfinfinite = Precision::IsNegativeInfinite(Vinf);
  Standard_Boolean Vsupinfinite = Precision::IsPositiveInfinite(Vsup);

  if (! Vinfinfinite) {
    deltap = Min(Usup-Uinf,2.*myInfinite);
    if (Uinf >= -myInfinite){
      pinf = Uinf;
      psup = pinf + deltap;
    }
    else if (Usup <= myInfinite) {
      psup = Usup;
      pinf = psup - deltap;
    }
    else {
      pinf = -myInfinite;
      psup = myInfinite;
    }

    // Line2dPtr = new Adaptor2d_Line2d(gp_Pnt2d(0.,Vinf),gp_Dir2d(1.,0.),pinf,psup);
    //myRestr[nbRestr] = new Adaptor2d_HLine2d(*Line2dPtr);
    myRestr[nbRestr] = new Adaptor2d_HLine2d(Adaptor2d_Line2d(gp_Pnt2d(0.,Vinf),gp_Dir2d(1.,0.),pinf,psup));
    nbRestr++;
  }

  if (!Usupinfinite) {
    deltap = Min(Vsup-Vinf,2.*myInfinite);
    if (Vinf >= -myInfinite){
      pinf = Vinf;
      psup = pinf + deltap;
    }
    else if (Vsup <= myInfinite) {
      psup = Vsup;
      pinf = psup - deltap;
    }
    else {
      pinf = -myInfinite;
      psup = myInfinite;
    }


    
    //Line2dPtr = new Adaptor2d_Line2d(gp_Pnt2d(Usup,0.),gp_Dir2d(0.,1.),pinf,psup);
    //myRestr[nbRestr] = new Adaptor2d_HLine2d(*Line2dPtr);
    myRestr[nbRestr] = new Adaptor2d_HLine2d(Adaptor2d_Line2d(gp_Pnt2d(Usup,0.),gp_Dir2d(0.,1.),pinf,psup));
    nbRestr++;
  }

  if (!Vsupinfinite) {
    deltap = Min(Usup-Uinf,2.*myInfinite);
    if (-Usup >= -myInfinite){
      pinf = -Usup;
      psup = pinf + deltap;
    }
    else if (-Uinf <= myInfinite) {
      psup = -Uinf;
      pinf = psup - deltap;
    }
    else {
      pinf = -myInfinite;
      psup = myInfinite;
    }

    
    //Line2dPtr = new Adaptor2d_Line2d(gp_Pnt2d(0.,Vsup),gp_Dir2d(-1.,0.),pinf,psup);
    //myRestr[nbRestr] = new Adaptor2d_HLine2d(*Line2dPtr);
    myRestr[nbRestr] = new Adaptor2d_HLine2d(Adaptor2d_Line2d(gp_Pnt2d(0.,Vsup),gp_Dir2d(-1.,0.),pinf,psup));
    nbRestr++;
  }

  if (!Uinfinfinite) {
    deltap = Min(Vsup-Vinf,2.*myInfinite);
    if (-Vsup >= -myInfinite){
      pinf = -Vsup;
      psup = pinf + deltap;
    }
    else if (-Vinf <= myInfinite) {
      psup = -Vinf;
      pinf = psup - deltap;
    }
    else {
      pinf = -myInfinite;
      psup = myInfinite;
    }

    //Line2dPtr = new Adaptor2d_Line2d(gp_Pnt2d(Uinf,0.),gp_Dir2d(0.,-1),pinf,psup);
    //myRestr[nbRestr] = new Adaptor2d_HLine2d(*Line2dPtr);
    myRestr[nbRestr] = new Adaptor2d_HLine2d(Adaptor2d_Line2d(gp_Pnt2d(Uinf,0.),gp_Dir2d(0.,-1),pinf,psup));
    nbRestr++;
  }

  myS = S;

  if(nbRestr == 2 && S->GetType() == GeomAbs_Cone ) {
    Standard_Real U = 0., V = 0.;
    GetConeApexParam(S->Cone(),U,V);

    deltap = Min(Usup-Uinf,2.*myInfinite);
    if (Uinf >= -myInfinite){
      pinf = Uinf;
      psup = pinf + deltap;
    }
    else if (Usup <= myInfinite) {
      psup = Usup;
      pinf = psup - deltap;
    }
    else {
      pinf = -myInfinite;
      psup = myInfinite;
    }

    //Line2dPtr = new Adaptor2d_Line2d(gp_Pnt2d(U,V),gp_Dir2d(1.,0.),pinf,psup);
    //myRestr[nbRestr] = new Adaptor2d_HLine2d(*Line2dPtr);
    myRestr[nbRestr] = new Adaptor2d_HLine2d(Adaptor2d_Line2d(gp_Pnt2d(U,V),gp_Dir2d(1.,0.),pinf,psup));
    nbRestr++;
  }
}


void Adaptor3d_TopolTool::Init ()
{
  idRestr = 0;
}


Standard_Boolean Adaptor3d_TopolTool::More ()
{
  return (idRestr < nbRestr);
}

Handle(Adaptor2d_HCurve2d) Adaptor3d_TopolTool::Value ()
{
  if (idRestr >= nbRestr) {throw Standard_DomainError();}
  return myRestr[idRestr];
}

void Adaptor3d_TopolTool::Next ()
{
  idRestr++;
}


void Adaptor3d_TopolTool::Initialize(const Handle(Adaptor2d_HCurve2d)& C)
{
  nbVtx = 0;
  idVtx = 0;
  Standard_Real theUinf,theUsup;
  theUinf = C->FirstParameter();
  theUsup = C->LastParameter();
//  if (!Precision::IsNegativeInfinite(theUinf)) {
  if (theUinf > -myInfinite) {
    myVtx[nbVtx] = new Adaptor3d_HVertex(C->Value(theUinf),TopAbs_FORWARD,1.e-8);
    nbVtx++;
  }
//  if (!Precision::IsPositiveInfinite(theUsup)) {
  if (theUsup < myInfinite) {
    myVtx[nbVtx] = new Adaptor3d_HVertex(C->Value(theUsup),TopAbs_REVERSED,1.e-8);
    nbVtx++;
  }
}


void Adaptor3d_TopolTool::InitVertexIterator ()
{
  idVtx =  0;
}


Standard_Boolean Adaptor3d_TopolTool::MoreVertex ()
{
  return (idVtx < nbVtx);
}


Handle(Adaptor3d_HVertex) Adaptor3d_TopolTool::Vertex ()
{
  if (idVtx >= nbVtx) {throw Standard_DomainError();}
  return myVtx[idVtx];
}

void Adaptor3d_TopolTool::NextVertex ()
{
  idVtx++;
}


TopAbs_State Adaptor3d_TopolTool::Classify(const gp_Pnt2d& P,
					 const Standard_Real Tol,
					 const Standard_Boolean )
//					 const Standard_Boolean RecadreOnPeriodic)
{

  Standard_Real U = P.X();
  Standard_Real V = P.Y();

  if (nbRestr == 4) {
    if ((U < Uinf - Tol) || (U > Usup + Tol) ||
      (V < Vinf - Tol) || (V > Vsup + Tol)) {
        return TopAbs_OUT;
    }
    if ((Abs(U - Uinf) <= Tol) || (Abs(U - Usup) <= Tol) ||
      (Abs(V - Vinf) <= Tol) || (Abs(V - Vsup) <= Tol)) {
        return TopAbs_ON;
    }
    return TopAbs_IN;
  }
  else if (nbRestr == 0) {
    return TopAbs_IN;
  }
  else {
    Standard_Boolean dansu,dansv,surumin,surumax,survmin,survmax;
    if (Precision::IsNegativeInfinite(Uinf) && 
	    Precision::IsPositiveInfinite(Usup)) {
      dansu = Standard_True;
      surumin = surumax = Standard_False;
    }
    else if (Precision::IsNegativeInfinite(Uinf)) {
      surumin = Standard_False;
      if (U >= Usup+Tol) {
	      dansu = Standard_False;
	      surumax = Standard_False;
      }
      else {
	      dansu = Standard_True;
	      surumax = Standard_False;
	      if (Abs(U-Usup)<=Tol) {
	        surumax = Standard_True;
	      }
      }
    }
    else if (Precision::IsPositiveInfinite(Usup)) {
      surumax = Standard_False;
      if (U < Uinf-Tol) {
        dansu = Standard_False;
        surumin = Standard_False;
      }
      else {
        dansu = Standard_True;
        surumin = Standard_False;
        if (Abs(U-Uinf)<=Tol) {
          surumin = Standard_True;
        }
      }
    }
    else {
      if ((U < Uinf - Tol) || (U > Usup + Tol)) {
        surumin = surumax = dansu = Standard_False;
      }
      else {
        dansu = Standard_True;
        surumin = surumax = Standard_False;
        if (Abs(U-Uinf)<=Tol) {
          surumin = Standard_True;
        }
        else if (Abs(U-Usup)<=Tol) {
          surumax = Standard_True;
        }
      }
    }

    if (Precision::IsNegativeInfinite(Vinf) &&
	    Precision::IsPositiveInfinite(Vsup)) {
      dansv = Standard_True;
      survmin = survmax = Standard_False;
    }
    else if (Precision::IsNegativeInfinite(Vinf)) {
      survmin = Standard_False;
      if (V > Vsup+Tol) {
        dansv = Standard_False;
        survmax = Standard_False;
      }
      else {
        dansv = Standard_True;
        survmax = Standard_False;
        if (Abs(V-Vsup)<=Tol) {
          survmax = Standard_True;
        }
      }
    }
    else if (Precision::IsPositiveInfinite(Vsup)) {
      survmax = Standard_False;
      if (V < Vinf-Tol) {
        dansv = Standard_False;
        survmin = Standard_False;
      }
      else {
        dansv = Standard_True;
        survmin = Standard_False;
        if (Abs(V-Vinf)<=Tol) {
          survmin = Standard_True;
        }
      }
    }
    else {
      if ((V < Vinf - Tol) || (V > Vsup + Tol)) {
        survmin = survmax = dansv = Standard_False;
      }
      else {
        dansv = Standard_True;
        survmin = survmax = Standard_False;
        if (Abs(V-Vinf)<=Tol) {
          survmin = Standard_True;
        }
        else if (Abs(V-Vsup)<=Tol) {
          survmax = Standard_True;
        }
      }
    }

    if (!dansu || !dansv) {
      return TopAbs_OUT;
    }
    if (surumin || survmin || surumax || survmax) {
      return TopAbs_ON;
    }
    return TopAbs_IN;
  }
}



Standard_Boolean  Adaptor3d_TopolTool::IsThePointOn(const gp_Pnt2d& P,
					     const Standard_Real Tol,
					     const Standard_Boolean )
//					     const Standard_Boolean RecadreOnPeriodic)
{

  Standard_Real U = P.X();
  Standard_Real V = P.Y();

  if (nbRestr == 4) {
    if ((U < Uinf - Tol) || (U > Usup + Tol) ||
	(V < Vinf - Tol) || (V > Vsup + Tol)) {
      return(Standard_False);
    }
    if ((Abs(U - Uinf) <= Tol) || (Abs(U - Usup) <= Tol) ||
	(Abs(V - Vinf) <= Tol) || (Abs(V - Vsup) <= Tol)) {
      return(Standard_True);
    }
    return(Standard_False);
  }
  else if (nbRestr == 0) {
    return(Standard_False);
  }
  else {
    Standard_Boolean dansu,dansv,surumin,surumax,survmin,survmax;
    if (Precision::IsNegativeInfinite(Uinf) && 
	Precision::IsPositiveInfinite(Usup)) {
      dansu = Standard_True;
      surumin = surumax = Standard_False;
    }
    else if (Precision::IsNegativeInfinite(Uinf)) {
      surumin = Standard_False;
      if (U >= Usup+Tol) {
	dansu = Standard_False;
	surumax = Standard_False;
      }
      else {
	dansu = Standard_True;
	surumax = Standard_False;
	if (Abs(U-Usup)<=Tol) {
	  surumax = Standard_True;
	}
      }
    }
    else if (Precision::IsPositiveInfinite(Usup)) {
      surumax = Standard_False;
      if (U < Uinf-Tol) {
	dansu = Standard_False;
	surumin = Standard_False;
      }
      else {
	dansu = Standard_True;
	surumin = Standard_False;
	if (Abs(U-Uinf)<=Tol) {
	  surumin = Standard_True;
	}
      }
    }
    else {
      if ((U < Uinf - Tol) || (U > Usup + Tol)) {
	surumin = surumax = dansu = Standard_False;
      }
      else {
	dansu = Standard_True;
	surumin = surumax = Standard_False;
	if (Abs(U-Uinf)<=Tol) {
	  surumin = Standard_True;
	}
	else if (Abs(U-Usup)<=Tol) {
	  surumax = Standard_True;
	}
      }
    }

    if (Precision::IsNegativeInfinite(Vinf) &&
	Precision::IsPositiveInfinite(Vsup)) {
      dansv = Standard_True;
      survmin = survmax = Standard_False;
    }
    else if (Precision::IsNegativeInfinite(Vinf)) {
      survmin = Standard_False;
      if (V > Vsup+Tol) {
	dansv = Standard_False;
	survmax = Standard_False;
      }
      else {
	dansv = Standard_True;
	survmax = Standard_False;
	if (Abs(V-Vsup)<=Tol) {
	  survmax = Standard_True;
	}
      }
    }
    else if (Precision::IsPositiveInfinite(Vsup)) {
      survmax = Standard_False;
      if (V < Vinf-Tol) {
	dansv = Standard_False;
	survmin = Standard_False;
      }
      else {
	dansv = Standard_True;
	survmin = Standard_False;
	if (Abs(V-Vinf)<=Tol) {
	  survmin = Standard_True;
	}
      }
    }
    else {
      if ((V < Vinf - Tol) || (V > Vsup + Tol)) {
	survmin = survmax = dansv = Standard_False;
      }
      else {
	dansv = Standard_True;
	survmin = survmax = Standard_False;
	if (Abs(V-Vinf)<=Tol) {
	  survmin = Standard_True;
	}
	else if (Abs(V-Vsup)<=Tol) {
	  survmax = Standard_True;
	}
      }
    }

    if (!dansu || !dansv) {
      return(Standard_False);
    }
    if (surumin || survmin || surumax || survmax) {
      return(Standard_True);
    }
    return(Standard_False);;
  }
}


TopAbs_Orientation Adaptor3d_TopolTool::Orientation
  (const Handle(Adaptor2d_HCurve2d)&)
{
  return TopAbs_FORWARD;
}

TopAbs_Orientation Adaptor3d_TopolTool::Orientation
  (const Handle(Adaptor3d_HVertex)& V)
{
  return V->Orientation();
}

Standard_Boolean Adaptor3d_TopolTool::Identical
  (const Handle(Adaptor3d_HVertex)& V1,
   const Handle(Adaptor3d_HVertex)& V2)
{
  return V1->IsSame(V2);
}


//-- ============================================================
//-- m e t h o d e s   u t i l i s e e s   p o u r   l e s  
//--  s a m p l e s
//-- ============================================================ 
#include <TColgp_Array2OfPnt.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>
//#include <gce_MakeLin.hxx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

#define myMinPnts 4 //Absolut possible minimum of sample points
                    //Restriction of IntPolyh


static void Analyse(const TColgp_Array2OfPnt& array2,
		    const Standard_Integer nbup,
		    const Standard_Integer nbvp,
		    Standard_Integer& myNbSamplesU,
		    Standard_Integer& myNbSamplesV) { 
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
      for(j=3; j<nbvp;j++) {  //-- try
	const gp_Pnt& A1=array2.Value(i,j-1);
	const gp_Pnt& B1=array2.Value(i,j);
	const gp_Pnt& C1=array2.Value(i,j+1);
	Vip1.SetCoord(C1.X()-B1.X()-B1.X()+A1.X(),
                      C1.Y()-B1.Y()-B1.Y()+A1.Y(),
                      C1.Z()-B1.Z()-B1.Z()+A1.Z());
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
      for(i=3; i<nbup;i++) {  //-- try
	const gp_Pnt& A1=array2.Value(i-1,j);
	const gp_Pnt& B1=array2.Value(i,j);
	const gp_Pnt& C1=array2.Value(i+1,j);
	Vip1.SetCoord(C1.X()-B1.X()-B1.X()+A1.X(),
                      C1.Y()-B1.Y()-B1.Y()+A1.Y(),
                      C1.Z()-B1.Z()-B1.Z()+A1.Z());
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


void Adaptor3d_TopolTool::ComputeSamplePoints() { 
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
  case GeomAbs_Torus:
  case GeomAbs_SurfaceOfRevolution:
  case GeomAbs_SurfaceOfExtrusion:    { nbsv = 15; nbsu=15; }     break;
  default:                            { nbsu = 10; nbsv=10; }    break;
  }
  
  //-- If the number of points is too great... analyze 
  //-- 
  //-- 
  
  if(nbsu<6) nbsu=6;
  if(nbsv<6) nbsv=6;
  
  myNbSamplesU = nbsu;
  myNbSamplesV = nbsv;

  if(nbsu>8 || nbsv>8) {
    if(typS == GeomAbs_BSplineSurface) { 
      const Handle(Geom_BSplineSurface)& Bspl = myS->BSpline();
      Standard_Integer nbup = Bspl->NbUPoles();
      Standard_Integer nbvp = Bspl->NbVPoles();
      TColgp_Array2OfPnt array2(1,nbup,1,nbvp);
      Bspl->Poles(array2);
      Analyse(array2,nbup,nbvp,myNbSamplesU,myNbSamplesV);
    }
    else if(typS == GeomAbs_BezierSurface) { 
      const Handle(Geom_BezierSurface)& Bez = myS->Bezier();
      Standard_Integer nbup = Bez->NbUPoles();
      Standard_Integer nbvp = Bez->NbVPoles();
      TColgp_Array2OfPnt array2(1,nbup,1,nbvp);
      Bez->Poles(array2);
      Analyse(array2,nbup,nbvp,myNbSamplesU,myNbSamplesV);
    }
  }
}

Standard_Integer Adaptor3d_TopolTool::NbSamplesU()
{ 
  if(myNbSamplesU <0) {
    ComputeSamplePoints();
  }
  return(myNbSamplesU);
}

Standard_Integer Adaptor3d_TopolTool::NbSamplesV()
{ 
  if(myNbSamplesU <0) { 
    ComputeSamplePoints();    
  }
  return(myNbSamplesV);
}

Standard_Integer Adaptor3d_TopolTool::NbSamples()
{ 
  if(myNbSamplesU <0) { 
    ComputeSamplePoints();    
  }
  return(myNbSamplesU*myNbSamplesV);
}

void Adaptor3d_TopolTool::UParameters(TColStd_Array1OfReal& theArray) const
{
  theArray = myUPars->Array1();
}

void Adaptor3d_TopolTool::VParameters(TColStd_Array1OfReal& theArray) const
{
  theArray = myVPars->Array1();
}

void Adaptor3d_TopolTool::SamplePoint(const Standard_Integer i,
				      gp_Pnt2d& P2d,
				      gp_Pnt& P3d)
{
  Standard_Integer iu, iv;
  Standard_Real u, v;
  if (myUPars.IsNull())
    {
      Standard_Real myDU=(Usup-Uinf)/(myNbSamplesU+1);
      Standard_Real myDV=(Vsup-Vinf)/(myNbSamplesV+1);
      iv = 1 + i/myNbSamplesU;
      iu = 1+ i-(iv-1)*myNbSamplesU;
      u=Uinf+iu*myDU;
      v=Vinf+iv*myDV;
    }
  else
    {
      iv = (i-1)/myNbSamplesU + 1;
      iu = (i-1)%myNbSamplesU + 1;
      u = myUPars->Value(iu);
      v = myVPars->Value(iv);
    }

  P2d.SetCoord(u,v);
  P3d = myS->Value(u,v);
}



Standard_Boolean Adaptor3d_TopolTool::DomainIsInfinite() { 
  if(Precision::IsNegativeInfinite(Uinf)) return(Standard_True);
  if(Precision::IsPositiveInfinite(Usup)) return(Standard_True);
  if(Precision::IsNegativeInfinite(Vinf)) return(Standard_True);
  if(Precision::IsPositiveInfinite(Vsup)) return(Standard_True);
  return(Standard_False);
}
//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
  Standard_Address Adaptor3d_TopolTool::Edge() const 
{
  return NULL;
} 
//=======================================================================
//function : Has3d
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_TopolTool::Has3d() const
{
  return Standard_False;
}

//=======================================================================
//function : Tol3d
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_TopolTool::Tol3d(const Handle(Adaptor2d_HCurve2d)&) const
{
  throw Standard_DomainError("Adaptor3d_TopolTool: has no 3d representation");
}

//=======================================================================
//function : Tol3d
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_TopolTool::Tol3d(const Handle(Adaptor3d_HVertex)&) const
{
  throw Standard_DomainError("Adaptor3d_TopolTool: has no 3d representation");
}

//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

gp_Pnt Adaptor3d_TopolTool::Pnt(const Handle(Adaptor3d_HVertex)&) const
{
  throw Standard_DomainError("Adaptor3d_TopolTool: has no 3d representation");
}


//=======================================================================
//function : SamplePnts
//purpose  : 
//=======================================================================

void Adaptor3d_TopolTool::SamplePnts(const Standard_Real theDefl, 
				     const Standard_Integer theNUmin,
				     const Standard_Integer theNVmin)
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
  
//   Standard_Integer nbsu,nbsv;
  GeomAbs_SurfaceType typS = myS->GetType();
//   switch(typS) { 
//   case GeomAbs_Plane:          { nbsv=2; nbsu=2; } break;
//   case GeomAbs_BezierSurface:  { 
//     nbsv=myS->NbVPoles(); 
//     nbsu=myS->NbUPoles();
//     nbsu = Max(nbsu, theNUmin);
//     nbsv = Max(nbsv, theNVmin);
//     if(nbsu>8 || nbsv>8) {
//       const Handle(Geom_BezierSurface)& Bez = myS->Bezier();
//       Standard_Integer nbup = Bez->NbUPoles();
//       Standard_Integer nbvp = Bez->NbVPoles();
//       TColgp_Array2OfPnt array2(1,nbup,1,nbvp);
//       Bez->Poles(array2);
//       Analyse(array2,nbup,nbvp,myNbSamplesU,myNbSamplesV);
//     }  
//   } 
//     break;
//   case GeomAbs_BSplineSurface: {
  if(typS == GeomAbs_BSplineSurface) {
    // Processing BSpline surface 
    BSplSamplePnts(theDefl, theNUmin, theNVmin);
    return;
  }
  else {
    ComputeSamplePoints();
  }
//   case GeomAbs_Cylinder:
//   case GeomAbs_Cone:
//   case GeomAbs_Sphere:
//   case GeomAbs_Torus:
//   case GeomAbs_SurfaceOfRevolution:
//   case GeomAbs_SurfaceOfExtrusion:    { nbsv = Max(15,theNVmin); nbsu=Max(15,theNUmin); } break;
//   default:                            { nbsu = Max(10,theNUmin); nbsv=Max(10,theNVmin); } break;
//   }
  
  
//   if(nbsu<6) nbsu=6;
//   if(nbsv<6) nbsv=6;
  
//   myNbSamplesU = nbsu;
//   myNbSamplesV = nbsv;


  myUPars = new TColStd_HArray1OfReal(1, myNbSamplesU);
  myVPars = new TColStd_HArray1OfReal(1, myNbSamplesV);
  Standard_Integer i;
  Standard_Real t, dt = (usup - uinf)/(myNbSamplesU - 1);
  myUPars->SetValue(1, uinf);
  myUPars->SetValue(myNbSamplesU, usup);
  for(i = 2, t = uinf+dt; i < myNbSamplesU; ++i, t += dt) {
    myUPars->SetValue(i, t);
  }

  dt = (vsup - vinf)/(myNbSamplesV - 1);
  myVPars->SetValue(1, vinf);
  myVPars->SetValue(myNbSamplesV, vsup);
  for(i = 2, t = vinf+dt; i < myNbSamplesV; ++i, t += dt) {
    myVPars->SetValue(i, t);
  }

  return;

}

//=======================================================================
//function : BSplSamplePnts
//purpose  : 
//=======================================================================

void Adaptor3d_TopolTool::BSplSamplePnts(const Standard_Real theDefl, 
					 const Standard_Integer theNUmin,
					 const Standard_Integer theNVmin)
{ 
  const Handle(Geom_BSplineSurface)& aBS = myS->BSpline();
  Standard_Real uinf,usup,vinf,vsup;
  uinf = myS->FirstUParameter();  usup = myS->LastUParameter();
  vinf = myS->FirstVParameter();  vsup = myS->LastVParameter();

  Standard_Integer i, k, j = 1;
  Standard_Real t1, t2, dt;
  Standard_Integer ui1 = aBS->FirstUKnotIndex();
  Standard_Integer ui2 = aBS->LastUKnotIndex();
  Standard_Integer vi1 = aBS->FirstVKnotIndex();
  Standard_Integer vi2 = aBS->LastVKnotIndex();

  for(i = ui1; i < ui2; ++i) {
    if(uinf >= aBS->UKnot(i) && uinf < aBS->UKnot(i+1)) {
      ui1 = i;
      break;
    }
  }

  for(i = ui2; i > ui1; --i) {
    if(usup <= aBS->UKnot(i) && usup > aBS->UKnot(i-1)) {
      ui2 = i;
      break;
    }
  }

  for(i = vi1; i < vi2; ++i) {
    if(vinf >= aBS->VKnot(i) && vinf < aBS->VKnot(i+1)) {
      vi1 = i;
      break;
    }
  }

  for(i = vi2; i > vi1; --i) {
    if(vsup <= aBS->VKnot(i) && vsup > aBS->VKnot(i-1)) {
      vi2 = i;
      break;
    }
  }

  Standard_Integer nbsu = ui2-ui1+1; nbsu += (nbsu - 1) * (aBS->UDegree()-1);
  Standard_Integer nbsv = vi2-vi1+1; nbsv += (nbsv - 1) * (aBS->VDegree()-1);
  Standard_Boolean bUuniform = Standard_False;
  Standard_Boolean bVuniform = Standard_False;

  //modified by NIZHNY-EMV Mon Jun 10 14:19:04 2013
  if (nbsu < theNUmin || nbsv < theNVmin) {
    Standard_Integer aNb;
    if (nbsu < nbsv) {
      aNb = (Standard_Integer)(nbsv * ((Standard_Real)theNUmin)/((Standard_Real)nbsu));
      aNb = Min(aNb, 30);
      bVuniform = (aNb > nbsv) ? Standard_True : bVuniform;
      nbsv = bVuniform ? aNb : nbsv;
    } else {
      aNb = (Standard_Integer)(nbsu * ((Standard_Real)theNVmin)/((Standard_Real)nbsv));
      aNb = Min(aNb, 30);
      bUuniform = (aNb > nbsu) ? Standard_True : bUuniform;
      nbsu = bUuniform ? aNb : nbsu;
    }
  }
  //modified by NIZHNY-EMV Mon Jun 10 14:19:05 2013

  if(nbsu < theNUmin) {
    nbsu = theNUmin;
    bUuniform = Standard_True;
  }

  if(nbsv < theNVmin) {
    nbsv = theNVmin;
    bVuniform = Standard_True;
  }

  TColStd_Array1OfReal anUPars(1, nbsu);
  TColStd_Array1OfBoolean anUFlg(1, nbsu);
  TColStd_Array1OfReal aVPars(1, nbsv);
  TColStd_Array1OfBoolean aVFlg(1, nbsv);

  //Filling of sample parameters
  if(bUuniform) {
    t1 = uinf;
    t2 = usup;
    dt = (t2 - t1)/(nbsu - 1);
    anUPars(1) = t1;
    anUFlg(1) = Standard_False;
    anUPars(nbsu) = t2;
    anUFlg(nbsu) = Standard_False;
    for(i = 2, t1 += dt; i < nbsu; ++i, t1 += dt) {
      anUPars(i) = t1;
      anUFlg(i) = Standard_False;
    }
  }
  else {  
    Standard_Integer nbi = aBS->UDegree();
    k = 0;
    t1 = uinf;
    for(i = ui1+1; i <= ui2; ++i) {
      if(i == ui2) t2 = usup;
      else t2 = aBS->UKnot(i);
      dt = (t2 - t1)/nbi;
      j = 1;
      do { 
	++k;
	anUPars(k) = t1;
	anUFlg(k) = Standard_False;
	t1 += dt;	
      }
      while (++j <= nbi);
      t1 = t2;
    }
    ++k;
    anUPars(k) = t1;
  }

  if(bVuniform) {
    t1 = vinf;
    t2 = vsup;
    dt = (t2 - t1)/(nbsv - 1);
    aVPars(1) = t1;
    aVFlg(1) = Standard_False;
    aVPars(nbsv) = t2;
    aVFlg(nbsv) = Standard_False;
    for(i = 2, t1 += dt; i < nbsv; ++i, t1 += dt) {
      aVPars(i) = t1;
      aVFlg(i) = Standard_False;
    }
  }
  else {  
    Standard_Integer nbi = aBS->VDegree();
    k = 0;
    t1 = vinf;
    for(i = vi1+1; i <= vi2; ++i) {
      if(i == vi2) t2 = vsup;
      else t2 = aBS->VKnot(i);
      dt = (t2 - t1)/nbi;
      j = 1;
      do { 
	++k;
	aVPars(k) = t1;
	aVFlg(k) = Standard_False;
	t1 += dt;	
      }
      while (++j <= nbi);
      t1 = t2;
    }
    ++k;
    aVPars(k) = t1;
  }
 
  //Analysis of deflection

  Standard_Real aDefl2 = Max(theDefl*theDefl, 1.e-9);
  Standard_Real tol = Max(0.01*aDefl2, 1.e-9);
  Standard_Integer l;

  anUFlg(1) = Standard_True;
  anUFlg(nbsu) = Standard_True;
  //myNbSamplesU = 2; 
  for(i = 1; i <= nbsv; ++i) {
    t1 = aVPars(i);
    j = 1;
    Standard_Boolean bCont = Standard_True;
    while (j < nbsu-1 && bCont) {

      if(anUFlg(j+1)) {
	++j;
	continue;
      }

      t2 = anUPars(j);
      gp_Pnt p1 = myS->Value(t2, t1);
      for(k = j+2; k <= nbsu; ++k) {
	t2 = anUPars(k);
	gp_Pnt p2 = myS->Value(t2, t1);
	//gce_MakeLin MkLin(p1, p2);
	//const gp_Lin& lin = MkLin.Value();

	if(p1.SquareDistance(p2) <= tol) continue;

	gp_Lin lin(p1, gp_Dir(gp_Vec(p1, p2)));
	Standard_Boolean ok = Standard_True;
	for(l = j+1; l < k; ++l) {

	  if(anUFlg(l)) {
	    ok = Standard_False;
	    break;
	  }

	  gp_Pnt pp =  myS->Value(anUPars(l), t1);
	  Standard_Real d = lin.SquareDistance(pp);
	  
	  if(d <= aDefl2) continue;

	  ok = Standard_False;
	  break;
	}

	if(!ok) {
	  j = k - 1;
	  anUFlg(j) = Standard_True;
	  //++myNbSamplesU;
	  break;
	}
	
	if(anUFlg(k)) {
	  j = k;
	  break;
	}
	

      }

      if(k >= nbsu) bCont = Standard_False;

    }
  }

  myNbSamplesU = 0;
  for (i = 1; i <= nbsu; i++)
    if (anUFlg(i) == Standard_True)
      myNbSamplesU++;
  
  if(myNbSamplesU < myMinPnts) {
    if(myNbSamplesU == 2) {
      //"uniform" distribution;
      Standard_Integer nn = nbsu/myMinPnts;
      anUFlg(1+nn) = Standard_True;
      anUFlg(nbsu-nn) = Standard_True;
    }
    else { //myNbSamplesU == 3
      //insert in bigger segment
      i = 2;
      while(!anUFlg(i++));
      if(i < nbsu/2) j = Min(i+(nbsu-i)/2, nbsu-1);
      else j = Max(i/2, 2);
    }
    anUFlg(j) = Standard_True;
    myNbSamplesU = myMinPnts;
  }
      
  aVFlg(1) = Standard_True;
  aVFlg(nbsv) = Standard_True;
  //myNbSamplesV = 2;
  for(i = 1; i <= nbsu; ++i) {
    t1 = anUPars(i);
    j = 1;
    Standard_Boolean bCont = Standard_True;
    while (j < nbsv-1 && bCont) {

      if(aVFlg(j+1)) {
	++j;
	continue;
      }

      t2 = aVPars(j);
      gp_Pnt p1 = myS->Value(t1, t2);
      for(k = j+2; k <= nbsv; ++k) {
	t2 = aVPars(k);
	gp_Pnt p2 = myS->Value(t1, t2);

	if(p1.SquareDistance(p2) <= tol) continue;
	//gce_MakeLin MkLin(p1, p2);
	//const gp_Lin& lin = MkLin.Value();
	gp_Lin lin(p1, gp_Dir(gp_Vec(p1, p2)));
	Standard_Boolean ok = Standard_True;
	for(l = j+1; l < k; ++l) {

	  if(aVFlg(l)) {
	    ok = Standard_False;
	    break;
	  }

	  gp_Pnt pp =  myS->Value(t1, aVPars(l));
	  Standard_Real d = lin.SquareDistance(pp);
	  
	  if(d <= aDefl2) continue;

	  ok = Standard_False;
	  break;
	}

	if(!ok) {
	  j = k - 1;
	  aVFlg(j) = Standard_True;
	  //++myNbSamplesV;
	  break;
	}
	
	if(aVFlg(k)) {
	  j = k;
	  break;
	}
	

      }

      if(k >= nbsv) bCont = Standard_False;

    }
  }

  myNbSamplesV = 0;
  for (i = 1; i <= nbsv; i++)
    if (aVFlg(i) == Standard_True)
      myNbSamplesV++;
  
  if(myNbSamplesV < myMinPnts) {
    if(myNbSamplesV == 2) {
      //"uniform" distribution;
      Standard_Integer nn = nbsv/myMinPnts;
      aVFlg(1+nn) = Standard_True;
      aVFlg(nbsv-nn) = Standard_True;
      myNbSamplesV = myMinPnts;
    }
    else { //myNbSamplesU == 3
      //insert in bigger segment
      i = 2;
      while(!aVFlg(i++));
      if(i < nbsv/2) j = Min(i+(nbsv-i)/2, nbsv-1);
      else j = Max(i/2, 2);
    }
    myNbSamplesV = myMinPnts;
    aVFlg(j) = Standard_True;
  }
  //
  //modified by NIZNHY-PKV Fri Dec 16 10:05:01 2011f
  //
  Standard_Boolean bFlag;
  //
  // U 
  bFlag=(myNbSamplesU < theNUmin);
  if (bFlag) {
    myNbSamplesU=nbsu;
  }
  //
  myUPars = new TColStd_HArray1OfReal(1, myNbSamplesU);
  //
  for(j = 0, i = 1; i <= nbsu; ++i) {
    if (bFlag) {
      myUPars->SetValue(i,anUPars(i));
    }
    else {
      if(anUFlg(i)) {
        ++j;
        myUPars->SetValue(j,anUPars(i));
      }
    }
  }
  //
  // V 
  bFlag=(myNbSamplesV < theNVmin);
  if (bFlag) {
    myNbSamplesV=nbsv;
  }
  //
  myVPars = new TColStd_HArray1OfReal(1, myNbSamplesV);
  //
  for(j = 0, i = 1; i <= nbsv; ++i) {
    if (bFlag) {
       myVPars->SetValue(i,aVPars(i));
    }
    else {
      if(aVFlg(i)) {
        ++j;
        myVPars->SetValue(j,aVPars(i));
      }
    }
  }
  //
  /*
  myUPars = new TColStd_HArray1OfReal(1, myNbSamplesU);
  myVPars = new TColStd_HArray1OfReal(1, myNbSamplesV);

  j = 0;
  for(i = 1; i <= nbsu; ++i) {
    if(anUFlg(i)) {
      ++j;
      myUPars->SetValue(j,anUPars(i));
    }
  }

  j = 0;
  for(i = 1; i <= nbsv; ++i) {
    if(aVFlg(i)) {
      ++j;
      myVPars->SetValue(j,aVPars(i));
    }
  }
  */
  //modified by NIZNHY-PKV Mon Dec 26 12:25:35 2011t

}

//=======================================================================
//function : IsUniformSampling
//purpose  : 
//=======================================================================
Standard_Boolean Adaptor3d_TopolTool::IsUniformSampling() const
{
  GeomAbs_SurfaceType typS = myS->GetType();

  if(typS == GeomAbs_BSplineSurface)
    return Standard_False;
  return Standard_True;
}
