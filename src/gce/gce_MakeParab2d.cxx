// File:	gce_MakeParab2d.cxx
// Created:	Wed Sep  2 12:35:36 1992
// Author:	Remi GILET
//		<reg@sdsun1>

#include <gce_MakeParab2d.ixx>
#include <gp.hxx>
#include <StdFail_NotDone.hxx>

gce_MakeParab2d::gce_MakeParab2d(const gp_Ax22d&     A     ,
				 const Standard_Real Focal ) 
{
  if (Focal < 0.0) { TheError = gce_NullFocusLength; }
  else {
    TheParab2d = gp_Parab2d(A,Focal);
    TheError = gce_Done;
  }
}

gce_MakeParab2d::gce_MakeParab2d(const gp_Ax2d&         MirrorAxis ,
				 const Standard_Real    Focal      ,
				 const Standard_Boolean Sense      ) 
{
  if (Focal < 0.0) { TheError = gce_NullFocusLength; }
  else {
    TheParab2d = gp_Parab2d(MirrorAxis,Focal,Sense);
    TheError = gce_Done;
  }
}

gce_MakeParab2d::gce_MakeParab2d(const gp_Ax2d&  D            ,
				 const gp_Pnt2d& F            ,
				 const Standard_Boolean Sense )
{
  TheParab2d = gp_Parab2d(D,F,Sense);
  TheError = gce_Done;
}

gce_MakeParab2d::gce_MakeParab2d(const gp_Ax22d&  D ,
				 const gp_Pnt2d& F  )
{
  TheParab2d = gp_Parab2d(D,F);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une Parabole 2d de gp de centre <Center> et de sommet     +
//   <S1> .                                                               +
//   <CenterS1> donne le grand axe .                                      +
//   <S1> donne la focale.                                                +
//=========================================================================

gce_MakeParab2d::gce_MakeParab2d(const gp_Pnt2d&        S      ,
				 const gp_Pnt2d&        Center ,
				 const Standard_Boolean Sense  ) 
{
  if (S.Distance(Center) >= gp::Resolution()) {
    gp_Dir2d XAxis(gp_XY(S.XY()-Center.XY()));
    TheParab2d = gp_Parab2d(gp_Ax2d(Center,XAxis),S.Distance(Center),Sense);
    TheError = gce_Done;
  }
  else { TheError = gce_NullAxis; }
}

const gp_Parab2d& gce_MakeParab2d::Value () const
{
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheParab2d;
}

const gp_Parab2d& gce_MakeParab2d::Operator() const 
{
  return Value();
}

gce_MakeParab2d::operator gp_Parab2d() const
{
  return Value();
}

