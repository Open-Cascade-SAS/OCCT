// File:	GC_MakeSegment.cxx
// Created:	Fri Oct  2 16:37:54 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeSegment.ixx>
#include <GC_MakeLine.hxx>
#include <Geom_Line.hxx>
#include <StdFail_NotDone.hxx>
#include <ElCLib.hxx>

GC_MakeSegment::GC_MakeSegment(const gp_Pnt& P1 ,
				 const gp_Pnt& P2 ) 
{
  Standard_Real dist = P1.Distance(P2);
  Handle(Geom_Line) L = GC_MakeLine(P1,P2);
  TheSegment = new Geom_TrimmedCurve(L,0.,dist,Standard_True);
  TheError = gce_Done;
}

GC_MakeSegment::GC_MakeSegment(const gp_Lin& Line    ,
				 const gp_Pnt& Point   ,
				 const Standard_Real U ) 
{
  Standard_Real Ufirst = ElCLib::Parameter(Line,Point);
  Handle(Geom_Line) L = new Geom_Line(Line);
  TheSegment=new Geom_TrimmedCurve(L,Ufirst,U,Standard_True);
  TheError = gce_Done;
}

GC_MakeSegment::GC_MakeSegment(const gp_Lin& Line  ,
				 const gp_Pnt& P1    ,
				 const gp_Pnt& P2    ) 
{
  Standard_Real Ufirst = ElCLib::Parameter(Line,P1);
  Standard_Real Ulast = ElCLib::Parameter(Line,P2);
  Handle(Geom_Line) L = new Geom_Line(Line);
  TheSegment = new Geom_TrimmedCurve(L,Ufirst,Ulast,Standard_True);
  TheError = gce_Done;
}

GC_MakeSegment::GC_MakeSegment(const gp_Lin& Line  ,
				 const Standard_Real    U1    ,
				 const Standard_Real    U2    ) 
{
  Handle(Geom_Line) L = new Geom_Line(Line);
  TheSegment = new Geom_TrimmedCurve(L,U1,U2,Standard_True);
  TheError = gce_Done;
}

const Handle(Geom_TrimmedCurve)& GC_MakeSegment::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheSegment;
}

const Handle(Geom_TrimmedCurve)& GC_MakeSegment::Operator() const 
{
  return Value();
}

GC_MakeSegment::operator Handle(Geom_TrimmedCurve) () const
{
  return Value();
}

