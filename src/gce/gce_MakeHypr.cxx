// File:	gce_MakeElips.cxx
// Created:	Wed Sep  2 12:35:36 1992
// Author:	Remi GILET
//		<reg@sdsun1>

#include <gce_MakeHypr.ixx>
#include <StdFail_NotDone.hxx>
#include <gp_Lin.hxx>

//=========================================================================
//   Creation d une Hyperbole 3d de gp de centre <Center> et de sommets   +
//   <S1> et <S2>.                                                        +
//   <CenterS1> donne le grand axe .                                      +
//   <S1> donne le grand rayon et <S2> le petit rayon.                    +
//=========================================================================

gce_MakeHypr::gce_MakeHypr(const gp_Pnt&   S1     ,
			   const gp_Pnt&   S2     ,
			   const gp_Pnt&   Center ) 
{
  gp_Dir XAxis(gp_XYZ(S1.XYZ()-Center.XYZ()));
  gp_Lin L(Center,XAxis);
  Standard_Real D = S1.Distance(Center);
  Standard_Real d = L.Distance(S2);
  if (d > D) { TheError = gce_InvertAxis; }
  else {
    gp_Dir Norm(XAxis.Crossed(gp_Dir(gp_XYZ(S2.XYZ()-Center.XYZ()))));
    TheHypr = gp_Hypr(gp_Ax2(Center,Norm,XAxis),D,d);
    TheError = gce_Done;
  }
}

gce_MakeHypr::gce_MakeHypr(const gp_Ax2&       A2          ,
			   const Standard_Real MajorRadius ,
			   const Standard_Real MinorRadius ) 
{
  if (MajorRadius < MinorRadius) { TheError = gce_InvertRadius; }
  else if (MajorRadius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    TheHypr = gp_Hypr(A2,MajorRadius,MinorRadius);
    TheError = gce_Done;
  }
}

const gp_Hypr& gce_MakeHypr::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheHypr;
}

const gp_Hypr& gce_MakeHypr::Operator() const 
{
  return Value();
}

gce_MakeHypr::operator gp_Hypr() const
{
  return Value();
}

