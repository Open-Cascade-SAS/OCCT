// File:	GC_MakeCylindricalSurface.cxx
// Created:	Fri Oct  2 16:35:54 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeCylindricalSurface.ixx>
#include <gce_MakeCylinder.hxx>
#include <gp_Lin.hxx>
#include <StdFail_NotDone.hxx>

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Cylinder& C)
{
  TheError = gce_Done;
  TheCylinder = new Geom_CylindricalSurface(C);
}

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Ax2& A2    ,
						const Standard_Real  Radius)
{
  if (Radius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    TheError = gce_Done;
    TheCylinder = new Geom_CylindricalSurface(A2,Radius);
  }
}

//=========================================================================
//   Construction d un cylindre par axe <A1> et rayon <Radius>.           +
//=========================================================================

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Ax1& A1     ,
						 const Standard_Real Radius ) 
{
  gce_MakeCylinder Cyl = gce_MakeCylinder(A1,Radius);
  TheError = Cyl.Status();
  if (TheError == gce_Done) {
    TheCylinder=new Geom_CylindricalSurface(Cyl.Value());
  }
}

//=========================================================================
//   Construction d un cylindre par un cercle <Cir>.                      +
//=========================================================================

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Circ& Circ ) {
  gp_Cylinder Cyl = gce_MakeCylinder(Circ);
  TheCylinder=new Geom_CylindricalSurface(Cyl);
  TheError = gce_Done;
}

//=========================================================================
//   Construction d un cylindre par trois points <P1>, <P2>, <P3>.        +
//   Les deux premiers points definissent l axe.                          +
//   Le troisieme donne le rayon.                                         +
//=========================================================================

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Pnt& P1 ,
						       const gp_Pnt& P2 ,
						       const gp_Pnt& P3 ) {
  gce_MakeCylinder Cyl = gce_MakeCylinder(P1,P2,P3);
  TheError = Cyl.Status();
  if (TheError == gce_Done) {
    TheCylinder=new Geom_CylindricalSurface(Cyl.Value());
  }
}

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Cylinder& Cyl ,
						     const Standard_Real  Dist)
{
  TheError = gce_Done;
  Standard_Real R = Abs(Cyl.Radius()-Dist);
  TheCylinder = new Geom_CylindricalSurface(Cyl);
  TheCylinder->SetRadius(R);
}

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Cylinder& Cyl ,
						       const gp_Pnt&     Point)
{
  TheError = gce_Done;
  gp_Cylinder C(Cyl);
  gp_Lin L(C.Axis());
  Standard_Real R = L.Distance(Point);
  C.SetRadius(R);
  TheCylinder = new Geom_CylindricalSurface(C);
}

const Handle(Geom_CylindricalSurface)& 
       GC_MakeCylindricalSurface::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheCylinder;
}

const Handle(Geom_CylindricalSurface)& GC_MakeCylindricalSurface::Operator() const 
{
  return Value();
}

GC_MakeCylindricalSurface::operator Handle(Geom_CylindricalSurface) () const
{
  return Value();
}
