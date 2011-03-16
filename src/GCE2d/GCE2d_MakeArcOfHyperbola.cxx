// File:	GCE2d_MakeArcOfHyperbola.cxx
// Created:	Fri Oct  2 16:31:29 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GCE2d_MakeArcOfHyperbola.ixx>
#include <StdFail_NotDone.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <ElCLib.hxx>

GCE2d_MakeArcOfHyperbola::GCE2d_MakeArcOfHyperbola(const gp_Hypr2d&     Hypr ,
						 const gp_Pnt2d&        P1   ,
						 const gp_Pnt2d&        P2   ,
						 const Standard_Boolean Sense)
{
  Standard_Real Alpha1 = ElCLib::Parameter(Hypr,P1);
  Standard_Real Alpha2 = ElCLib::Parameter(Hypr,P2);
  Handle(Geom2d_Hyperbola) H = new Geom2d_Hyperbola(Hypr);
  TheArc = new Geom2d_TrimmedCurve(H,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}

GCE2d_MakeArcOfHyperbola::
  GCE2d_MakeArcOfHyperbola(const gp_Hypr2d&       Hypr  ,
			   const gp_Pnt2d&        P     ,
			   const Standard_Real    Alpha ,
			   const Standard_Boolean Sense )
{
  Standard_Real Alphafirst = ElCLib::Parameter(Hypr,P);
  Handle(Geom2d_Hyperbola) H = new Geom2d_Hyperbola(Hypr);
  TheArc = new Geom2d_TrimmedCurve(H,Alphafirst,Alpha,Sense);
  TheError = gce_Done;
}

GCE2d_MakeArcOfHyperbola::
  GCE2d_MakeArcOfHyperbola(const gp_Hypr2d&       Hypr ,
			   const Standard_Real    Alpha1,
			   const Standard_Real    Alpha2,
			   const Standard_Boolean Sense )
{
  Handle(Geom2d_Hyperbola) H = new Geom2d_Hyperbola(Hypr);
  TheArc = new Geom2d_TrimmedCurve(H,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}

const Handle(Geom2d_TrimmedCurve)& GCE2d_MakeArcOfHyperbola::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheArc;
}

const Handle(Geom2d_TrimmedCurve)& GCE2d_MakeArcOfHyperbola::Operator() const 
{
  return Value();
}

GCE2d_MakeArcOfHyperbola::operator Handle(Geom2d_TrimmedCurve) () const
{
  return Value();
}

