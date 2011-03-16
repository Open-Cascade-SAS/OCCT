// File:	BinTools_LocationSet.cxx
// Created:	Tue Jun 15 12:04:06 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004

#include <BinTools_LocationSet.ixx>
#include <BinTools.hxx>
#include <gp_Ax3.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>

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
	      V3[0],V3[1],V3[2],V[2],
	      Precision::Angular(),
	      Precision::Confusion());
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
  OS << "Locations "<< nbLoc <<endl; 
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
  catch(Standard_Failure) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_LocatioSet::Write(..)" << endl;
    Handle(Standard_Failure) anExc = Standard_Failure::Caught();
    aMsg << anExc << endl;
    Standard_Failure::Raise(aMsg);
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
    Standard_Failure::Raise(aMsg);
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
      
      const Standard_Byte aTypLoc = IS.get();
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
	Standard_Failure::Raise(aMsg);
      }    
      if (!L.IsIdentity()) myMap.Add(L);
    }
  }
  catch(Standard_Failure) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_LocationSet::Read(..)" << endl;
    Handle(Standard_Failure) anExc = Standard_Failure::Caught();
    aMsg << anExc << endl;
    Standard_Failure::Raise(aMsg);
  }
}
