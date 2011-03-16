// File:	GC_MakeConicalSurface.cxx
// Created:	Fri Oct  2 16:33:58 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeConicalSurface.ixx>
#include <gce_MakeCone.hxx>
#include <gp.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_NotImplemented.hxx>

GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Ax2&       A2    ,
					       const Standard_Real Ang   ,
					       const Standard_Real Radius)
{
  if (Radius < 0.) { TheError = gce_NegativeRadius; }
  else if (Ang <= gp::Resolution() || Ang >= PI/2. - gp::Resolution()) {
    TheError = gce_BadAngle;
  }
  else {
    TheError = gce_Done;
    TheCone = new Geom_ConicalSurface(A2,Ang,Radius);
  }
}

GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Cone& C)
{
  TheError = gce_Done;
  TheCone = new Geom_ConicalSurface(C);
}

GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Cone& , //C,
					       const gp_Pnt&  ) //P )
{
  Standard_NotImplemented::Raise("GC_MakeConicalSurface");
}

GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Cone& , //C,
					       const Standard_Real) // Dist )
{
  Standard_NotImplemented::Raise("GC_MakeConicalSurface");
}

//=========================================================================
//   Creation d un cone par quatre points.                                +
//   les deux premiers donnent l axe.                                     +
//   le troisieme donne le rayon de la base.                              +
//   le troisieme et le quatrieme le demi angle.                          +
//=========================================================================

GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Pnt& P1 ,
					       const gp_Pnt& P2 ,
					       const gp_Pnt& P3 ,
					       const gp_Pnt& P4 ) 
{
  gce_MakeCone C = gce_MakeCone(P1,P2,P3,P4);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCone = new Geom_ConicalSurface(C.Value());
  }
}


GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Ax1& , //Axis,
					       const gp_Pnt& , //P1  ,
					       const gp_Pnt& ) //P2   )
{
  Standard_NotImplemented::Raise("GC_MakeConicalSurface");
}


GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Lin& , //Axis,
					       const gp_Pnt& , //P1  ,
					       const gp_Pnt& ) //P2   )
{
  Standard_NotImplemented::Raise("GC_MakeConicalSurface");
}


//=========================================================================
//=========================================================================

GC_MakeConicalSurface::GC_MakeConicalSurface(const gp_Pnt&       P1 ,
					       const gp_Pnt&       P2 ,
					       const Standard_Real R1 ,
					       const Standard_Real R2 ) 
{
  gce_MakeCone C = gce_MakeCone(P1,P2,R1,R2);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCone = new Geom_ConicalSurface(C);
  }
}

const Handle(Geom_ConicalSurface)& GC_MakeConicalSurface::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheCone;
}

const Handle(Geom_ConicalSurface)& GC_MakeConicalSurface::Operator() const 
{
  return Value();
}

GC_MakeConicalSurface::operator Handle(Geom_ConicalSurface) () const
{
  return Value();
}

