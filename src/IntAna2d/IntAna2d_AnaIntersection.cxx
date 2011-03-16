// File IntAna2d_AnaIntersection.cxx, JAG le 6 Juin 1991

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



