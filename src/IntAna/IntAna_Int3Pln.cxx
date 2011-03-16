//-- File : IntAna_Int3Pln.cxx


#ifndef DEB
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif

#include <IntAna_Int3Pln.ixx>
#include <StdFail_NotDone.hxx>
#include <Standard_DomainError.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <gp.hxx>

IntAna_Int3Pln::IntAna_Int3Pln () : done(Standard_False) {}


IntAna_Int3Pln::IntAna_Int3Pln (const gp_Pln& P1, const gp_Pln& P2,
				const gp_Pln& P3) {

  Perform(P1,P2,P3);
}

void IntAna_Int3Pln::Perform (const gp_Pln& P1, const gp_Pln& P2,
			      const gp_Pln& P3) {

  done=Standard_False;
  static math_Matrix M(1,3,1,3);
  static math_Vector V(1,3);
  
  P1.Coefficients(M(1,1),M(1,2),M(1,3),V(1));
  P2.Coefficients(M(2,1),M(2,2),M(2,3),V(2));
  P3.Coefficients(M(3,1),M(3,2),M(3,3),V(3));
  
  math_Gauss Resol(M,gp::Resolution());
  
  if (!Resol.IsDone()) {
    empt=Standard_True;
  }
  else {
    empt=Standard_False;
    V=-V;
    Resol.Solve(V);
    pnt.SetCoord(V(1),V(2),V(3));
  }
  done=Standard_True;
}



