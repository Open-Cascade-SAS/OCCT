// File:	GC_MakeArcOfHyperbola.cxx
// Created:	Fri Oct  2 16:31:29 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeArcOfHyperbola.ixx>
#include <Geom_Hyperbola.hxx>
#include <StdFail_NotDone.hxx>
#include <ElCLib.hxx>

GC_MakeArcOfHyperbola::
  GC_MakeArcOfHyperbola(const gp_Hypr& Hypr   ,
			 const gp_Pnt&  P1     ,
			 const gp_Pnt&  P2     ,
			 const Standard_Boolean  Sense  ) 
{
  Standard_Real Alpha1 = ElCLib::Parameter(Hypr,P1);
  Standard_Real Alpha2 = ElCLib::Parameter(Hypr,P2);
  Handle(Geom_Hyperbola) H = new Geom_Hyperbola(Hypr);
  TheArc = new Geom_TrimmedCurve(H,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}

GC_MakeArcOfHyperbola::GC_MakeArcOfHyperbola(const gp_Hypr& Hypr   ,
					       const gp_Pnt&  P     ,
					       const Standard_Real     Alpha ,
					       const Standard_Boolean  Sense ) 
{
  Standard_Real Alphafirst = ElCLib::Parameter(Hypr,P);
  Handle(Geom_Hyperbola) H = new Geom_Hyperbola(Hypr);
  TheArc = new Geom_TrimmedCurve(H,Alphafirst,Alpha,Sense);
  TheError = gce_Done;
}

GC_MakeArcOfHyperbola::GC_MakeArcOfHyperbola(const gp_Hypr&        Hypr   ,
					     const Standard_Real     Alpha1 ,
					     const Standard_Real     Alpha2 ,
					     const Standard_Boolean  Sense  ) 
{
  Handle(Geom_Hyperbola) H = new Geom_Hyperbola(Hypr);
  TheArc = new Geom_TrimmedCurve(H,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}

const Handle(Geom_TrimmedCurve)& GC_MakeArcOfHyperbola::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheArc;
}

const Handle(Geom_TrimmedCurve)& GC_MakeArcOfHyperbola::Operator() const 
{
  return Value();
}

GC_MakeArcOfHyperbola::operator Handle(Geom_TrimmedCurve) () const
{
  return Value();
}

