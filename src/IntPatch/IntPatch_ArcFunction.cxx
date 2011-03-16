// File:	IntPatch_ArcFunction.cxx
// Created:	Mon Jun  7 17:39:24 1993
// Author:	Jacques GOUSSARD
// Copyright:	OPEN CASCADE 1993

#include <IntPatch_ArcFunction.ixx>

#include <IntPatch_HInterTool.hxx>


IntPatch_ArcFunction::IntPatch_ArcFunction ()
{}

Standard_Boolean IntPatch_ArcFunction::Value(const Standard_Real X, Standard_Real& F)
{
  gp_Pnt2d p2d(myArc->Value(X));
  mySurf->D0(p2d.X(),p2d.Y(),ptsol);
  F = myQuad.Distance(ptsol);
  return Standard_True;
}

Standard_Boolean IntPatch_ArcFunction::Derivative(const Standard_Real X, Standard_Real& D)
{
  gp_Pnt2d p2d;
  gp_Vec2d d2d;
  gp_Vec v,d1u,d1v;
  myArc->D1(X,p2d,d2d);
  mySurf->D1(p2d.X(),p2d.Y(),ptsol,d1u,d1v);
  v.SetLinearForm(d2d.X(),d1u,d2d.Y(),d1v);
  D = v.Dot(myQuad.Gradient(ptsol));
  return Standard_True;
}

Standard_Boolean IntPatch_ArcFunction::Values(const Standard_Real X, Standard_Real& F, Standard_Real& D)
{
  gp_Pnt2d p2d;
  gp_Vec2d d2d;
  gp_Vec d1u,d1v;

  gp_Vec v1,v2;
  myArc->D1(X,p2d,d2d);
  mySurf->D1(p2d.X(),p2d.Y(),ptsol,d1u,d1v);
  v1.SetLinearForm(d2d.X(),d1u,d2d.Y(),d1v);

  myQuad.ValAndGrad(ptsol,F,v2);
  D = v1.Dot(v2);
  return Standard_True;
}

Standard_Integer IntPatch_ArcFunction::GetStateNumber ()
{
  seqpt.Append(ptsol);
  return seqpt.Length();
}

Standard_Integer IntPatch_ArcFunction::NbSamples () const
{
  return Max(Max(IntPatch_HInterTool::NbSamplesU(mySurf,0.,0.),
		 IntPatch_HInterTool::NbSamplesV(mySurf,0.,0.)),
	     IntPatch_HInterTool::NbSamplesOnArc(myArc));
}
