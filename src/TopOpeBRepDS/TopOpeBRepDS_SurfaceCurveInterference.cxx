// File:	TopOpeBRepDS_SurfaceCurveInterference.cxx
// Created:	Wed Jun 23 19:13:45 1993
// Author:	Jean Yves LEBEY
//		<jyl@zerox>


#include <TopOpeBRepDS_SurfaceCurveInterference.ixx>

#ifdef DEB
#include <TopOpeBRepDS_Dumper.hxx>
#endif

//=======================================================================
//function : TopOpeBRepDS_SurfaceCurveInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceCurveInterference::TopOpeBRepDS_SurfaceCurveInterference()
{
}

//=======================================================================
//function : TopOpeBRepDS_SurfaceCurveInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceCurveInterference::TopOpeBRepDS_SurfaceCurveInterference
  (const TopOpeBRepDS_Transition& T,
   const TopOpeBRepDS_Kind ST, 
   const Standard_Integer S, 
   const TopOpeBRepDS_Kind GT, 
   const Standard_Integer G, 
   const Handle(Geom2d_Curve)& PC) :
  TopOpeBRepDS_Interference(T,ST,S,GT,G),
  myPCurve(PC)
{
}

//=======================================================================
//function : TopOpeBRepDS_SurfaceCurveInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceCurveInterference::TopOpeBRepDS_SurfaceCurveInterference
  (const Handle(TopOpeBRepDS_Interference)& I) :
  TopOpeBRepDS_Interference(I)
{
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)& TopOpeBRepDS_SurfaceCurveInterference::PCurve
       ()const 
{
  return myPCurve;
}

//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_SurfaceCurveInterference::PCurve
  (const Handle(Geom2d_Curve)& PC)
{
  myPCurve = PC;
}


//=======================================================================
//function : DumpPCurve
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_SurfaceCurveInterference::DumpPCurve
  (Standard_OStream& OS,
   const Standard_Boolean compact)const
{
#ifdef DEB
  Dump(OS); OS<<endl;

  OS<<"PCurve ";
  if (!PCurve().IsNull()) TopOpeBRepDS_Dumper::Print(PCurve(),OS,compact);
  else OS<<" is null";
  OS<<endl;
#endif

  return OS;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_SurfaceCurveInterference::Dump
  (Standard_OStream& OS) const
{
#ifdef DEB
  OS<<"SCI ";TopOpeBRepDS_Dumper::PrintType(myPCurve,OS);OS<<" ";
  TopOpeBRepDS_Interference::Dump(OS);
#endif

  return OS;
}
