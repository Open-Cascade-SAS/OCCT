// File:	GeomInt_IntSS.cxx
// Created:	Fri Jan 27 10:46:42 1995
// Author:	Jacques GOUSSARD
// Copyright: Open CASCADE 1995

#include <GeomInt_IntSS.ixx>

#include <Adaptor3d_TopolTool.hxx>
#include <Standard_ConstructionError.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_HSurface.hxx>


//=======================================================================
//function : Perform
//purpose  : General intersection
//=======================================================================
  void GeomInt_IntSS::Perform(const Handle(Geom_Surface)& S1,
                              const Handle(Geom_Surface)& S2,
                              const Standard_Real Tol,
                              const Standard_Boolean Approx,
                              const Standard_Boolean ApproxS1,
                              const Standard_Boolean ApproxS2)
{
  myHS1 = new GeomAdaptor_HSurface(S1);
  if (S1==S2)
    myHS2 = myHS1;
  else
    myHS2 = new GeomAdaptor_HSurface(S2);
  InternalPerform(Tol,Approx,ApproxS1,ApproxS2,Standard_False,0.,0.,0.,0.);
}

//=======================================================================
//function : Perform
//purpose  : General intersection with a Starting Point 
//=======================================================================
  void GeomInt_IntSS::Perform(const Handle(Geom_Surface)& S1,
                              const Handle(Geom_Surface)& S2,
                              const Standard_Real Tol,
                              const Standard_Real U1, const Standard_Real V1,
                              const Standard_Real U2, const Standard_Real V2,
                              const Standard_Boolean Approx,
                              const Standard_Boolean ApproxS1,
                              const Standard_Boolean ApproxS2)
{
  myHS1 = new GeomAdaptor_HSurface(S1);
  if (S1==S2)
    myHS2 = myHS1;
  else
    myHS2 = new GeomAdaptor_HSurface(S2);
  InternalPerform(Tol,Approx,ApproxS1,ApproxS2,Standard_True,U1,V1,U2,V2);
}

//=======================================================================
//function : Internal Perform
//purpose  : 
//=======================================================================
  void GeomInt_IntSS::InternalPerform(const Standard_Real Tol,
                                      const Standard_Boolean Approx,
                                      const Standard_Boolean ApproxS1,
                                      const Standard_Boolean ApproxS2,
                                      const Standard_Boolean useStart,
                                      const Standard_Real U1,
                                      const Standard_Real V1,
                                      const Standard_Real U2,
                                      const Standard_Real V2)
{
  myTolReached2d = myTolReached3d = 0.0;
  myNbrestr = 0;
  sline.Clear();
  Handle(Adaptor3d_TopolTool) dom1 = new Adaptor3d_TopolTool(myHS1);
  Handle(Adaptor3d_TopolTool) dom2 = new Adaptor3d_TopolTool(myHS2);
  myLConstruct.Load(dom1,dom2,myHS1,myHS2);

  Standard_Real TolArc = Tol;
  Standard_Real TolTang = Tol;
  Standard_Real UVMaxStep = 0.001;
  Standard_Real Deflection = 0.1;

  myIntersector.SetTolerances(TolArc,TolTang,UVMaxStep,Deflection);

  if(myHS1 == myHS2) {
    myIntersector.Perform(myHS1,dom1,TolArc,TolTang);
  }
  else if (!useStart) {
    myIntersector.Perform(myHS1,dom1,myHS2,dom2,TolArc,TolTang);
  }
  else {
    myIntersector.Perform(myHS1,dom1,myHS2,dom2,U1,V1,U2,V2,TolArc,TolTang);
  }

  // ============================================================
  if (myIntersector.IsDone()) {
    const Standard_Integer nblin = myIntersector.NbLines();
    for (Standard_Integer i=1; i<= nblin; i++) {
      MakeCurve(i,dom1,dom2,Tol,Approx,ApproxS1,ApproxS2);
    }
  }
}

//=======================================================================
//function : Line
//purpose  : 
//=======================================================================
  const Handle(Geom_Curve) & GeomInt_IntSS::Line (const Standard_Integer Index) const
{
  StdFail_NotDone_Raise_if(!myIntersector.IsDone(),"GeomInt_IntSS::Line");
  return sline(Index+myNbrestr);
}

//=======================================================================
//function : Boundary
//purpose  : 
//=======================================================================
  const Handle(Geom_Curve) & GeomInt_IntSS::Boundary (const Standard_Integer Index) const
{
  StdFail_NotDone_Raise_if(!myIntersector.IsDone(),"GeomInt_IntSS::Line");
  Standard_OutOfRange_Raise_if(Index <= 0 || Index > myNbrestr,
			       "GeomInt_IntSS::Boundary");
  return sline(Index);
}

//=======================================================================
//function : Pnt2d
//purpose  : 
//=======================================================================
  gp_Pnt2d GeomInt_IntSS::Pnt2d(const Standard_Integer Index,
                                const Standard_Boolean OnFirst) const
{
  const IntPatch_Point&  thept = myIntersector.Point(Index);
  Standard_Real U,V;
  if (OnFirst)
    thept.ParametersOnS1(U,V);
  else
    thept.ParametersOnS2(U,V);
  return gp_Pnt2d(U,V);
}

//=======================================================================
//function : HasLineOnS1
//purpose  : 
//=======================================================================
  Standard_Boolean GeomInt_IntSS::HasLineOnS1(const Standard_Integer index) const 
{ 
  StdFail_NotDone_Raise_if(!myIntersector.IsDone(),"GeomInt_IntSS::HasLineOnS1");
  return (!slineS1(index).IsNull());
}

//=======================================================================
//function : HasLineOnS2
//purpose  : 
//=======================================================================
  Standard_Boolean GeomInt_IntSS::HasLineOnS2(const Standard_Integer index) const 
{ 
  StdFail_NotDone_Raise_if(!myIntersector.IsDone(),"GeomInt_IntSS::HasLineOnS2");
  return (!slineS2(index).IsNull());
}

//=======================================================================
//function : LineOnS1
//purpose  : 
//=======================================================================
  const Handle(Geom2d_Curve) & GeomInt_IntSS::LineOnS1(const Standard_Integer Index) const
{
  StdFail_NotDone_Raise_if(!myIntersector.IsDone(),"GeomInt_IntSS::LineOnS1");
  return slineS1(Index);
}

//=======================================================================
//function : LineOnS2
//purpose  : 
//=======================================================================
  const Handle(Geom2d_Curve) & GeomInt_IntSS::LineOnS2(const Standard_Integer Index) const
{
  StdFail_NotDone_Raise_if(!myIntersector.IsDone(),"GeomInt_IntSS::LineOnS2");
  return slineS2(Index);
}
