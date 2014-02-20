// Created on: 1993-03-03
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

// Update:      Frederic Maupas

#include <MgtTopLoc.ixx>

// Used for testing DownCast time
#define MgtTopLocSpeedDownCast

#ifdef chrono
#include<OSD_Timer.hxx>
extern OSD_Timer LocTimer;
#endif

//=======================================================================
//function : Translate
//purpose  : ... from Transient to Persistent
//=======================================================================

Handle(PTopLoc_Datum3D)  
     MgtTopLoc::Translate(const Handle(TopLoc_Datum3D)& D,
			  PTColStd_TransientPersistentMap& aMap)
{

  Handle(PTopLoc_Datum3D) PD;
  if (aMap.IsBound(D)) {
#ifdef MgtTopLocSpeedDownCast
    Handle(Standard_Persistent) aPers = aMap.Find(D);
    PD = (Handle(PTopLoc_Datum3D)&) aPers;
#else
    PD = Handle(PTopLoc_Datum3D)::DownCast(aMap.Find(D));
#endif
  }
  else {
    PD = new PTopLoc_Datum3D(D->Transformation());
    aMap.Bind(D,PD);
  }

  return PD;
}


//=======================================================================
//function : Translate
//purpose  : ... from Persistent to Transient
//=======================================================================

Handle(TopLoc_Datum3D)  
     MgtTopLoc::Translate(const Handle(PTopLoc_Datum3D)& D,
			  PTColStd_PersistentTransientMap& aMap)
{
  Handle(TopLoc_Datum3D) TD;
  if (aMap.IsBound(D)) {
#ifdef MgtTopLocSpeedDownCast
    Handle(Standard_Transient) aTrans = aMap.Find(D);
    TD = (Handle(TopLoc_Datum3D)&) aTrans;
#else
    TD = Handle(TopLoc_Datum3D)::DownCast(aMap.Find(D));
#endif
  }
  else {
    TD = new TopLoc_Datum3D(D->Transformation());
    aMap.Bind(D, TD);
  }

  return TD;
}

//=======================================================================
//function : Translate
//purpose  : .. from Transient to Persistent
//=======================================================================

PTopLoc_Location  
MgtTopLoc::Translate(const TopLoc_Location& L,
		     PTColStd_TransientPersistentMap& aMap)
{
  PTopLoc_Location result;
  if (!L.IsIdentity()) {
    result = PTopLoc_Location(Translate(L.FirstDatum(), aMap),
			      L.FirstPower(),
			      Translate(L.NextLocation(), aMap));
  }

  return result;
}


//=======================================================================
//function : Translate
//purpose  : .. from Persistent to Transient
//=======================================================================

TopLoc_Location 
MgtTopLoc::Translate(const PTopLoc_Location& L,
		     PTColStd_PersistentTransientMap& aMap)
{
#ifdef chrono
  LocTimer.Start();
#endif

  TopLoc_Location result;
  if (!L.IsIdentity()) {
    result = Translate(L.Next(),aMap) * 
      TopLoc_Location(Translate(L.Datum3D(),aMap)).Powered(L.Power());
  }
#ifdef chrono
  LocTimer.Stop();
#endif

  return result;
}


