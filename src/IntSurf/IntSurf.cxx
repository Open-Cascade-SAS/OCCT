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



#include <IntSurf.ixx>

#include <Precision.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <IntSurf_Transition.hxx>

//--------------------------------------------------------------
//-- IntSurf::MakeTransition(Vtgint,Vtgrst,Normale,Transline,Transarc);
//-- 


//-- tgFirst   = Tangente Ligne Intersection
//-- tgSecond  = Tangenet Restriction
//-- Normale   = Normale a la surface
void IntSurf::MakeTransition (const gp_Vec& TgFirst,
			      const gp_Vec& TgSecond,
			      const gp_Dir& Normale,
			      IntSurf_Transition& TFirst,
			      IntSurf_Transition& TSecond)

{
  
      
  // Effectuer le produit mixte normale, tangente 1, tangente 2
  // pour avoir le type de la transition.
      
  gp_Vec pvect(TgSecond.Crossed(TgFirst));
      
  Standard_Real NTgSecond = TgSecond.Magnitude();
  Standard_Real NTgFirst  = TgFirst.Magnitude();
  Standard_Real NTgSecondNTgFirstAngular = NTgSecond*NTgFirst*Precision::Angular();

  if(NTgFirst <= Precision::Confusion()) { 
    TFirst.SetValue(Standard_True,IntSurf_Undecided);
    TSecond.SetValue(Standard_True,IntSurf_Undecided);
  }
  else if (   (NTgSecond <= Precision::Confusion()) 
	   || (pvect.Magnitude()<= NTgSecondNTgFirstAngular)) {
    TFirst.SetValue(Standard_True,IntSurf_Unknown,TgFirst.Dot(TgSecond)<0.0);
    TSecond.SetValue(Standard_True,IntSurf_Unknown,TgFirst.Dot(TgSecond)<0.0);
  }
  else { 
    Standard_Real yu = pvect.Dot(Normale);
    yu/=NTgSecond*NTgFirst;
    if (yu>0.0001) {
      TFirst.SetValue(Standard_False,IntSurf_In);
      TSecond.SetValue(Standard_False,IntSurf_Out);
    }
    else if(yu<-0.0001) {
      TFirst.SetValue(Standard_False,IntSurf_Out);
      TSecond.SetValue(Standard_False,IntSurf_In);
    }
    else {
#if 0 
      //-- MODIF XAB
      gp_Vec V1(TgSecond.X() / NTgSecond,TgSecond.Y() / NTgSecond, TgSecond.Z() / NTgSecond);
      gp_Vec V2(TgFirst.X() / NTgFirst,TgFirst.Y() / NTgFirst, TgFirst.Z() / NTgFirst);
      
      pvect = V1.Crossed(V2);
      yu = pvect.Dot(Normale);

      if (yu>0.0000001) {
	TFirst.SetValue(Standard_False,IntSurf_In);
	TSecond.SetValue(Standard_False,IntSurf_Out);
      }
      else if(yu<-0.0000001) {
	TFirst.SetValue(Standard_False,IntSurf_Out);
	TSecond.SetValue(Standard_False,IntSurf_In);
      }
      else { 
	TFirst.SetValue(Standard_True,IntSurf_Undecided);
	TSecond.SetValue(Standard_True,IntSurf_Undecided);
      }
      
#else 
      TFirst.SetValue(Standard_True,IntSurf_Undecided);
      TSecond.SetValue(Standard_True,IntSurf_Undecided);
      
#endif
      


    }
  }
}





