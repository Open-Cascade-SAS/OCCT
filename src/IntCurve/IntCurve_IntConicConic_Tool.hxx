// File:	IntCurve_IntConicConic_Tool.hxx
// Created:	Wed May  6 16:02:53 1992
// Author:	Laurent BUCHARD
//		<lbr@topsn3>



#ifndef IntCurve_IntConicConic_Tool_HeaderFile
#define IntCurve_IntConicConic_Tool_HeaderFile


#include <IntCurve_IntImpConicParConic.hxx>
#include <IntCurve_IConicTool.hxx>
#include <IntCurve_PConic.hxx>
// #include <IntCurve_IConicPConicTool.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_Transition.hxx>
#include <IntRes2d_Position.hxx>
#include <gp_Vec2d.hxx>
#include <gp.hxx>


static Standard_Real PIpPI = Standard_PI+Standard_PI;

//======================================================================
//==========          P R O T O T Y P E S                   ============
//======================================================================

void Determine_Transition_LC(const IntRes2d_Position,
			     gp_Vec2d&,
			     const gp_Vec2d&,
			     IntRes2d_Transition&,
			     const IntRes2d_Position,
			     gp_Vec2d&,
			     const gp_Vec2d&,
			     IntRes2d_Transition&,
			     const Standard_Real);
//======================================================================
Standard_Real NormalizeOnCircleDomain(const Standard_Real Param,const IntRes2d_Domain& Domain);
//=====================================================================
//====   C l a s s e     I n t e r v a l      !! Specifique !! ========
//=====================================================================
class Interval {
 public:
  Standard_Real Binf;
  Standard_Real Bsup;
  Standard_Boolean HasFirstBound;
  Standard_Boolean HasLastBound;
  Standard_Boolean IsNull;
  
  Interval();
  Interval(const Standard_Real a,const Standard_Real b);
  Interval(const IntRes2d_Domain& Domain);
  Interval( const Standard_Real a,const Standard_Boolean hf
	   ,const Standard_Real b,const Standard_Boolean hl);
  Standard_Real Length(); 
  Interval IntersectionWithBounded(const Interval& Inter);
};



//======================================================================
//==  C L A S S E    P E R I O D I C    I N T E R V A L  (Specifique)
//======================================================================
class PeriodicInterval {
 public:
  Standard_Real Binf;
  Standard_Real Bsup;
  Standard_Boolean isnull;

  void SetNull()      { isnull=Standard_True; }
  Standard_Boolean IsNull()    { return(isnull); }

  void Complement()   { if(!isnull) {
                          Standard_Real t=Binf; Binf=Bsup; Bsup=t+PIpPI;
			  if(Binf>PIpPI) {
			    Binf-=PIpPI;
			    Bsup-=PIpPI;
			  }
			}
		      }

  Standard_Real Length()       { return((isnull)? -100.0: Abs(Bsup-Binf)); }


  PeriodicInterval(const IntRes2d_Domain& Domain) {  
                        isnull=Standard_False;
                        if(Domain.HasFirstPoint())  
			  Binf=Domain.FirstParameter();
			else
			  Binf=-1.0;
			if(Domain.HasLastPoint()) 
			  Bsup=Domain.LastParameter();
			else
			  Bsup=20.0;
		      }
  PeriodicInterval()  { isnull=Standard_True; Binf=Bsup=0.0; }
  PeriodicInterval(const Standard_Real a,const Standard_Real b) 
                      {
			isnull=Standard_False;
			Binf=a;
			Bsup=b;
			if((b-a) < PIpPI)
			  this->Normalize();
		      }
  void SetValues(const Standard_Real a,const Standard_Real b)
                      {
			isnull=Standard_False;
			Binf=a;
			Bsup=b;
			if((b-a) < PIpPI)
			  this->Normalize();
		      }
  void Normalize() {
                   if(!isnull) {
		     while(Binf>PIpPI) Binf-=PIpPI;
		     while(Binf<0.0)   Binf+=PIpPI;
		     while(Bsup<Binf)  Bsup+=PIpPI;
		     while(Bsup>=(Binf+PIpPI)) Bsup-=PIpPI;
		   }
		 }
    
  PeriodicInterval FirstIntersection(PeriodicInterval& I1);
  PeriodicInterval SecondIntersection(PeriodicInterval& I2);
};


#endif
