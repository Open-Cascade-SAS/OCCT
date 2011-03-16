// File:	GC_MakeArcOfEllipse.cxx
// Created:	Fri Oct  2 16:31:13 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeArcOfEllipse.ixx>
#include <Geom_Ellipse.hxx>
#include <StdFail_NotDone.hxx>
#include <ElCLib.hxx>

GC_MakeArcOfEllipse::GC_MakeArcOfEllipse(const gp_Elips& Elips   ,
					   const gp_Pnt&    P1   ,
					   const gp_Pnt&    P2   ,
					   const Standard_Boolean  Sense  ) 
{
  Standard_Real Alpha1 = ElCLib::Parameter(Elips,P1);
  Standard_Real Alpha2 = ElCLib::Parameter(Elips,P2);
  Handle(Geom_Ellipse) E = new Geom_Ellipse(Elips);
  TheArc = new Geom_TrimmedCurve(E,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}

GC_MakeArcOfEllipse::GC_MakeArcOfEllipse(const gp_Elips& Elips   ,
					   const gp_Pnt&   P     ,
					   const Standard_Real      Alpha ,
					   const Standard_Boolean   Sense ) 
{
  Standard_Real Alphafirst = ElCLib::Parameter(Elips,P);
  Handle(Geom_Ellipse) E = new Geom_Ellipse(Elips);
  TheArc = new Geom_TrimmedCurve(E,Alphafirst,Alpha,Sense);
  TheError = gce_Done;
}

GC_MakeArcOfEllipse::GC_MakeArcOfEllipse(const gp_Elips& Elips   ,
					   const Standard_Real     Alpha1 ,
					   const Standard_Real     Alpha2 ,
					   const Standard_Boolean  Sense  ) 
{
  Handle(Geom_Ellipse) E = new Geom_Ellipse(Elips);
  TheArc = new Geom_TrimmedCurve(E,Alpha1,Alpha2,Sense);
  TheError = gce_Done;
}

const Handle(Geom_TrimmedCurve)& GC_MakeArcOfEllipse::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheArc;
}

const Handle(Geom_TrimmedCurve)& GC_MakeArcOfEllipse::Operator() const 
{
  return Value();
}

GC_MakeArcOfEllipse::operator Handle(Geom_TrimmedCurve) () const
{
  return Value();
}




