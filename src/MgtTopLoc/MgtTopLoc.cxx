// Created on: 1993-03-03
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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


