// File:	Approx_SweepApproximation.cxx
// Created:	Wed Jun 25 17:01:37 1997
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <Approx_SweepApproximation.ixx>
#include <gp_XYZ.hxx>
#include <BSplCLib.hxx>

#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_DichoCutting.hxx>
#include <AdvApprox_PrefAndRec.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>

#include <TColStd_Array1OfReal.hxx>

#include <StdFail_NotDone.hxx>

//=======================================================================
//class : Approx_SweepApproximation_Eval
//purpose: evaluator class for approximation
//=======================================================================

class Approx_SweepApproximation_Eval : public AdvApprox_EvaluatorFunction
{
 public:
  Approx_SweepApproximation_Eval (Approx_SweepApproximation& theTool)
    : Tool(theTool) {}
  
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  Approx_SweepApproximation &Tool;
};

void Approx_SweepApproximation_Eval::Evaluate (Standard_Integer *,/*Dimension*/
                                               Standard_Real     StartEnd[2],
                                               Standard_Real    *Parameter,
                                               Standard_Integer *DerivativeRequest,
                                               Standard_Real    *Result,// [Dimension]
                                               Standard_Integer *ErrorCode)
{
  *ErrorCode = Tool.Eval (*Parameter, *DerivativeRequest, 
                          StartEnd[0], StartEnd[1], Result[0]);
}

Approx_SweepApproximation::
Approx_SweepApproximation(const Handle(Approx_SweepFunction)& Func) 
{
  myFunc  = Func;
  //  Init des variables de controles
  myParam = 0;
  myOrder = -1;
  first = 1.e100; last = -1.e100;
  done = Standard_False;
}

void Approx_SweepApproximation::Perform(const Standard_Real First,
					const Standard_Real Last,
					const Standard_Real Tol3d,
					const Standard_Real BoundTol,
					const Standard_Real Tol2d,
					const Standard_Real TolAngular,
					const GeomAbs_Shape Continuity,
					const Standard_Integer Degmax,
					const Standard_Integer Segmax)
{
 Standard_Integer NbPolSect, NbKnotSect, ii;
 Standard_Real Tol, Tol3dMin = Tol3d, The3D2DTol=0 ; 
 GeomAbs_Shape   continuity = Continuity;

// (1) Caracteristiques d'une section
 myFunc->SectionShape(NbPolSect, NbKnotSect, udeg);
 Num2DSS = myFunc->Nb2dCurves();
 tabUKnots = new (TColStd_HArray1OfReal)    (1, NbKnotSect);
 tabUMults = new (TColStd_HArray1OfInteger) (1, NbKnotSect);
 myFunc->Knots(tabUKnots->ChangeArray1());
 myFunc->Mults(tabUMults->ChangeArray1());

// (2) Decompositition en sous espaces
 Handle(TColStd_HArray1OfReal) OneDTol, TwoDTol, ThreeDTol;
 Num3DSS = NbPolSect;

// (2.1) Tolerance 3d et 1d
 OneDTol = new (TColStd_HArray1OfReal) (1, Num3DSS);
 ThreeDTol = new (TColStd_HArray1OfReal) (1, Num3DSS);

 myFunc->GetTolerance(BoundTol, Tol3d, TolAngular, 
		      ThreeDTol->ChangeArray1());

 for (ii=1; ii<=Num3DSS; ii++) 
   if (ThreeDTol->Value(ii) < Tol3dMin) Tol3dMin = ThreeDTol->Value(ii);

 if (myFunc->IsRational()) {
   Standard_Real Size;
   Num1DSS = NbPolSect;
   TColStd_Array1OfReal Wmin(1, Num1DSS);
   myFunc->GetMinimalWeight(Wmin);
   Size =  myFunc->MaximalSection();
   Translation.SetXYZ
     (myFunc->BarycentreOfSurf().XYZ());
   for (ii=1; ii<=Num3DSS; ii++) {
     Tol =  ThreeDTol->Value(ii)/2; //Afin de respecter l'erreur sur le resultat final.
     OneDTol->SetValue(ii, Tol * Wmin(ii) / Size);
     Tol *= Wmin(ii); //Facteur de projection
     ThreeDTol->SetValue(ii, Max(Tol, 1.e-20) );
   }
 }
 else { Num1DSS = 0; }

 
// (2.2) Tolerance et Transformation 2d.
 if (Num2DSS == 0) {TwoDTol.Nullify();}
 else {
   // pour le 2d on definit une affinite a partir des resolutions, afin
   // d'avoir une tolerance d'approximation homogene (u/v et 2d/3d)
   Standard_Real res, tolu, tolv; 
   TwoDTol = new (TColStd_HArray1OfReal) (1, Num2DSS);
   AAffin = new (Approx_HArray1OfGTrsf2d) (1, Num2DSS);
   The3D2DTol= 0.9*BoundTol; // 10% de securite
   for (ii=1; ii<=Num2DSS; ii++) {
     myFunc->Resolution(ii, The3D2DTol, tolu, tolv);   
     if ( tolu> tolv ) {
       res = tolv;
       AAffin->ChangeValue(ii).SetValue(1,1, tolv/tolu); 
     }
     else {
       res = tolu;
       AAffin->ChangeValue(ii).SetValue(2,2, tolu/tolv);
     }
     TwoDTol->SetValue(ii, Min( Tol2d, res));
   }
 }

// (3) Approximation

// Init
 myPoles = new (TColgp_HArray1OfPnt)(1, Num3DSS);
 myDPoles = new (TColgp_HArray1OfVec)(1, Num3DSS);
 myD2Poles = new (TColgp_HArray1OfVec)(1, Num3DSS);

 myWeigths = new (TColStd_HArray1OfReal)(1, Num3DSS);
 myDWeigths = new (TColStd_HArray1OfReal)(1, Num3DSS);
 myD2Weigths = new (TColStd_HArray1OfReal)(1, Num3DSS);
 
 if (Num2DSS>0) {
   myPoles2d   = new (TColgp_HArray1OfPnt2d)(1, Num2DSS);
   myDPoles2d  = new (TColgp_HArray1OfVec2d)(1, Num2DSS);
   myD2Poles2d = new (TColgp_HArray1OfVec2d)(1, Num2DSS);  
   COnSurfErr  = new (TColStd_HArray1OfReal)(1, Num2DSS);
 }
// Controle que myFunc->D2 est implemente
 if (continuity >= GeomAbs_C2) {
   Standard_Boolean B;
   B = myFunc->D2(First, First, Last, 
		  myPoles->ChangeArray1(), myDPoles->ChangeArray1(), 
		  myD2Poles->ChangeArray1(),
		  myPoles2d->ChangeArray1(), myDPoles2d->ChangeArray1(), 
		  myD2Poles2d->ChangeArray1(),
		  myWeigths->ChangeArray1(), myDWeigths->ChangeArray1(),
		  myD2Weigths->ChangeArray1());
   if (!B) continuity =  GeomAbs_C1;
 }
// Controle que myFunc->D1 est implemente
 if (continuity == GeomAbs_C1) {
   Standard_Boolean B;
   B = myFunc->D1(First, First, Last, 
		  myPoles->ChangeArray1(), myDPoles->ChangeArray1(), 
		  myPoles2d->ChangeArray1(), myDPoles2d->ChangeArray1(), 
		  myWeigths->ChangeArray1(), myDWeigths->ChangeArray1());
   if (!B) continuity =  GeomAbs_C0;
 }

//Pour que F soit au moins 20 fois plus precise que son approx
 myFunc->SetTolerance(Tol3dMin/20, Tol2d/20);
 
 Standard_Integer NbIntervalC2 = myFunc->NbIntervals(GeomAbs_C2);
 Standard_Integer NbIntervalC3 = myFunc->NbIntervals(GeomAbs_C3);

 if (NbIntervalC3 > 1) {
// (3.1) Approximation avec decoupe preferentiel
   TColStd_Array1OfReal Param_de_decoupeC2 (1, NbIntervalC2+1);
   myFunc->Intervals(Param_de_decoupeC2, GeomAbs_C2);
   TColStd_Array1OfReal Param_de_decoupeC3 (1, NbIntervalC3+1);
   myFunc->Intervals(Param_de_decoupeC3, GeomAbs_C3);


   AdvApprox_PrefAndRec Preferentiel(Param_de_decoupeC2,
				     Param_de_decoupeC3);
   
   Approx_SweepApproximation_Eval ev (*this);
   Approximation(OneDTol,  TwoDTol, ThreeDTol,
		 The3D2DTol,
		 First,     Last, 
		 continuity,
		 Degmax,    Segmax, 
		 ev,
		 Preferentiel);
 }
 else {
// (3.2) Approximation sans decoupe preferentiel
   AdvApprox_DichoCutting Dichotomie;
   Approx_SweepApproximation_Eval ev (*this);
   Approximation(OneDTol,  TwoDTol, ThreeDTol,
		 The3D2DTol,
		 First,     Last, 
		 continuity,
		 Degmax,    Segmax, 
		 ev,
		 Dichotomie);
 }
}

//========================================================================
//function : Approximation
//purpose  : Appel F(t) et stocke les resultats
//========================================================================
void Approx_SweepApproximation::
Approximation(const Handle(TColStd_HArray1OfReal)& OneDTol,
	      const Handle(TColStd_HArray1OfReal)& TwoDTol,
	      const Handle(TColStd_HArray1OfReal)& ThreeDTol,
	      const Standard_Real BoundTol,
	      const Standard_Real First,const Standard_Real Last,
	      const GeomAbs_Shape Continuity,const Standard_Integer Degmax,
	      const Standard_Integer Segmax,
	      const AdvApprox_EvaluatorFunction& TheApproxFunction,
	      const AdvApprox_Cutting& TheCuttingTool) 
{
 AdvApprox_ApproxAFunction Approx(Num1DSS, 
				  Num2DSS,
				  Num3DSS,
				  OneDTol,
				  TwoDTol,
				  ThreeDTol,
				  First,
				  Last, 
				  Continuity,
				  Degmax,
				  Segmax, 
				  TheApproxFunction,
				  TheCuttingTool);
 done = Approx.HasResult();

 if (done) {
 // --> Remplissage des Champs de la surface ----
   Standard_Integer ii, jj;

   vdeg = Approx.Degree();
   // Malheureusement Adv_Approx stock la transpose de 
   // ce que l'on souhaite, donc l'ecriture
   // tabPoles = Approx.Poles() donnerait un resultat errone
   // Il n'y a plus qu'a allouer et recopier termes a termes...
   tabPoles = new (TColgp_HArray2OfPnt)
     (1, Num3DSS, 1, Approx.NbPoles());
   tabWeights = new (TColStd_HArray2OfReal)
     (1, Num3DSS, 1, Approx.NbPoles());

   if (Num1DSS == Num3DSS) {
     Standard_Real wpoid;
     gp_Pnt P;
     for (ii=1; ii <=Num3DSS; ii++) {
       for (jj=1; jj <=Approx.NbPoles() ; jj++) {
         P = Approx.Poles()->Value(jj,ii);
         wpoid = Approx.Poles1d()->Value(jj,ii);
         P.ChangeCoord() /= wpoid; // Il faut diviser les poles par les poids
         P.Translate(Translation); 
	 tabPoles->SetValue  (ii, jj, P);
	 tabWeights->SetValue(ii, jj, wpoid );
       }
     }
   }
   else {
     tabWeights->Init(1);
     for (ii=1; ii <=Num3DSS; ii++) {
       for (jj=1; jj <=Approx.NbPoles() ; jj++) {
	 tabPoles->SetValue  (ii, jj, Approx.Poles  ()->Value(jj,ii) );
       }
     }
   }
   
   // ici cela va mieux
   tabVKnots = Approx.Knots();
   tabVMults = Approx.Multiplicities();

   

  // --> Remplissage des courbes 2d  ----------
   if (Num2DSS>0) {
     gp_GTrsf2d TrsfInv;
     deg2d = vdeg;
     tab2dKnots = Approx.Knots();
     tab2dMults = Approx.Multiplicities();

     for (ii=1; ii<=Num2DSS; ii++) {
       TrsfInv = AAffin->Value(ii).Inverted();
       Handle(TColgp_HArray1OfPnt2d) P2d = 
	 new (TColgp_HArray1OfPnt2d) (1, Approx.NbPoles());
       Approx.Poles2d( ii, P2d->ChangeArray1() );
       // On n'oublie pas d'appliquer l'homothetie inverse.
       for (jj=1; jj<=Approx.NbPoles(); jj++) {
	  TrsfInv.Transforms(P2d->ChangeValue(jj).ChangeCoord());
	}
       seqPoles2d.Append(P2d);
     }
   }
  // ---> Remplissage des erreurs
   MError3d = new (TColStd_HArray1OfReal) (1,Num3DSS);
   AError3d = new (TColStd_HArray1OfReal) (1,Num3DSS);
   for (ii=1; ii<=Num3DSS; ii++) {
     MError3d->SetValue(ii, Approx.MaxError(3, ii));
     AError3d->SetValue(ii, Approx.AverageError(3, ii));
   }

   if (myFunc->IsRational()) {
     MError1d = new (TColStd_HArray1OfReal) (1,Num3DSS);
     AError1d = new (TColStd_HArray1OfReal) (1,Num3DSS);
     for (ii=1; ii<=Num1DSS; ii++) {
       MError1d->SetValue(ii, Approx.MaxError(1, ii));
       AError1d->SetValue(ii, Approx.AverageError(1, ii));
     }
   }

   if (Num2DSS>0) {
     tab2dError = new (TColStd_HArray1OfReal) (1,Num2DSS);
     Ave2dError = new (TColStd_HArray1OfReal) (1,Num2DSS);
     for (ii=1; ii<=Num2DSS; ii++) {
       tab2dError->SetValue(ii, Approx.MaxError(2, ii));
       Ave2dError->SetValue(ii, Approx.AverageError(2, ii));
       COnSurfErr->SetValue(ii,
	  (tab2dError->Value(ii)/TwoDTol->Value(ii))*BoundTol);
     }
   }     
 }
}

Standard_Integer Approx_SweepApproximation::Eval(const Standard_Real Parameter,
						 const Standard_Integer DerivativeRequest,
						 const Standard_Real First,
						 const Standard_Real Last,
						 Standard_Real& Result) 
{
 Standard_Integer ier=0;
 switch (DerivativeRequest) {
 case 0 : 
   ier = ( ! D0(Parameter, First, Last, Result));
   break;
 case 1 : 
   ier = ( ! D1(Parameter, First, Last, Result));
   break;
 case 2 : 
   ier = ( ! D2(Parameter, First, Last,Result));
   break;
 default :
   ier = 2;
 }
 return ier;
}

Standard_Boolean Approx_SweepApproximation::D0(const Standard_Real Param,
					       const Standard_Real First,
					       const Standard_Real Last,
					       Standard_Real& Result) 
{
  Standard_Integer index, ii;
  Standard_Boolean Ok=Standard_True;
  Standard_Real * LocalResult =  &Result;

  // Gestion des Bornes
  if ((first!=First) || (Last!=last)) {
       myFunc->SetInterval(First, Last);
     }

  if (! ( (Param==myParam) && (myOrder>=0)
	 && (first==First) && (Last==last)) ) {
    // Positionement dans le cas ou l'on ne repete pas 
   //  la derniere operation
    Ok = myFunc->D0(Param, First, Last,
		    myPoles->ChangeArray1(),
		    myPoles2d->ChangeArray1(),
		    myWeigths->ChangeArray1());

    //  On multiplie les poles3d par les poids apres tranlations.
    for (ii=1; ii<=Num1DSS; ii++) {
      myPoles->ChangeValue(ii).ChangeCoord()
	-= Translation.XYZ();
      myPoles->ChangeValue(ii).ChangeCoord() 
	*= myWeigths->Value(ii);
    }

    //  On applique la transformation aux poles 2d.
    for (ii=1; ii<=Num2DSS; ii++) {
      AAffin->Value(ii).Transforms(myPoles2d->ChangeValue(ii).ChangeCoord());
    }

    // Mise a jour des variable de controles et retour
    first = First;
    last  = Last;
    myOrder = 0;
    myParam = Param;
  }

  //  Extraction des resultats
  index = 0;
  for (ii=1; ii<=Num1DSS; ii++) {
    LocalResult[index] = myWeigths->Value(ii);
    index++;
  }
  for (ii=1; ii<=Num2DSS; ii++) {
    LocalResult[index] =    myPoles2d->Value(ii).X();
    LocalResult[index+1] =  myPoles2d->Value(ii).Y();
    index += 2;
  }
  for (ii=1; ii<=Num3DSS; ii++, index+=3) {
    LocalResult[index]   = myPoles->Value(ii).X();
    LocalResult[index+1] = myPoles->Value(ii).Y();
    LocalResult[index+2] = myPoles->Value(ii).Z();
  }

  return Ok;
}

Standard_Boolean Approx_SweepApproximation::D1(const Standard_Real Param,
					       const Standard_Real First,
					       const Standard_Real Last,
					       Standard_Real& Result) 
{
  gp_XY Vcoord;
  gp_Vec Vaux;
  Standard_Integer index, ii;
  Standard_Boolean Ok=Standard_True;
  Standard_Real * LocalResult =  &Result;


  if ((first!=First) || (Last!=last)) {
    myFunc->SetInterval(First, Last);
  }

  if (! ( (Param==myParam) && (myOrder>=1)
	 && (first==First) && (Last==last)) ){

    // Positionement 
    Ok = myFunc->D1(Param, First, Last,
		    myPoles->ChangeArray1(),
		    myDPoles->ChangeArray1(),
		    myPoles2d->ChangeArray1(),
		    myDPoles2d->ChangeArray1(),
		    myWeigths->ChangeArray1(),
		    myDWeigths->ChangeArray1());

    //  On tient compte de la multiplication des poles3d par les poids.
    //  et de la translation.
    for ( ii=1; ii<=Num1DSS; ii++) {
      //Translation sur la section
      myPoles->ChangeValue(ii).ChangeCoord()
	-= Translation.XYZ();
      // Homothetie sur tout
      myDPoles->ChangeValue(ii) *= myWeigths->Value(ii);
      Vaux.SetXYZ( myPoles->Value(ii).Coord());
      myDPoles->ChangeValue(ii) += myDWeigths->Value(ii)*Vaux;
      myPoles->ChangeValue(ii).ChangeCoord() 
	*= myWeigths->Value(ii); // Pour le cache
    }
      

    //  On applique les transformation 2d aux vecteurs idoines
    for (ii=1; ii<=Num2DSS; ii++) {
      Vcoord =  myDPoles2d->Value(ii).XY();
      AAffin->Value(ii).Transforms(Vcoord);
      myDPoles2d->ChangeValue(ii).SetXY(Vcoord);
      AAffin->Value(ii).Transforms(myPoles2d->ChangeValue(ii).ChangeCoord());
    }

    // Mise a jour des variable de controles et retour
    first = First;
    last  = Last;
    myOrder = 1;
    myParam = Param;
  }
  
  //  Extraction des resultats
  index = 0;
  for (ii=1; ii<=Num1DSS; ii++) {
    LocalResult[index] = myDWeigths->Value(ii);
    index++;
  }
  for (ii=1; ii<=Num2DSS; ii++) {
    LocalResult[index] =    myDPoles2d->Value(ii).X();
    LocalResult[index+1] =  myDPoles2d->Value(ii).Y();
    index += 2;
  }
  for (ii=1; ii<=Num3DSS; ii++, index+=3) {
    LocalResult[index]   = myDPoles->Value(ii).X();
    LocalResult[index+1] = myDPoles->Value(ii).Y();
    LocalResult[index+2] = myDPoles->Value(ii).Z();
  }
  return Ok;
}

Standard_Boolean Approx_SweepApproximation::D2(const Standard_Real Param,
					       const Standard_Real First,
					       const Standard_Real Last,
					       Standard_Real& Result) 
{
  gp_XY Vcoord;  
  gp_Vec Vaux;
  Standard_Integer index, ii;
  Standard_Boolean Ok=Standard_True;
  Standard_Real * LocalResult =  &Result;

  // Gestion des Bornes
  if ((first!=First) || (Last!=last)) {
       myFunc->SetInterval(First, Last);
     }

  if (! ( (Param==myParam) && (myOrder>=2)
	 && (first==First) && (Last==last)) ) {
    // Positionement dans le cas ou l'on ne repete pas 
   //  la derniere operation
    Ok = myFunc->D2(Param, First, Last,
		    myPoles->ChangeArray1(),
		    myDPoles->ChangeArray1(),
		    myD2Poles->ChangeArray1(),
		    myPoles2d->ChangeArray1(),
		    myDPoles2d->ChangeArray1(),
		    myD2Poles2d->ChangeArray1(),
		    myWeigths->ChangeArray1(),
		    myDWeigths->ChangeArray1(),
		    myD2Weigths->ChangeArray1());

    //  On multiplie les poles3d par les poids apres tranlations.
    for (ii=1; ii<=Num1DSS; ii++) {
      // D'abord on translate
      myPoles->ChangeValue(ii).ChangeCoord()
	-= Translation.XYZ();
	
      //On calcul la derive seconde
      myD2Poles->ChangeValue(ii) *= myWeigths->Value(ii);
      Vaux.SetXYZ( myDPoles->Value(ii).XYZ());
      myD2Poles->ChangeValue(ii) += (2*myDWeigths->Value(ii))*Vaux;
      Vaux.SetXYZ( myPoles->Value(ii).Coord());
      myD2Poles->ChangeValue(ii) += myD2Weigths->Value(ii)*Vaux;

      //Puis le reste pour le cache
      myDPoles->ChangeValue(ii) *= myWeigths->Value(ii);
      Vaux.SetXYZ( myPoles->Value(ii).Coord());
      myDPoles->ChangeValue(ii) += myDWeigths->Value(ii)*Vaux;
      myPoles->ChangeValue(ii).ChangeCoord() 
	*= myWeigths->Value(ii); 
    }

    //  On applique la transformation aux poles 2d.
    for (ii=1; ii<=Num2DSS; ii++) {
      Vcoord =  myD2Poles2d->Value(ii).XY();
      AAffin->Value(ii).Transforms(Vcoord);
      myD2Poles2d->ChangeValue(ii).SetXY(Vcoord);
      Vcoord =  myDPoles2d->Value(ii).XY();
      AAffin->Value(ii).Transforms(Vcoord);
      myDPoles2d->ChangeValue(ii).SetXY(Vcoord);
      AAffin->Value(ii).Transforms(myPoles2d->ChangeValue(ii).ChangeCoord());
    }

    // Mise a jour des variable de controles et retour
    first = First;
    last  = Last;
    myOrder = 2;
    myParam = Param;
  }

  //  Extraction des resultats
  index = 0;
  for (ii=1; ii<=Num1DSS; ii++) {
    LocalResult[index] = myD2Weigths->Value(ii);
    index++;
  }
  for (ii=1; ii<=Num2DSS; ii++) {
    LocalResult[index] =    myD2Poles2d->Value(ii).X();
    LocalResult[index+1] =  myD2Poles2d->Value(ii).Y();
    index += 2;
  }
  for (ii=1; ii<=Num3DSS; ii++, index+=3) {
    LocalResult[index]   = myD2Poles->Value(ii).X();
    LocalResult[index+1] = myD2Poles->Value(ii).Y();
    LocalResult[index+2] = myD2Poles->Value(ii).Z();
  }

  return Ok;
}

void Approx_SweepApproximation::
SurfShape(Standard_Integer& UDegree,
	  Standard_Integer& VDegree,Standard_Integer& NbUPoles,
	  Standard_Integer& NbVPoles,
	  Standard_Integer& NbUKnots,
	  Standard_Integer& NbVKnots) const
{
  if (!done) {StdFail_NotDone::Raise("Approx_SweepApproximation");}
  UDegree  = udeg;
  VDegree  = vdeg;
  NbUPoles = tabPoles->ColLength();
  NbVPoles = tabPoles->RowLength();
  NbUKnots = tabUKnots->Length();
  NbVKnots = tabVKnots->Length();
}

void Approx_SweepApproximation::
Surface(TColgp_Array2OfPnt& TPoles,
	TColStd_Array2OfReal& TWeights,
	TColStd_Array1OfReal& TUKnots,
	TColStd_Array1OfReal& TVKnots,
	TColStd_Array1OfInteger& TUMults,
	TColStd_Array1OfInteger& TVMults) const
{
  if (!done) {StdFail_NotDone::Raise("Approx_SweepApproximation");}
  TPoles   = tabPoles->Array2();
  TWeights = tabWeights->Array2();
  TUKnots  = tabUKnots->Array1();
  TUMults  = tabUMults->Array1();
  TVKnots  = tabVKnots->Array1();
  TVMults  = tabVMults->Array1();
}

Standard_Real Approx_SweepApproximation::MaxErrorOnSurf() const
{
 Standard_Integer ii;
 Standard_Real MaxError = 0, err;
 if (!done) {StdFail_NotDone::Raise("Approx_SweepApproximation");}

 if (myFunc->IsRational()) {
   TColStd_Array1OfReal Wmin(1, Num1DSS);
   myFunc->GetMinimalWeight(Wmin);
   Standard_Real Size = myFunc->MaximalSection();
   for (ii=1; ii<=Num3DSS; ii++) {
     err = (Size*MError1d->Value(ii) + MError3d->Value(ii)) / Wmin(ii);
     if (err>MaxError) MaxError = err;
   }
 }
 else {
  for (ii=1; ii<=Num3DSS; ii++) {
     err = MError3d->Value(ii);
     if (err>MaxError) MaxError = err;
   } 
 } 
 return MaxError;
}

 Standard_Real Approx_SweepApproximation::AverageErrorOnSurf() const
{
 Standard_Integer ii;
 Standard_Real MoyError = 0, err;
 if (!done) {StdFail_NotDone::Raise("Approx_SweepApproximation");}

 if (myFunc->IsRational()) {
   TColStd_Array1OfReal Wmin(1, Num1DSS);
   myFunc->GetMinimalWeight(Wmin);
   Standard_Real Size = myFunc->MaximalSection();
   for (ii=1; ii<=Num3DSS; ii++) {
     err = (Size*AError1d->Value(ii) + AError3d->Value(ii)) / Wmin(ii);
     MoyError += err;
   }
 }
 else {
  for (ii=1; ii<=Num3DSS; ii++) {
     err = AError3d->Value(ii);
     MoyError += err;
   } 
 } 
 return MoyError/Num3DSS;
}


void Approx_SweepApproximation::Curves2dShape(Standard_Integer& Degree,
					      Standard_Integer& NbPoles,
					      Standard_Integer& NbKnots) const
{
  if (!done) {StdFail_NotDone::Raise("Approx_SweepApproximation");}
  if (seqPoles2d.Length() == 0) {Standard_DomainError::Raise("Approx_SweepApproximation");}
  Degree = deg2d;
  NbPoles = seqPoles2d(1)->Length();
  NbKnots = tab2dKnots->Length();
}

void Approx_SweepApproximation::Curve2d(const Standard_Integer Index,
					TColgp_Array1OfPnt2d& TPoles,
					TColStd_Array1OfReal& TKnots,
					TColStd_Array1OfInteger& TMults) const
{
  if (!done) {StdFail_NotDone::Raise("Approx_SweepApproximation");}
  if (seqPoles2d.Length() == 0) {Standard_DomainError::Raise("Approx_SweepApproximation");}
  TPoles = seqPoles2d(Index)->Array1();
  TKnots  = tab2dKnots->Array1();
  TMults  = tab2dMults->Array1(); 
}

 Standard_Real Approx_SweepApproximation::Max2dError(const Standard_Integer Index) const
{
  if (!done) {StdFail_NotDone::Raise("Approx_SweepApproximation");}
  return tab2dError->Value(Index);
}

 Standard_Real Approx_SweepApproximation::Average2dError(const Standard_Integer Index) const
{
  if (!done) {StdFail_NotDone::Raise("Approx_SweepApproximation");}
  return Ave2dError->Value(Index); 
}

Standard_Real Approx_SweepApproximation::TolCurveOnSurf(const Standard_Integer Index) const
{
  if (!done) {StdFail_NotDone::Raise("Approx_SweepApproximation");}
  return  COnSurfErr->Value(Index);
}

 void Approx_SweepApproximation::Dump(Standard_OStream& o) const
{
  o << "Dump of SweepApproximation" << endl;
  if (done) { 
    o << "Error 3d = " << MaxErrorOnSurf() << endl;

    if (Num2DSS>0) {
      o << "Error 2d = ";
      for (Standard_Integer ii=1; ii<=Num2DSS; ii++) 
	{  o << Max2dError(ii);
	   if (ii < Num2DSS) o << " , " << endl;
	 }
      cout << endl;
    }
    o <<  tabVKnots->Length()-1 <<" Segment(s) of degree " << vdeg << endl;
  }
  else cout << " Not Done " << endl;
}
