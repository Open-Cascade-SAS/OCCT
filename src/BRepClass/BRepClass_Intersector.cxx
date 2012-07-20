// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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



#include <BRepClass_Intersector.ixx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <IntRes2d_Domain.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom2d_Curve.hxx>
#include <ElCLib.hxx>
#include <Precision.hxx>

#include <Geom2d_Line.hxx>

#include <Geom2dInt_GInter.hxx>

#include <Extrema_ExtPC2d.hxx>
#include <IntRes2d_Transition.hxx>
#include <IntRes2d_IntersectionPoint.hxx>

//=======================================================================
//function : BRepClass_Intersector
//purpose  : 
//=======================================================================

BRepClass_Intersector::BRepClass_Intersector()
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void  BRepClass_Intersector::Perform(const gp_Lin2d& L, 
				     const Standard_Real P, 
				     const Standard_Real Tol, 
				     const BRepClass_Edge& E)
{
  
  Standard_Real pfbid,plbid;
  if (BRep_Tool::CurveOnSurface(E.Edge(),E.Face(),pfbid,plbid).IsNull()) {
    done = Standard_False; // !IsDone()
  }
  else {
    IntRes2d_Domain DL;
    if(P!=RealLast()) 
      DL.SetValues(L.Location(),0.,Tol,ElCLib::Value(P,L),P,Tol);
    else 
      DL.SetValues(L.Location(),0.,Tol,Standard_True);
    
    const TopoDS_Edge& EE = E.Edge();
    const TopoDS_Face& F = E.Face();
    TopoDS_Vertex Vdeb, Vfin;
    TopExp::Vertices(EE, Vdeb, Vfin);
    BRepAdaptor_Curve2d C(EE,F);
    Standard_Real deb = C.FirstParameter(), fin = C.LastParameter();

    // Case of "ON": direct check of belonging to edge
    // taking into account the tolerance
    Extrema_ExtPC2d theExtPC2d(L.Location(), C);
    Standard_Real MinDist = RealLast(), aDist;
    Standard_Integer MinInd = 0, i;
    if (theExtPC2d.IsDone())
    {
      for (i = 1; i <= theExtPC2d.NbExt(); i++)
      {
        aDist = theExtPC2d.SquareDistance(i);
        if (aDist < MinDist)
        {
          MinDist = aDist;
          MinInd = i;
        }
      }
    }
    if (MinInd)
      MinDist = sqrt(MinDist);
    if (MinDist <= Tol)
    {
      gp_Pnt2d pnt_exact = (theExtPC2d.Point(MinInd)).Value();
      Standard_Real par = (theExtPC2d.Point(MinInd)).Parameter();
      IntRes2d_Transition tr_on_lin(IntRes2d_Head);
      IntRes2d_Position pos_on_curve = IntRes2d_Middle;
      if (Abs(par - deb) <= Precision::Confusion())
        pos_on_curve = IntRes2d_Head;
      else if (Abs(par - fin) <= Precision::Confusion())
        pos_on_curve = IntRes2d_End;
      IntRes2d_Transition tr_on_curve(pos_on_curve);
      IntRes2d_IntersectionPoint pnt_inter(pnt_exact, 0., par,
                                           tr_on_lin, tr_on_curve, Standard_False);
      this->Append(pnt_inter);
      done = Standard_True;
      return;
    }
    ///////////////
    
    gp_Pnt2d pdeb,pfin;
    C.D0(deb,pdeb);
    C.D0(fin,pfin);
    Standard_Real toldeb = 1.e-5, tolfin = 1.e-5;
#if 0 
    // essai de calcul juste des tolerances du domaine
    // qui ne couche pas avec les modeles pourris de 
    // styler !!
    BRepAdaptor_Surface S(F);
    gp_Vec2d vdeb,vfin;
    C.D1(deb,pdeb,vdeb);
    C.D1(fin,pfin,vfin);
    gp_Pnt P; gp_Vec DU, DV;
    S.D1(pdeb.X(),pdeb.Y(),P,DU,DV);
    Standard_Real scaldeb = (vdeb.X()*DU + vdeb.Y()*DV).Magnitude();
    scaldeb = Max(scaldeb, 1.e-5);
    toldeb = BRep_Tool::Tolerance(Vdeb)/scaldeb;
    S.D1(pfin.X(),pfin.Y(),P,DU,DV);
    Standard_Real scalfin = (vfin.X()*DU + vfin.Y()*DV).Magnitude();
    scalfin = Max(scalfin, 1.e-5);
    tolfin = BRep_Tool::Tolerance(Vfin)/scalfin;
#endif
    
    IntRes2d_Domain DE(pdeb,deb,toldeb,pfin,fin,tolfin);
    // temporary periodic domain
    if (C.Curve()->IsPeriodic()) {
      DE.SetEquivalentParameters(C.FirstParameter(),
				 C.FirstParameter() + 
				 C.Curve()->LastParameter() -
				 C.Curve()->FirstParameter());
    }
    
    Handle(Geom2d_Line) GL= new Geom2d_Line(L);
    Geom2dAdaptor_Curve CGA(GL);
    Geom2dInt_GInter Inter(CGA,DL,C,DE,
			   Precision::PConfusion(),
			   Precision::PIntersection());
    this->SetValues(Inter);
  }
}

//=======================================================================
//function : LocalGeometry
//purpose  : 
//=======================================================================

void  BRepClass_Intersector::LocalGeometry(const BRepClass_Edge& E, 
					   const Standard_Real U, 
					   gp_Dir2d& Tang, 
					   gp_Dir2d& Norm, 
					   Standard_Real& C) const 
{
  Standard_Real f,l;
  Geom2dLProp_CLProps2d Prop(BRep_Tool::CurveOnSurface(E.Edge(),E.Face(),f,l),
			     U,2,Precision::PConfusion());
  Prop.Tangent(Tang);
  C = Prop.Curvature();
  if (C > Precision::PConfusion())
    Prop.Normal(Norm);
  else
    Norm.SetCoord(Tang.Y(),-Tang.X());
}





