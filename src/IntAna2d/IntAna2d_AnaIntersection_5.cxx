//============================================ IntAna2d_AnaIntersection_5.cxx
//============================================================================
#include <IntAna2d_AnaIntersection.jxx>

#include <IntAna2d_Outils.hxx>

void IntAna2d_AnaIntersection::Perform(const gp_Circ2d& Circle,
				  const IntAna2d_Conic& Conic)
{
   Standard_Boolean CIsDirect = Circle.IsDirect();
   Standard_Real A,B,C,D,E,F;
   Standard_Real pcc,pss,p2sc,pc,ps,pcte;
   Standard_Real radius=Circle.Radius();
   Standard_Real radius_P2=radius*radius;
   Standard_Integer i;
   Standard_Real tx,ty,S;

   done = Standard_False;
   nbp  = 0;
   para = Standard_False;
   empt = Standard_False;
   iden = Standard_False;

   gp_Ax2d Axe_rep(Circle.XAxis());
    
   Conic.Coefficients(A,B,C,D,E,F);
   Conic.NewCoefficients(A,B,C,D,E,F,Axe_rep);
   
   // Parametre a avec x=Radius Cos(a)  et y=Radius Sin(a)

   pss = B*radius_P2;
   pcc = A*radius_P2 - pss;                            // COS ^2
   p2sc =C*radius_P2;                                  // 2 SIN COS
   pc  = 2.0*D*radius;                                 // COS
   ps  = 2.0*E*radius;                                 // SIN
   pcte= F + pss;                                      // 1

   math_TrigonometricFunctionRoots Sol(pcc,p2sc,pc,ps,pcte,0.0,2.0*PI);

   if(!Sol.IsDone()) {
     cout << "\n\nmath_TrigonometricFunctionRoots -> NotDone\n\n"<<endl;
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
       S = Sol.Value(i);
       tx= radius*Cos(S); 
       ty= radius*Sin(S); 
       Coord_Ancien_Repere(tx,ty,Axe_rep);
       if(!CIsDirect) 
	 S = PI+PI-S;
       lpnt[i-1].SetValue(tx,ty,S);
     }        
     Traitement_Points_Confondus(nbp,lpnt);
   }		       
   done=Standard_True;
 }
 

