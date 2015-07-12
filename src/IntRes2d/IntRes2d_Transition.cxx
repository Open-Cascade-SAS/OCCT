// Created on: 1992-06-10
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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


#include <IntRes2d_Transition.hxx>
#include <Standard_DomainError.hxx>

IntRes2d_Transition::IntRes2d_Transition() : tangent(Standard_True),
                                             posit(IntRes2d_Middle),
                                             typetra(IntRes2d_Undecided),
                                             situat(IntRes2d_Unknown),
                                             oppos(Standard_False)
{
}


ostream& operator << (ostream& os, IntRes2d_Transition& Trans) {

  os << "   Position : ";
  if (Trans.PositionOnCurve()==IntRes2d_Head) {
    os << "Debut\n";
  }
  else if (Trans.PositionOnCurve()==IntRes2d_Middle) {
    os << "Milieu\n";
  }
  else {
    os << "Fin\n";
  }

  os << "   Type de transition : ";
  if (Trans.TransitionType()==IntRes2d_Undecided) {
    os << "Indeterminee\n";
  }
  else {
    if (Trans.TransitionType()==IntRes2d_In) {
      os << "Entrante\n";
    }
    else if (Trans.TransitionType()==IntRes2d_Out) {
      os << "Sortante\n";
    }
    else {
      os << "Touch\n";
      os << "     Position par rapport a l'autre courbe : ";
      if (Trans.Situation()==IntRes2d_Inside) {
	os << "Interieure\n";
      }
      else if (Trans.Situation()==IntRes2d_Outside) {
	os << "Exterieure\n";
      }
      else if (Trans.Situation()==IntRes2d_Unknown) {
	os << "Indeterminee\n";
      }
      os << "   Position matiere : ";
      if (Trans.IsOpposite()) {
	os << "Opposee\n";
      }
      else {
	os << "Idem\n";
      }
    }
    os << "   Cas de tangence : ";
    if (Trans.IsTangent()) {
      os << "Oui\n";
    }
    else {
      os << "Non\n";
    }
  }
  os << "\n";
  return os;
}



