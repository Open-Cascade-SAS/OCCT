//============================================ IntAna2d_AnaIntersection_4.cxx
//============================================================================
#include <IntAna2d_AnaIntersection.jxx>
#include <IntAna2d_Outils.hxx>

void IntAna2d_AnaIntersection::Perform (const gp_Lin2d& L,
				   const IntAna2d_Conic& Conic)
{
  Standard_Real A,B,C,D,E,F;
  Standard_Real px0,px1,px2;
  Standard_Real DR_A,DR_B,DR_C,X0,Y0;
  Standard_Integer i;
  Standard_Real tx,ty,S;
  
  done = Standard_False;
  nbp  = 0;
  para = Standard_False;
  iden = Standard_False;
 
  Conic.Coefficients(A,B,C,D,E,F);
  L.Coefficients(DR_A,DR_B,DR_C);
  X0=L.Location().X();
  Y0=L.Location().Y();
  
  // Parametre: L
  // X = Xo - L DR_B    et     Y = Yo + L DR_A

  px0=F + X0*(D+D + A*X0 + 2.0*C*Y0) + Y0*(E+E + B*Y0);
  px1=2.0*(E*DR_A - D*DR_B + X0*(C*DR_A - A*DR_B) + Y0*(B*DR_A - C*DR_B));
  px2=DR_A*(B*DR_A - 2.0*C*DR_B) + A*(DR_B*DR_B);
  
  MyDirectPolynomialRoots Sol(px2,px1,px0);
  
  if(!Sol.IsDone()) {
    done=Standard_False;
    return;
  }
  else { 
    if(Sol.InfiniteRoots()) {
      iden=Standard_True;
      done=Standard_True;
      return;
    }
    nbp=Sol.NbSolutions();
    for(i=1;i<=nbp;i++) {
      S=Sol.Value(i);
      tx=X0 - S*DR_B;
      ty=Y0 + S*DR_A;
      lpnt[i-1].SetValue(tx,ty,S);
    }
    Traitement_Points_Confondus(nbp,lpnt);
  }
  done=Standard_True;
}
