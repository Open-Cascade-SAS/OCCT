// File:	TopOpeBRepTool.cxx
// Created:	Thu Mar 10 12:13:09 1994
// Author:	Jean Yves LEBEY
//		<jyl@phobox>

#include <TopOpeBRepTool.hxx>

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepTool::Print(const TopOpeBRepTool_OutCurveType t,
					Standard_OStream& OS)
{
//#ifdef DEB
  switch (t) {
  case TopOpeBRepTool_BSPLINE1 : OS << "BSPLINE1"; break;
  case TopOpeBRepTool_APPROX   : OS << "APPROX"; break;
  case TopOpeBRepTool_INTERPOL : OS << "INTERPOL"; break;
  default                      : OS << "UNKNOWN"; break;  
  }
//#endif
  return OS;
}
