//============================================================================
//======================================================= IntAna2d_Outils.hxx
//============================================================================
#ifndef IntAna2d_Outil_HeaderFile
#define IntAna2d_Outil_HeaderFile

#ifndef math_DirectPolynomialRoots_HeaderFile
#include <math_DirectPolynomialRoots.hxx>
#endif

#ifndef math_TrigonometricFunctionRoots_HeaderFile
#include <math_TrigonometricFunctionRoots.hxx>
#endif

#ifndef IntAna2d_IntPoint_HeaderFile
#include <IntAna2d_IntPoint.hxx>
#endif

#ifndef gp_Ax2d_HeaderFile
#include <gp_Ax2d.hxx>
#endif

class MyDirectPolynomialRoots { 
public:
  MyDirectPolynomialRoots(const Standard_Real A4,
			  const Standard_Real A3,
			  const Standard_Real A2,
			  const Standard_Real A1,
			  const Standard_Real A0);

  MyDirectPolynomialRoots(const Standard_Real A2,
			  const Standard_Real A1,
			  const Standard_Real A0); 
  
  Standard_Integer NbSolutions() const { return(nbsol); } 
  Standard_Real    Value(const Standard_Integer i) const { return(sol[i-1]); }
  Standard_Real    IsDone() const { return(nbsol>-1); }
  Standard_Boolean InfiniteRoots() const { return(same); } 
private:
  Standard_Real      sol[16];
  Standard_Real      val[16];
  Standard_Integer   nbsol;
  Standard_Boolean   same;
}; 
						     

Standard_Boolean Points_Confondus(const Standard_Real xa,const Standard_Real ya,
				  const Standard_Real xb,const Standard_Real yb);



void Traitement_Points_Confondus(Standard_Integer& nb_pts
				 ,IntAna2d_IntPoint *pts);

void Coord_Ancien_Repere(Standard_Real& Ancien_X,Standard_Real& Ancien_Y
                        ,const gp_Ax2d Axe_Nouveau_Repere);


#endif




