// File:	GC_MakeTrimmedCone.cxx
// Created:	Fri Oct  2 16:38:33 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeTrimmedCone.ixx>
#include <GC_MakeConicalSurface.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom_ConicalSurface.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Extrema_ExtPElC.hxx>

//=========================================================================
//   Creation d un cone par quatre points.                                +
//   les deux premiers donnent l axe.                                     +
//   le troisieme donne le rayon de la base.                              +
//   le troisieme et le quatrieme le demi angle.                          +
//=========================================================================

GC_MakeTrimmedCone::GC_MakeTrimmedCone(const gp_Pnt& P1 ,
					 const gp_Pnt& P2 ,
					 const gp_Pnt& P3 ,
					 const gp_Pnt& P4 ) 
{
  GC_MakeConicalSurface Cone(P1,P2,P3,P4);
  TheError = Cone.Status();
  if (TheError == gce_Done) {
    gp_Dir D1(P2.XYZ()-P1.XYZ());
    gp_Lin L1(P1,D1);
    Extrema_ExtPElC ext1(P3,L1,1.0e-7,-2.0e+100,+2.0e+100);
    Extrema_ExtPElC ext2(P4,L1,1.0e-7,-2.0e+100,+2.0e+100);
    gp_Pnt P5 = ext1.Point(1).Value();
    gp_Pnt P6 = ext2.Point(1).Value();
    Standard_Real D = P6.Distance(P5)/cos((Cone.Value())->SemiAngle());
    TheCone=new Geom_RectangularTrimmedSurface(Cone.Value(),0.,2.*PI,0.,D,Standard_True,Standard_True);
  }
}

//=========================================================================
//=========================================================================

GC_MakeTrimmedCone::GC_MakeTrimmedCone(const gp_Pnt&       P1 ,
					 const gp_Pnt&       P2 ,
					 const Standard_Real R1 ,
					 const Standard_Real R2 ) 
{
  GC_MakeConicalSurface Cone(P1,P2,R1,R2);
  TheError = Cone.Status();
  if (TheError == gce_Done) {
    Standard_Real D = (P2.Distance(P1))/cos((Cone.Value())->SemiAngle());
    TheCone=new Geom_RectangularTrimmedSurface(Cone.Value(),0.,2.*PI,0.,D,Standard_True,Standard_True);
  }
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const Handle(Geom_RectangularTrimmedSurface)& GC_MakeTrimmedCone::
       Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheCone;
}

const Handle(Geom_RectangularTrimmedSurface)& GC_MakeTrimmedCone::
       Operator() const 
{
  return Value();
}

GC_MakeTrimmedCone::
  operator Handle(Geom_RectangularTrimmedSurface) () const
{
  return Value();
}

