// File:	BRepClass_Intersector.cxx
// Created:	Thu Nov 19 15:04:21 1992
// Author:	Remi LEQUETTE
//		<rle@phylox>


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





