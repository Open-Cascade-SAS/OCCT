// Created on: 1993-07-16
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


#include <GeomTools.hxx>
#include <gp_Ax3.hxx>
#include <gp_Vec.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressSentry.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Stream.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_LocationSet.hxx>

//=======================================================================
//function : TopTools_LocationSet
//purpose  : 
//=======================================================================
TopTools_LocationSet::TopTools_LocationSet() 
{
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void  TopTools_LocationSet::Clear()
{
  myMap.Clear();
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

Standard_Integer  TopTools_LocationSet::Add(const TopLoc_Location& L)
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

const TopLoc_Location&  TopTools_LocationSet::Location
  (const Standard_Integer I)const 
{
  static TopLoc_Location identity;
  if (I <= 0 || I > myMap.Extent()) return identity;
  return myMap(I);
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer  TopTools_LocationSet::Index(const TopLoc_Location& L) const
{
  if (L.IsIdentity()) return 0;
  return myMap.FindIndex(L);
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

static void WriteTrsf(const gp_Trsf& T,
		      Standard_OStream& OS, 
		      const Standard_Boolean compact)
{
  gp_XYZ V = T.TranslationPart();
  gp_Mat M = T.VectorialPart();

  if (!compact) OS << "( ";
  OS << setw(15) << M(1,1) << " ";
  OS << setw(15) << M(1,2) << " ";
  OS << setw(15) << M(1,3) << " ";
  OS << setw(15) << V.Coord(1) << " ";
  if (!compact) OS << " )";
  OS << "\n";
  if (!compact) OS << "( ";
  OS << setw(15) << M(2,1) << " ";
  OS << setw(15) << M(2,2) << " ";
  OS << setw(15) << M(2,3) << " ";
  OS << setw(15) << V.Coord(2) << " ";
  if (!compact) OS << " )";
  OS << "\n";
  if (!compact) OS << "( ";
  OS << setw(15) << M(3,1) << " ";
  OS << setw(15) << M(3,2) << " ";
  OS << setw(15) << M(3,3) << " ";
  OS << setw(15) << V.Coord(3) << " ";
  if (!compact) OS << " )";
  OS << "\n";
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  TopTools_LocationSet::Dump(Standard_OStream& OS) const 
{
  Standard_Integer i, nbLoc = myMap.Extent();

  OS << "\n\n";
  OS << "\n -------";
  OS << "\n Dump of "<< nbLoc << " Locations";
  OS << "\n -------\n\n";
  
  for (i = 1; i <= nbLoc; i++) {
    TopLoc_Location L = myMap(i);
    OS << setw(5) << i << " : \n";
    
    TopLoc_Location L2 = L.NextLocation();
    Standard_Boolean simple = L2.IsIdentity();
    Standard_Integer p = L.FirstPower();
    TopLoc_Location L1 = L.FirstDatum();
    Standard_Boolean elementary = (simple && p == 1);
    if (elementary) {
      OS << "Elementary location\n";
    }
    else {
      OS << "Complex : L"<<myMap.FindIndex(L1);
      if (p != 1) OS <<"^"<<p;
      while (!L2.IsIdentity()) {
	L1 = L2.FirstDatum();
	p = L2.FirstPower();
	L2 = L2.NextLocation();
	OS << " * L" << myMap.FindIndex(L1);
	if (p != 1) OS << "^"<<p;
      }
      OS <<"\n";
    }
    WriteTrsf(L.Transformation(),OS,Standard_False);
  }
} 

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void  TopTools_LocationSet::Write(Standard_OStream& OS) const 
{
  
  std::streamsize prec = OS.precision(15);

  Standard_Integer i, nbLoc = myMap.Extent();
  OS << "Locations " << nbLoc << "\n";
  
  //OCC19559
  Message_ProgressSentry PS(GetProgress(), "Locations", 0, nbLoc, 1);
  for (i = 1; i <= nbLoc && PS.More(); i++, PS.Next()) {
    TopLoc_Location L = myMap(i);

    
    TopLoc_Location L2 = L.NextLocation();
    Standard_Boolean simple = L2.IsIdentity();
    Standard_Integer p = L.FirstPower();
    TopLoc_Location L1 = L.FirstDatum();
    Standard_Boolean elementary = (simple && p == 1);
    if (elementary) {
      OS << "1\n";
      WriteTrsf(L.Transformation(),OS,Standard_True);
    }
    else {
      OS << "2 ";
      OS << " "<<myMap.FindIndex(L1) << " "<<p;
      while (!L2.IsIdentity()) {
	L1 = L2.FirstDatum();
	p  = L2.FirstPower();
	L2 = L2.NextLocation();
	OS << " "<<myMap.FindIndex(L1) << " "<<p;
      }
      OS << " 0\n";
    }
  }
  OS.precision(prec);
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

static void ReadTrsf(gp_Trsf& T,
		     Standard_IStream& IS)
{
  Standard_Real V1[3],V2[3],V3[3];
  Standard_Real V[3];
  
  GeomTools::GetReal(IS, V1[0]);
  GeomTools::GetReal(IS, V1[1]);
  GeomTools::GetReal(IS, V1[2]);
  GeomTools::GetReal(IS, V[0]);

  GeomTools::GetReal(IS, V2[0]);
  GeomTools::GetReal(IS, V2[1]);
  GeomTools::GetReal(IS, V2[2]);
  GeomTools::GetReal(IS, V[1]);

  GeomTools::GetReal(IS, V3[0]);
  GeomTools::GetReal(IS, V3[1]);
  GeomTools::GetReal(IS, V3[2]);
  GeomTools::GetReal(IS, V[2]);
  
  T.SetValues(V1[0],V1[1],V1[2],V[0],
	      V2[0],V2[1],V2[2],V[1],
	      V3[0],V3[1],V3[2],V[2]);
  return;
}
//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

void  TopTools_LocationSet::Read(Standard_IStream& IS)
{
  myMap.Clear();

  char buffer[255];
  Standard_Integer l1,p;

  IS >> buffer;
  if (strcmp(buffer,"Locations")) {
    cout << "Not a location table "<<endl;
    return;
  }

  Standard_Integer i, nbLoc;
  IS >> nbLoc;
  
  TopLoc_Location L;
  gp_Trsf T;
    
  //OCC19559
  Message_ProgressSentry PS(GetProgress(), "Locations", 0, nbLoc, 1);
  for (i = 1; i <= nbLoc&& PS.More(); i++, PS.Next()) {
    Standard_Integer typLoc;
    IS >> typLoc;
    
    if (typLoc == 1) {
      ReadTrsf(T,IS);
      L = T;
    }

    else if (typLoc == 2) {
      L = TopLoc_Location();
      IS >> l1;
      while (l1 != 0) { 
	IS >> p;
	TopLoc_Location L1 = myMap(l1);
	L = L1.Powered(p) *L;
	IS >> l1;
      }
    }
    
    if (!L.IsIdentity()) myMap.Add(L);
  }
}

//=======================================================================
//function : GetProgress
//purpose  : 
//=======================================================================

Handle(Message_ProgressIndicator) TopTools_LocationSet::GetProgress() const
{
  return myProgress;
}

//=======================================================================
//function : SetProgress
//purpose  : 
//=======================================================================

void TopTools_LocationSet::SetProgress(const Handle(Message_ProgressIndicator)& PR)
{
  myProgress = PR;
}


