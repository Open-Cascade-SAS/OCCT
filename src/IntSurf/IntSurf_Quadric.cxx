// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <IntSurf_Quadric.ixx>
#include <StdFail_NotDone.hxx>

#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <gp.hxx>



// ============================================================
IntSurf_Quadric::IntSurf_Quadric ():typ(GeomAbs_OtherSurface),
   prm1(0.), prm2(0.), prm3(0.), prm4(0.)
{}
// ============================================================
IntSurf_Quadric::IntSurf_Quadric (const gp_Pln& P):
      ax3(P.Position()),typ(GeomAbs_Plane)
{
  ax3direc = ax3.Direct();
  P.Coefficients(prm1,prm2,prm3,prm4); 
}
// ============================================================
IntSurf_Quadric::IntSurf_Quadric (const gp_Cylinder& C):

       ax3(C.Position()),lin(ax3.Axis()),typ(GeomAbs_Cylinder)
{
  prm2=prm3=prm4=0.0;
  ax3direc=ax3.Direct();
  prm1=C.Radius();
}
// ============================================================
IntSurf_Quadric::IntSurf_Quadric (const gp_Sphere& S):

       ax3(S.Position()),lin(ax3.Axis()),typ(GeomAbs_Sphere)
{
  prm2=prm3=prm4=0.0;
  ax3direc = ax3.Direct();
  prm1=S.Radius();
}
// ============================================================
IntSurf_Quadric::IntSurf_Quadric (const gp_Cone& C):

       ax3(C.Position()),typ(GeomAbs_Cone)
{
  ax3direc = ax3.Direct();
  lin.SetPosition(ax3.Axis());
  prm1 = C.RefRadius();
  prm2 = C.SemiAngle();
  prm3 = Cos(prm2);
  prm4 = 0.0;
}
// ============================================================
void IntSurf_Quadric::SetValue (const gp_Pln& P)
{
  typ = GeomAbs_Plane;
  ax3 = P.Position();
  ax3direc = ax3.Direct();
  P.Coefficients(prm1,prm2,prm3,prm4); 
}
// ============================================================
void IntSurf_Quadric::SetValue (const gp_Cylinder& C)
{
  typ = GeomAbs_Cylinder;
  ax3 = C.Position();
  ax3direc = ax3.Direct();
  lin.SetPosition(ax3.Axis());
  prm1 = C.Radius();
  prm2=prm3=prm4=0.0;
}
// ============================================================
void IntSurf_Quadric::SetValue (const gp_Sphere& S)
{
  typ = GeomAbs_Sphere;
  ax3 = S.Position();
  ax3direc = ax3.Direct();
  lin.SetPosition(ax3.Axis());
  prm1 = S.Radius();
  prm2=prm3=prm4=0.0;
}
// ============================================================
void IntSurf_Quadric::SetValue (const gp_Cone& C)
{
  typ = GeomAbs_Cone;
  ax3 = C.Position();
  ax3direc = ax3.Direct();
  lin.SetPosition(ax3.Axis());
  prm1 = C.RefRadius();
  prm2 = C.SemiAngle();
  prm3 = Cos(prm2);
  prm4 = 0.0;
}
// ============================================================
Standard_Real IntSurf_Quadric::Distance (const gp_Pnt& P) const {
  switch (typ) {
  case GeomAbs_Plane:   // plan
    return prm1*P.X() + prm2*P.Y() + prm3*P.Z() + prm4;
  case GeomAbs_Cylinder:   // cylindre
    return (lin.Distance(P) - prm1);
  case GeomAbs_Sphere:   // sphere
    return (lin.Location().Distance(P) - prm1);
  case GeomAbs_Cone:   // cone
    {
      Standard_Real dist = lin.Distance(P);
      Standard_Real U,V;
      ElSLib::ConeParameters(ax3,prm1,prm2,P,U,V);
      gp_Pnt Pp = ElSLib::ConeValue(U,V,ax3,prm1,prm2);
      Standard_Real distp = lin.Distance(Pp);
      dist = (dist-distp)/prm3;
      return(dist);
    }
  default:
    {
    }
    break;
  }
  return(0.0);
}
// ============================================================
gp_Vec IntSurf_Quadric::Gradient (const gp_Pnt& P) const {
  gp_Vec grad;
  switch (typ) {
  case GeomAbs_Plane:   // plan
    grad.SetCoord(prm1,prm2,prm3);
    break;
  case GeomAbs_Cylinder:   // cylindre
    {
      gp_XYZ PP(lin.Location().XYZ());
      PP.Add(ElCLib::Parameter(lin,P)*lin.Direction().XYZ());
      grad.SetXYZ(P.XYZ()-PP);
      Standard_Real N = grad.Magnitude();
      if(N>1e-14) { grad.Divide(N); } 
      else { grad.SetCoord(0.0,0.0,0.0); } 
    }
    break;
  case GeomAbs_Sphere:   // sphere
    {
      gp_XYZ PP(P.XYZ());
      grad.SetXYZ((PP-lin.Location().XYZ()));
      Standard_Real N = grad.Magnitude();
      if(N>1e-14) { grad.Divide(N); } 
      else { grad.SetCoord(0.0,0.0,0.0); }
    }
    break;
  case GeomAbs_Cone:   // cone
    {
      Standard_Real U,V;
      ElSLib::ConeParameters(ax3,prm1,prm2,P,U,V);
      gp_Pnt Pp = ElSLib::ConeValue(U,V,ax3,prm1,prm2);
      gp_Vec D1u,D1v;
      ElSLib::ConeD1(U,V,ax3,prm1,prm2,Pp,D1u,D1v);
      grad=D1u.Crossed(D1v);
      if(ax3direc==Standard_False) { 
	grad.Reverse();
      }
      grad.Normalize();
    }
    break;
  default:
    {}
    break;
  }
  return grad;
}
// ============================================================
void IntSurf_Quadric::ValAndGrad (const gp_Pnt& P,
				   Standard_Real& Dist,
				   gp_Vec& Grad) const
{

  switch (typ) {
  case GeomAbs_Plane:   
    {
      Dist = prm1*P.X() + prm2*P.Y() + prm3*P.Z() + prm4;
      Grad.SetCoord(prm1,prm2,prm3);
    }
    break;
  case GeomAbs_Cylinder:   
    {
      Dist = lin.Distance(P) - prm1;
      gp_XYZ PP(lin.Location().XYZ());
      PP.Add(ElCLib::Parameter(lin,P)*lin.Direction().XYZ());
      Grad.SetXYZ((P.XYZ()-PP));
      Standard_Real N = Grad.Magnitude();
      if(N>1e-14) { Grad.Divide(N); } 
      else { Grad.SetCoord(0.0,0.0,0.0); }
    }
    break;
  case GeomAbs_Sphere:  
    {
      Dist = lin.Location().Distance(P) - prm1;
      gp_XYZ PP(P.XYZ());
      Grad.SetXYZ((PP-lin.Location().XYZ()));
      Standard_Real N = Grad.Magnitude();
      if(N>1e-14) { Grad.Divide(N); } 
      else { Grad.SetCoord(0.0,0.0,0.0); }
    }
    break;
  case GeomAbs_Cone:  
    {
      Standard_Real dist = lin.Distance(P);
      Standard_Real U,V;
      gp_Vec D1u,D1v;
      gp_Pnt Pp;
      ElSLib::ConeParameters(ax3,prm1,prm2,P,U,V);
      ElSLib::ConeD1(U,V,ax3,prm1,prm2,Pp,D1u,D1v);
      Standard_Real distp = lin.Distance(Pp);
      dist = (dist-distp)/prm3;
      Dist = dist;
      Grad=D1u.Crossed(D1v);
      if(ax3direc==Standard_False) { 
	Grad.Reverse();
      }
      //-- lbr le 7 mars 96 
      //-- Si le gardient est nul, on est sur l axe 
      //-- et dans ce cas dist vaut 0 
      //-- On peut donc renvoyer une valeur quelconque. 
      if(   Grad.X() > 1e-13 
	 || Grad.Y() > 1e-13 
	 || Grad.Z() > 1e-13) { 
	Grad.Normalize();
      }
    }
    break;
  default:
    {}
    break;
  }
}
// ============================================================
gp_Pnt IntSurf_Quadric::Value(const Standard_Real U,
			       const Standard_Real V) const
{
  switch (typ) {

  case GeomAbs_Plane:  
    return ElSLib::PlaneValue(U,V,ax3);
  case GeomAbs_Cylinder:
    return ElSLib::CylinderValue(U,V,ax3,prm1);
  case GeomAbs_Sphere:
    return ElSLib::SphereValue(U,V,ax3,prm1);
  case GeomAbs_Cone:  
    return ElSLib::ConeValue(U,V,ax3,prm1,prm2);
  default:
    {
      gp_Pnt p(0,0,0);
      return(p);
    }
    //break;
  }
// pop : pour NT
//  return gp_Pnt(0,0,0);
}
// ============================================================
void IntSurf_Quadric::D1(const Standard_Real U,
			 const Standard_Real V,
			 gp_Pnt& P,
			 gp_Vec& D1U,
			 gp_Vec& D1V) const
{
  switch (typ) {
  case GeomAbs_Plane:  
    ElSLib::PlaneD1(U,V,ax3,P,D1U,D1V);
    break;
  case GeomAbs_Cylinder:
    ElSLib::CylinderD1(U,V,ax3,prm1,P,D1U,D1V);
    break;
  case GeomAbs_Sphere: 
    ElSLib::SphereD1(U,V,ax3,prm1,P,D1U,D1V);
    break;
  case GeomAbs_Cone:  
    ElSLib::ConeD1(U,V,ax3,prm1,prm2,P,D1U,D1V);
    break;
  default:
    {
    }
    break;
  }
}
// ============================================================
gp_Vec IntSurf_Quadric::DN(const Standard_Real U,
			    const Standard_Real V,
			    const Standard_Integer Nu,
			    const Standard_Integer Nv) const
{
  switch (typ) {
  case GeomAbs_Plane: 
    return ElSLib::PlaneDN(U,V,ax3,Nu,Nv);
  case GeomAbs_Cylinder:
    return ElSLib::CylinderDN(U,V,ax3,prm1,Nu,Nv);
  case GeomAbs_Sphere:
    return ElSLib::SphereDN(U,V,ax3,prm1,Nu,Nv);
  case GeomAbs_Cone:  
    return ElSLib::ConeDN(U,V,ax3,prm1,prm2,Nu,Nv);
  default:
    {
      gp_Vec v(0,0,0);
      return(v);
    }
    //break;
  }
// pop : pour NT
//  return gp_Vec(0,0,0);
}
// ============================================================
gp_Vec IntSurf_Quadric::Normale(const Standard_Real U,
				 const Standard_Real V) const
{
  switch (typ) {
  case GeomAbs_Plane:  
    if(ax3direc) 
      return ax3.Direction();
    else 
      return ax3.Direction().Reversed();
  case GeomAbs_Cylinder:  
    return Normale(Value(U,V));
  case GeomAbs_Sphere:  
    return Normale(Value(U,V));
  case GeomAbs_Cone:   
    {
      gp_Pnt P;
      gp_Vec D1u,D1v;
      ElSLib::ConeD1(U,V,ax3,prm1,prm2,P,D1u,D1v);
      if(D1u.Magnitude()<0.0000001) { 
	gp_Vec Vn(0.0,0.0,0.0); 
	return(Vn);
      }
      return(D1u.Crossed(D1v));
    }
  default:     
    {
      gp_Vec v(0,0,0);
      return(v);
    }     
  //  break;
  }
// pop : pour NT
//  return gp_Vec(0,0,0);
}
// ============================================================
gp_Vec IntSurf_Quadric::Normale (const gp_Pnt& P) const
{
  switch (typ) {
  case GeomAbs_Plane:   
    if(ax3direc) 
      return ax3.Direction();
    else 
      return ax3.Direction().Reversed();
  case GeomAbs_Cylinder:   
    {
      if(ax3direc) { 
	return lin.Normal(P).Direction();
      }
      else { 
	gp_Dir D(lin.Normal(P).Direction());
	D.Reverse();
	return(D);
      }
    }
  case GeomAbs_Sphere:   
    {
      if(ax3direc) { 
	gp_Vec ax3P(ax3.Location(),P);
	return gp_Dir(ax3P);
      }
      else { 
	gp_Vec Pax3(P,ax3.Location());
	return gp_Dir(Pax3);
      }
    }
  case GeomAbs_Cone:   
    {
      Standard_Real U,V;
      ElSLib::ConeParameters(ax3,prm1,prm2,P,U,V);
      return Normale(U,V);;
    }
  default:      
    {
      gp_Vec v(0,0,0);
      return(v);
    } //    break;
  }
}
// ============================================================
void IntSurf_Quadric::Parameters (const gp_Pnt& P,
				   Standard_Real& U,
				   Standard_Real& V) const
{
  switch (typ) {
  case GeomAbs_Plane:   
    ElSLib::PlaneParameters(ax3,P,U,V);
    break;
  case GeomAbs_Cylinder: 
    ElSLib::CylinderParameters(ax3,prm1,P,U,V);
    break;
  case GeomAbs_Sphere:   
    ElSLib::SphereParameters(ax3,prm1,P,U,V);
    break;
  case GeomAbs_Cone: 
    {
      ElSLib::ConeParameters(ax3,prm1,prm2,P,U,V);
    }
    break;
  default:
    {}
    break;
  }
}
// ============================================================





