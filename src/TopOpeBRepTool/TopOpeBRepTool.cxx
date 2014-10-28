// Created on: 1994-03-10
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <TopOpeBRepTool.hxx>

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepTool::Print(const TopOpeBRepTool_OutCurveType t,
					Standard_OStream& OS)
{
//#ifdef OCCT_DEBUG
  switch (t) {
  case TopOpeBRepTool_BSPLINE1 : OS << "BSPLINE1"; break;
  case TopOpeBRepTool_APPROX   : OS << "APPROX"; break;
  case TopOpeBRepTool_INTERPOL : OS << "INTERPOL"; break;
  default                      : OS << "UNKNOWN"; break;  
  }
//#endif
  return OS;
}
