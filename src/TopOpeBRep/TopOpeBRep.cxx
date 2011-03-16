// File:	TopOpeBRep.cxx
// Created:	Tue Aug  2 16:02:44 1994
// Author:	Jean Yves LEBEY
//		<jyl@meteox>
// Modifed:     Portage NT 7-5-97 DPF (strcasecmp)

#include <TopOpeBRep.ixx>

//#ifdef WNT
//#define strcasecmp strcmp Already defined
//#endif

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRep::Print
  (const TopOpeBRep_TypeLineCurve t, Standard_OStream& OS)
{
  switch (t) {
  case TopOpeBRep_ANALYTIC    : OS << "ANALYTIC"; break;
  case TopOpeBRep_RESTRICTION : OS << "RESTRICTION"; break;
  case TopOpeBRep_WALKING     : OS << "WALKING"; break;
  case TopOpeBRep_LINE        : OS << "LINE"; break;
  case TopOpeBRep_CIRCLE      : OS << "CIRCLE"; break;
  case TopOpeBRep_ELLIPSE     : OS << "ELLIPSE"; break;
  case TopOpeBRep_PARABOLA    : OS << "PARABOLA"; break;
  case TopOpeBRep_HYPERBOLA   : OS << "HYPERBOLA"; break;
  case TopOpeBRep_OTHERTYPE   : OS << "OTHERTYPE"; break;
  default                     : OS << "UNKNOWN"; break;  
  }
  return OS;
}
