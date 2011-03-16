// File:	HLRTest_Projector.cxx
// Created:	Wed Apr  5 16:58:12 1995
// Author:	Christophe MARION
//		<cma@ecolox>

#include <Standard_Stream.hxx>
#include <HLRTest_Projector.ixx>

//=======================================================================
//function : HLRTest_Projector
//purpose  : 
//=======================================================================

HLRTest_Projector::HLRTest_Projector (const HLRAlgo_Projector& P) :
myProjector(P)
{
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void HLRTest_Projector::DrawOn (Draw_Display&) const 
{
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) HLRTest_Projector::Copy () const 
{
  return new HLRTest_Projector(myProjector);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void HLRTest_Projector::Dump (Standard_OStream& S) const 
{
  S << "Projector : \n";
  if (myProjector.Perspective())
    S << "perspective, focal = " << myProjector.Focus() << "\n";

  for (Standard_Integer i = 1; i <= 3; i++) {

    for (Standard_Integer j = 1; j <= 4; j++) {
      S << setw(15) << myProjector.Transformation().Value(i,j);
    }
    S << "\n";
  }
  S << endl;
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void HLRTest_Projector::Whatis (Draw_Interpretor& I) const 
{
  I << "projector";
}

