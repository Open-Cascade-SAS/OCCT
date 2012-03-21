// Created on: 1992-06-10
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <IntRes2d_Transition.ixx>


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



