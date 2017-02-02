// Created on: 2004-06-15
// Created by: Sergey ZARITCHNY
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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


#include <BinTools.hxx>
#include <BinTools_LocationSet.hxx>
#include <gp_Ax3.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_OutOfRange.hxx>
#include <TopLoc_Location.hxx>

//=======================================================================
//function : operator >> (gp_Trsf& T)
//purpose  : 
//=======================================================================
static Standard_IStream& operator >>(Standard_IStream& IS, gp_Trsf& T)
{
  Standard_Real V1[3],V2[3],V3[3];
  Standard_Real V[3];
  
  BinTools::GetReal(IS, V1[0]);
  BinTools::GetReal(IS, V1[1]);
  BinTools::GetReal(IS, V1[2]);
  BinTools::GetReal(IS,  V[0]);

  BinTools::GetReal(IS, V2[0]);
  BinTools::GetReal(IS, V2[1]);
  BinTools::GetReal(IS, V2[2]);
  BinTools::GetReal(IS,  V[1]);

  BinTools::GetReal(IS, V3[0]);
  BinTools::GetReal(IS, V3[1]);
  BinTools::GetReal(IS, V3[2]);
  BinTools::GetReal(IS,  V[2]);

  T.SetValues(V1[0],V1[1],V1[2],V[0],
	      V2[0],V2[1],V2[2],V[1],
	      V3[0],V3[1],V3[2],V[2]);
  return IS;
}

//=======================================================================
//function : operator << (gp_Trsf& T)
//purpose  : 
//=======================================================================
static Standard_OStream& operator <<(Standard_OStream& OS,const gp_Trsf& T)
{
  gp_XYZ V = T.TranslationPart();
  gp_Mat M = T.VectorialPart();

  BinTools::PutReal(OS, M(1,1));
  BinTools::PutReal(OS, M(1,2));
  BinTools::PutReal(OS, M(1,3));
  BinTools::PutReal(OS,V.Coord(1)); 

  BinTools::PutReal(OS, M(2,1));
  BinTools::PutReal(OS, M(2,2));
  BinTools::PutReal(OS, M(2,3));
  BinTools::PutReal(OS,V.Coord(2));  

  BinTools::PutReal(OS, M(3,1));
  BinTools::PutReal(OS, M(3,2));
  BinTools::PutReal(OS, M(3,3));
  BinTools::PutReal(OS,V.Coord(3));  

  return OS;
}

//=======================================================================
//function : BinTools_LocationSet
//purpose  : 
//=======================================================================

BinTools_LocationSet::BinTools_LocationSet() 
{
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void  BinTools_LocationSet::Clear()
{
  myMap.Clear();
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

Standard_Integer  BinTools_LocationSet::Add(const TopLoc_Location& L)
{
  if (L.IsIdentity()) return 0;
  Standard_Integer n = myMap.FindIndex(L);
  if (n > 0) return n;
  TopLoc_Location N = L;
  do {
    myMap.Add(N.FirstDatum());
    N = N.NextLocation();
  } while (!N.IsIdentity());
  return myMap.Add(L);
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

const TopLoc_Location&  BinTools_LocationSet::Location
  (const Standard_Integer I)const 
{
  static TopLoc_Location identity;
  if (I == 0) return identity;
  else return myMap(I);
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer  BinTools_LocationSet::Index(const TopLoc_Location& L) const
{
  if (L.IsIdentity()) return 0;
  return myMap.FindIndex(L);
}

//=======================================================================
//function : NbLocations
//purpose  : 
//=======================================================================

Standard_Integer  BinTools_LocationSet::NbLocations() const
{
  return myMap.Extent();
}

//=======================================================================
//function : Write locations
//purpose  : 
//=======================================================================

void  BinTools_LocationSet::Write(Standard_OStream& OS) const 
{
  
  Standard_Integer i, nbLoc = myMap.Extent();
  OS << "Locations "<< nbLoc << "\n";
  try {
    OCC_CATCH_SIGNALS
    for (i = 1; i <= nbLoc; i++) {
      TopLoc_Location L = myMap(i);
      
      TopLoc_Location L2 = L.NextLocation();
      Standard_Boolean simple = L2.IsIdentity();
      Standard_Integer p = L.FirstPower();
      TopLoc_Location L1 = L.FirstDatum();
      Standard_Boolean elementary = (simple && p == 1);
      if (elementary) {
	
	OS.put((Standard_Byte)1); // 1
	OS << L.Transformation();
      }
      else {
	OS.put((Standard_Byte)2); // 2
	BinTools::PutInteger(OS, myMap.FindIndex(L1));
	BinTools::PutInteger(OS, p);
	while (!L2.IsIdentity()) {
	  L1 = L2.FirstDatum();
	  p  = L2.FirstPower();
	  L2 = L2.NextLocation();
	  BinTools::PutInteger(OS,  myMap.FindIndex(L1));
	  BinTools::PutInteger(OS, p);
	}
	
	BinTools::PutInteger(OS, 0);
      }
    }
  }
  catch(Standard_Failure const& anException) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_LocatioSet::Write(..)" << endl;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================
void  BinTools_LocationSet::Read(Standard_IStream& IS)
{

  myMap.Clear();
  char buffer[255];
  Standard_Integer l1,p;

  IS >> buffer;
  if (IS.fail() || (strcmp(buffer,"Locations"))) {
    Standard_SStream aMsg;
    aMsg << "BinTools_LocationSet::Read: Not a location table"<<endl;
    throw Standard_Failure(aMsg.str().c_str());
    return;
  }

  Standard_Integer i, nbLoc;
  IS >> nbLoc;
  IS.get();// remove lf
  TopLoc_Location L;
  gp_Trsf T;

  try {
    OCC_CATCH_SIGNALS
    for (i = 1; i <= nbLoc; i++) { 
      
      const Standard_Byte aTypLoc = (Standard_Byte)IS.get();
      if (aTypLoc == 1) {
	IS >> T;
	L = T;
      }
      
      else if (aTypLoc == 2) {
	L = TopLoc_Location();
	BinTools::GetInteger(IS, l1); //Index
	while (l1 != 0) { 
	  BinTools::GetInteger(IS, p);
	  TopLoc_Location L1 = myMap(l1);
	  L = L1.Powered(p) *L;
	  BinTools::GetInteger(IS, l1);
	}
      } else {
	Standard_SStream aMsg;
	aMsg << "Unexpected location's type = " << aTypLoc << endl;
	throw Standard_Failure(aMsg.str().c_str());
      }    
      if (!L.IsIdentity()) myMap.Add(L);
    }
  }
  catch(Standard_Failure const& anException) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_LocationSet::Read(..)" << endl;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}
