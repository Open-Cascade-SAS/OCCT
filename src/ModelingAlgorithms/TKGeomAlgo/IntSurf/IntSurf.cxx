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

#include <Adaptor3d_Surface.hxx>
#include <IntSurf_Transition.hxx>
#include <Precision.hxx>
#include <gp_Vec.hxx>

//--------------------------------------------------------------
//-- IntSurf::MakeTransition(Vtgint,Vtgrst,Normale,Transline,Transarc);
//--
//-- tgFirst   = Tangente Ligne Intersection
//-- tgSecond  = Tangenet Restriction
//-- Normale   = Normale a la surface
void IntSurf::MakeTransition(const gp_Vec&       TgFirst,
                             const gp_Vec&       TgSecond,
                             const gp_Dir&       Normale,
                             IntSurf_Transition& TFirst,
                             IntSurf_Transition& TSecond)

{

  // Compute the mixed product of normal, tangent 1, tangent 2
  // to get the type of transition.

  gp_Vec pvect(TgSecond.Crossed(TgFirst));

  double NTgSecond                = TgSecond.Magnitude();
  double NTgFirst                 = TgFirst.Magnitude();
  double NTgSecondNTgFirstAngular = NTgSecond * NTgFirst * Precision::Angular();

  if (NTgFirst <= Precision::Confusion())
  {
    TFirst.SetValue(true, IntSurf_Undecided);
    TSecond.SetValue(true, IntSurf_Undecided);
  }
  else if ((NTgSecond <= Precision::Confusion()) || (pvect.Magnitude() <= NTgSecondNTgFirstAngular))
  {
    TFirst.SetValue(true, IntSurf_Unknown, TgFirst.Dot(TgSecond) < 0.0);
    TSecond.SetValue(true, IntSurf_Unknown, TgFirst.Dot(TgSecond) < 0.0);
  }
  else
  {
    double yu = pvect.Dot(Normale);
    yu /= NTgSecond * NTgFirst;
    if (yu > 0.0001)
    {
      TFirst.SetValue(false, IntSurf_In);
      TSecond.SetValue(false, IntSurf_Out);
    }
    else if (yu < -0.0001)
    {
      TFirst.SetValue(false, IntSurf_Out);
      TSecond.SetValue(false, IntSurf_In);
    }
    else
    {
#if 0 
      //-- MODIF XAB
      gp_Vec V1(TgSecond.X() / NTgSecond,TgSecond.Y() / NTgSecond, TgSecond.Z() / NTgSecond);
      gp_Vec V2(TgFirst.X() / NTgFirst,TgFirst.Y() / NTgFirst, TgFirst.Z() / NTgFirst);
      
      pvect = V1.Crossed(V2);
      yu = pvect.Dot(Normale);

      if (yu>0.0000001) {
	TFirst.SetValue(false,IntSurf_In);
	TSecond.SetValue(false,IntSurf_Out);
      }
      else if(yu<-0.0000001) {
	TFirst.SetValue(false,IntSurf_Out);
	TSecond.SetValue(false,IntSurf_In);
      }
      else { 
	TFirst.SetValue(true,IntSurf_Undecided);
	TSecond.SetValue(true,IntSurf_Undecided);
      }

#else
      TFirst.SetValue(true, IntSurf_Undecided);
      TSecond.SetValue(true, IntSurf_Undecided);

#endif
    }
  }
}

//=================================================================================================

void IntSurf::SetPeriod(const occ::handle<Adaptor3d_Surface>& theFirstSurf,
                        const occ::handle<Adaptor3d_Surface>& theSecondSurf,
                        double                                theArrOfPeriod[4])
{
  theArrOfPeriod[0] = theFirstSurf->IsUPeriodic() ? theFirstSurf->UPeriod() : 0.0;
  theArrOfPeriod[1] = theFirstSurf->IsVPeriodic() ? theFirstSurf->VPeriod() : 0.0;
  theArrOfPeriod[2] = theSecondSurf->IsUPeriodic() ? theSecondSurf->UPeriod() : 0.0;
  theArrOfPeriod[3] = theSecondSurf->IsVPeriodic() ? theSecondSurf->VPeriod() : 0.0;
}
