// Created on: 1997-05-05
// Created by: Joelle CHAUVET
// Copyright (c) 1997-1999 Matra Datavision
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

// Modified:	Mon Nov  3 10:24:07 1997
//		ne traite que les GeomAdaptor_Surface;
//              plus de reference a BRepAdaptor


#include <GeomPlate_CurveConstraint.ixx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <GeomAdaptor.hxx>
#include <Precision.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Approx_Curve2d.hxx>
#include <GeomAbs_Shape.hxx>

//---------------------------------------------------------
//         Constructeur vide
//---------------------------------------------------------
GeomPlate_CurveConstraint :: GeomPlate_CurveConstraint () :
myLProp(2,1.e-4)
{
}

//---------------------------------------------------------
//         Constructeurs avec courbe sur surface
//---------------------------------------------------------
GeomPlate_CurveConstraint :: GeomPlate_CurveConstraint (const Handle(Adaptor3d_HCurveOnSurface)& Boundary,
						const Standard_Integer Tang,
						const Standard_Integer NPt,
						const Standard_Real TolDist,
						const Standard_Real TolAng,
						const Standard_Real TolCurv
) :
myFrontiere(Boundary),
myLProp(2,TolDist),
myTolDist(TolDist),
myTolAng(TolAng),
myTolCurv(TolCurv)
{ myOrder=Tang;
  if ((Tang<-1)||(Tang>2))
    Standard_Failure::Raise("GeomPlate : The continuity is not G0 G1 or G2"); 
  myNbPoints=NPt;
  myConstG0=Standard_True;
  myConstG1=Standard_True;
  myConstG2=Standard_True;
 if (myFrontiere.IsNull())
    Standard_Failure::Raise("GeomPlate_CurveConstraint : Curve must be on a Surface"); 
  Handle(Geom_Surface) Surf;
  Handle(GeomAdaptor_HSurface) GS1;
  GS1 = Handle(GeomAdaptor_HSurface)::DownCast(myFrontiere->ChangeCurve().GetSurface());
  if (!GS1.IsNull()) {
    Surf=GS1->ChangeSurface().Surface();
  }
  else {
//      Handle(BRepAdaptor_HSurface) BS1;
//      BS1=Handle(BRepAdaptor_HSurface)::DownCast(myFrontiere->ChangeCurve().GetSurface());
//      Surf = BRep_Tool::Surface(BS1->ChangeSurface().Face());
    Standard_Failure::Raise("GeomPlate_CurveConstraint : Surface must be GeomAdaptor_Surface");     
  }
  myLProp.SetSurface(Surf);
  my2dCurve.Nullify();
  myHCurve2d.Nullify();
  myTolU=0.;
  myTolV=0.;
  myG0Crit.Nullify();
  myG1Crit.Nullify();
  myG2Crit.Nullify();

}

//---------------------------------------------------------
//    Constructeurs avec courbe 3d (pour continuite G0 G-1)
//---------------------------------------------------------
GeomPlate_CurveConstraint :: GeomPlate_CurveConstraint (const Handle(Adaptor3d_HCurve)& Boundary,
						const Standard_Integer Tang, 
						const Standard_Integer NPt,
						const Standard_Real TolDist) :
my3dCurve(Boundary),
myLProp(2,TolDist),
myTolDist(TolDist)
{ myOrder=Tang;
  if ((Tang!=-1)&&(Tang!=0))
    Standard_Failure::Raise("GeomPlate : The continuity is not G0 or G-1"); 
  myNbPoints=NPt;
  myConstG0=Standard_True;
  myConstG1=Standard_True;
  myConstG2=Standard_True;
  my2dCurve.Nullify();
  myHCurve2d.Nullify();
  myTolU=0.;
  myTolV=0.;
  myG0Crit.Nullify();
  myG1Crit.Nullify();
  myG2Crit.Nullify();
}
//---------------------------------------------------------
// Fonction : FirstParameter
//---------------------------------------------------------
Standard_Real GeomPlate_CurveConstraint :: FirstParameter() const
{ 
  if (!myHCurve2d.IsNull())
    return myHCurve2d->FirstParameter();
  else if (my3dCurve.IsNull())
    return myFrontiere->FirstParameter();
  else
    return my3dCurve->FirstParameter();
}
//---------------------------------------------------------
// Fonction : LastParameter
//---------------------------------------------------------
Standard_Real GeomPlate_CurveConstraint :: LastParameter() const
{    
  if (!myHCurve2d.IsNull())
    return myHCurve2d->LastParameter();
  else if (my3dCurve.IsNull())
    return myFrontiere->LastParameter();
  else
 return my3dCurve->LastParameter();
}
//---------------------------------------------------------
// Fonction : Length
//---------------------------------------------------------
Standard_Real GeomPlate_CurveConstraint :: Length() const
{ GCPnts_AbscissaPoint AP;
  if (my3dCurve.IsNull())
{//   GCPnts_AbscissaPoint A(myFrontiere->Curve(),AP.Length(myFrontiere->Curve())/2,myFrontiere->FirstParameter());
  //  Standard_Real toto=A.Parameter();
//cout<<toto<<endl;
return AP.Length(myFrontiere->GetCurve()); 
    
}
 else
{ //  GCPnts_AbscissaPoint A(my3dCurve->Curve(),AP.Length(my3dCurve->Curve())/2,my3dCurve->FirstParameter());
  //  Standard_Real toto=A.Parameter();
//cout<<toto<<endl;

 return AP.Length(my3dCurve->GetCurve());
}


 
  
}
//---------------------------------------------------------
// Fonction : D0
//---------------------------------------------------------
void GeomPlate_CurveConstraint :: D0(const Standard_Real U,gp_Pnt& P) const
{ gp_Pnt2d P2d;
   
  if (my3dCurve.IsNull())
    { P2d = myFrontiere->ChangeCurve().GetCurve()->Value(U);
      myFrontiere->ChangeCurve().GetSurface()->D0(P2d.Coord(1),P2d.Coord(2),P);
    }
  else
    my3dCurve->D0(U,P);
}
//---------------------------------------------------------
// Fonction : D1
//---------------------------------------------------------
void GeomPlate_CurveConstraint :: D1(const Standard_Real U,
                                 gp_Pnt& P,gp_Vec& V1,gp_Vec& V2) const
{ gp_Pnt2d P2d;
   if (!my3dCurve.IsNull())
    Standard_Failure::Raise("GeomPlate_CurveConstraint.cxx : Curve must be on a Surface"); 

  P2d = myFrontiere->ChangeCurve().GetCurve()->Value(U);
  myFrontiere->ChangeCurve().GetSurface()->D1(P2d.Coord(1),P2d.Coord(2),P,V1,V2);

}

//---------------------------------------------------------
// Fonction : D2
//---------------------------------------------------------
void GeomPlate_CurveConstraint :: D2(const Standard_Real U,
                               gp_Pnt& P,gp_Vec& V1,gp_Vec& V2,
                               gp_Vec& V3,gp_Vec& V4,gp_Vec& V5) const
{  gp_Pnt2d P2d;
   if (!my3dCurve.IsNull())
    Standard_Failure::Raise("GeomPlate_CurveConstraint.cxx : Curve must be on a Surface"); 

  P2d = myFrontiere->ChangeCurve().GetCurve()->Value(U);
  myFrontiere->ChangeCurve().GetSurface()->D2(P2d.Coord(1),P2d.Coord(2),P,V1,V2,V3,V4,V5);

}
//---------------------------------------------------------
// Fonction : SetG0Criterion
//---------------------------------------------------------
void GeomPlate_CurveConstraint :: SetG0Criterion(const Handle_Law_Function &G0Crit)
{ myG0Crit=G0Crit;
  myConstG0=Standard_False;
}
//---------------------------------------------------------
// Fonction : SetG1Criterion
//---------------------------------------------------------
void GeomPlate_CurveConstraint :: SetG1Criterion(const Handle_Law_Function &G1Crit)
{ if (!my3dCurve.IsNull())
    Standard_Failure::Raise("GeomPlate_CurveConstraint.cxx : Curve must be on a Surface"); 
  myG1Crit=G1Crit;
  myConstG1=Standard_False;
}
//---------------------------------------------------------
// Fonction : SetG2Criterion
//---------------------------------------------------------
void GeomPlate_CurveConstraint :: SetG2Criterion(const Handle_Law_Function &G2Crit)
{ if (!my3dCurve.IsNull())
    Standard_Failure::Raise("GeomPlate_CurveConstraint.cxx : Curve must be on a Surface"); 
  myG2Crit=G2Crit;
  myConstG2=Standard_False;
}
//---------------------------------------------------------
// Fonction : G0Criterion
//---------------------------------------------------------
Standard_Real GeomPlate_CurveConstraint :: G0Criterion(const Standard_Real U) const
{ if (myConstG0)
    return myTolDist;
  else 
    return myG0Crit->Value(U);
}
//---------------------------------------------------------
// Fonction : G1Criterion 
//---------------------------------------------------------
Standard_Real GeomPlate_CurveConstraint :: G1Criterion(const Standard_Real U) const
{ if (!my3dCurve.IsNull())
    Standard_Failure::Raise("GeomPlate_CurveConstraint.cxx : Curve must be on a Surface"); 
  if (myConstG1)
    return myTolAng;
  else 
    return myG1Crit->Value(U);
}
//---------------------------------------------------------
// Fonction : G2Criterion 
//---------------------------------------------------------
Standard_Real GeomPlate_CurveConstraint :: G2Criterion(const Standard_Real U) const
{ if (!my3dCurve.IsNull())
    Standard_Failure::Raise("GeomPlate_CurveConstraint.cxx : Curve must be on a Surface"); 
  if (myConstG2)
    return myTolCurv;
  else 
    return myG2Crit->Value(U);
}
//---------------------------------------------------------
// Fonction : Curve2dOnSurf
//---------------------------------------------------------
Handle(Geom2d_Curve) GeomPlate_CurveConstraint :: Curve2dOnSurf () const
{ 
  if(my2dCurve.IsNull() && !myHCurve2d.IsNull())
  {
     Handle(Geom2d_Curve) C2d;
     GeomAbs_Shape Continuity = GeomAbs_C1;
     Standard_Integer MaxDegree = 10;    
     Standard_Integer MaxSeg=20+myHCurve2d->NbIntervals(GeomAbs_C3);
     Approx_Curve2d appr(myHCurve2d,myHCurve2d->FirstParameter(),myHCurve2d->LastParameter(),
                         myTolU,myTolV,Continuity,MaxDegree,MaxSeg);
     C2d = appr.Curve();
     return C2d;
  }
  else  return my2dCurve;
}
//---------------------------------------------------------
// Fonction : SetCurve2dOnSurf
//---------------------------------------------------------
void GeomPlate_CurveConstraint :: SetCurve2dOnSurf (const Handle(Geom2d_Curve) &Curve) 
{ my2dCurve=Curve;
}
//---------------------------------------------------------
// Fonction : ProjectedCurve
//---------------------------------------------------------
Handle(Adaptor2d_HCurve2d) GeomPlate_CurveConstraint :: ProjectedCurve () const
{  
  return myHCurve2d;
}
//---------------------------------------------------------
// Fonction : SetProjectedCurve
//---------------------------------------------------------
void GeomPlate_CurveConstraint :: SetProjectedCurve (const Handle(Adaptor2d_HCurve2d) &Curve, 
                                                    const Standard_Real TolU,const Standard_Real TolV) 
{ myHCurve2d=Curve;
  myTolU=TolU;
  myTolV=TolV;
}

//---------------------------------------------------------
// Fonction : Curve3d
//---------------------------------------------------------
Handle(Adaptor3d_HCurve) GeomPlate_CurveConstraint :: Curve3d () const
{ if (my3dCurve.IsNull())
    return myFrontiere;
  else
    return my3dCurve;
}
//------------------------------------------------------------
//Fonction : NbPoints
//------------------------------------------------------------
Standard_Integer GeomPlate_CurveConstraint::NbPoints() const
{
return myNbPoints;
}   
//------------------------------------------------------------
//Fonction : Order
//------------------------------------------------------------
Standard_Integer GeomPlate_CurveConstraint::Order() const
{
return myOrder;
}
//------------------------------------------------------------
//Fonction : SetNbPoints
//------------------------------------------------------------
void GeomPlate_CurveConstraint::SetNbPoints(const Standard_Integer NewNb)
{
myNbPoints = NewNb;
}
//------------------------------------------------------------
//Fonction : SetOrder
//------------------------------------------------------------
void GeomPlate_CurveConstraint::SetOrder(const Standard_Integer Order)
{
myOrder = Order;
}
//------------------------------------------------------------
//Fonction : LPropSurf
//------------------------------------------------------------
GeomLProp_SLProps &GeomPlate_CurveConstraint::LPropSurf(const Standard_Real U)
{  if (myFrontiere.IsNull())
    Standard_Failure::Raise("GeomPlate_CurveConstraint.cxx : Curve must be on a Surface"); 
  gp_Pnt2d P2d= myFrontiere->ChangeCurve().GetCurve()->Value(U);
myLProp.SetParameters(P2d.X(),P2d.Y());
return myLProp;
}
