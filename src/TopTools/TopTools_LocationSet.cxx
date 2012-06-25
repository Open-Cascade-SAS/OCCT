// Created on: 1993-07-16
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


#include <Standard_Stream.hxx>
#include <TopTools_LocationSet.ixx>
#include <TopLoc_Location.hxx>
#include <Message_ProgressSentry.hxx>

#include <gp_Ax3.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

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
    if ( !GetProgress().IsNull() ) 
      GetProgress()->Show();

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
  
  IS >> V1[0] >> V1[1] >> V1[2] >> V[0];
  IS >> V2[0] >> V2[1] >> V2[2] >> V[1];
  IS >> V3[0] >> V3[1] >> V3[2] >> V[2];
  
  T.SetValues(V1[0],V1[1],V1[2],V[0],
	      V2[0],V2[1],V2[2],V[1],
	      V3[0],V3[1],V3[2],V[2],
	      Precision::Angular(),
	      Precision::Confusion());
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
    if ( !GetProgress().IsNull() ) 
      GetProgress()->Show();

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


