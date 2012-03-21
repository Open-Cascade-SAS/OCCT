// Copyright (c) 1995-1999 Matra Datavision
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


#include <IntAna2d_AnaIntersection.ixx>

#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

IntAna2d_AnaIntersection::IntAna2d_AnaIntersection () {

  done = Standard_False;
}

IntAna2d_AnaIntersection::IntAna2d_AnaIntersection (const gp_Lin2d& L1,
						    const gp_Lin2d& L2) {
  Perform(L1,L2);
}

IntAna2d_AnaIntersection::IntAna2d_AnaIntersection (const gp_Circ2d& C1,
						    const gp_Circ2d& C2) {
  Perform(C1,C2);
}


IntAna2d_AnaIntersection::IntAna2d_AnaIntersection (const gp_Lin2d& L,
						    const gp_Circ2d& C) { 
  Perform(L,C);
}


IntAna2d_AnaIntersection::IntAna2d_AnaIntersection (const gp_Lin2d& L,
						    const IntAna2d_Conic& Conic) {
  Perform(L,Conic);
}

IntAna2d_AnaIntersection::IntAna2d_AnaIntersection (const gp_Parab2d& P,
						    const IntAna2d_Conic& Conic) {
  Perform(P,Conic);
}

IntAna2d_AnaIntersection::IntAna2d_AnaIntersection (const gp_Circ2d& C,
						    const IntAna2d_Conic& Conic) {
  Perform(C,Conic);
}

IntAna2d_AnaIntersection::IntAna2d_AnaIntersection (const gp_Elips2d& E,
						    const IntAna2d_Conic& Conic) {
  Perform(E,Conic);
}


IntAna2d_AnaIntersection::IntAna2d_AnaIntersection (const gp_Hypr2d& E,
						    const IntAna2d_Conic& Conic)
{
  Perform(E,Conic);
}



