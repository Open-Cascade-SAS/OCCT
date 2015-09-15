// Created on: 1994-08-02
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

// Modifed:     Portage NT 7-5-97 DPF (strcasecmp)

#include <TopOpeBRep.hxx>

//#ifdef _WIN32
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
