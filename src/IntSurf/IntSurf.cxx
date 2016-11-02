// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <IntSurf.hxx>

#include <Adaptor3d_HSurface.hxx>
#include <IntSurf_Transition.hxx>
#include <Precision.hxx>
#include <gp_Vec.hxx>

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

//=======================================================================
//function : SetPeriod
//purpose  : 
//=======================================================================
void IntSurf::SetPeriod(const Handle(Adaptor3d_HSurface)& theFirstSurf,
                        const Handle(Adaptor3d_HSurface)& theSecondSurf,
                        Standard_Real theArrOfPeriod[4])
{
  theArrOfPeriod[0] = theFirstSurf->IsUPeriodic()? theFirstSurf->UPeriod() : 0.0;
  theArrOfPeriod[1] = theFirstSurf->IsVPeriodic()? theFirstSurf->VPeriod() : 0.0;
  theArrOfPeriod[2] = theSecondSurf->IsUPeriodic()? theSecondSurf->UPeriod() : 0.0;
  theArrOfPeriod[3] = theSecondSurf->IsVPeriodic()? theSecondSurf->VPeriod() : 0.0;
}


