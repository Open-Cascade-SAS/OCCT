// File:	GCE2d_MakeArcOfParabola.cxx
// Created:	Fri Oct  2 16:31:46 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GCE2d_MakeArcOfParabola.ixx>
#include <Geom2d_Parabola.hxx>
#include <StdFail_NotDone.hxx>
#include <ElCLib.hxx>

GCE2d_MakeArcOfParabola::
  GCE2d_MakeArcOfParabola(const gp_Parab2d&       Parab ,
			  const gp_Pnt2d&         P1    ,
			  const gp_Pnt2d&         P2    ,
			  const Standard_Boolean  Sense  ) {
  Standard_Real Alpha1 = ElCLib::Parameter(Parab,P1);
  Standard_Real Alpha2 = ElCLib::Parameter(Parab,P2);
  Handle(Geom2d_Parabola) P = new Geom2d_Parabola(Parab);
  TheArc = new Geom2d_TrimmedCurve(P,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}

GCE2d_MakeArcOfParabola::
  GCE2d_MakeArcOfParabola(const gp_Parab2d&       Parab ,
			  const gp_Pnt2d&         P     ,
			  const Standard_Real     Alpha ,
			  const Standard_Boolean  Sense ) {
  Standard_Real Alphafirst = ElCLib::Parameter(Parab,P);
  Handle(Geom2d_Parabola) Parabola = new Geom2d_Parabola(Parab);
  TheArc = new Geom2d_TrimmedCurve(Parabola,Alphafirst,Alpha,Sense);
  TheError = gce_Done;
}

GCE2d_MakeArcOfParabola::
  GCE2d_MakeArcOfParabola(const gp_Parab2d&      Parab  ,
			  const Standard_Real    Alpha1 ,
			  const Standard_Real    Alpha2 ,
			  const Standard_Boolean Sense  ) 
{
  Handle(Geom2d_Parabola) P = new Geom2d_Parabola(Parab);
  TheArc = new Geom2d_TrimmedCurve(P,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}

const Handle(Geom2d_TrimmedCurve)& GCE2d_MakeArcOfParabola::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheArc;
}

const Handle(Geom2d_TrimmedCurve)& GCE2d_MakeArcOfParabola::Operator() const 
{
  return Value();
}

GCE2d_MakeArcOfParabola::operator Handle(Geom2d_TrimmedCurve) () const
{
  return Value();
}

