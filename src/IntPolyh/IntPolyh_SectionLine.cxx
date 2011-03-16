// File:	IntPolyh_SectionLine.cxx
// Created:	Tue Apr  6 11:05:39 1999
// Author:	Fabrice SERVANT
//		<fst@cleox.paris1.matra-dtv.fr>

// modified by Edward AGAPOV (eap) Thu Feb 14 2002 (occ139)
// Add Prepend(), replace array with sequence

#include <IntPolyh_StartPoint.ixx>
#include <IntPolyh_SectionLine.ixx>
#include <stdio.h>

//=======================================================================
//function : IntPolyh_SectionLine
//purpose  : 
//=======================================================================

IntPolyh_SectionLine::IntPolyh_SectionLine() /*: n(0),nbstartpoints(0),ptr(0)*/ { }

//=======================================================================
//function : IntPolyh_SectionLine
//purpose  : 
//=======================================================================

IntPolyh_SectionLine::IntPolyh_SectionLine(const Standard_Integer N)/* : nbstartpoints(0)*/{ 
  Init(N);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void IntPolyh_SectionLine::Init(const Standard_Integer /*N*/) { 
//   ptr = (void*) (new IntPolyh_StartPoint [N]);
//   n=N;
  if (!mySeqOfSPoints.Length()) IncrementNbStartPoints();
}

//=======================================================================
//function : GetN
//purpose  : 
//=======================================================================

Standard_Integer IntPolyh_SectionLine::GetN() const { 
  //return(n);
  return mySeqOfSPoints.Length();
}

//=======================================================================
//function : NbStartPoints
//purpose  : 
//=======================================================================

Standard_Integer IntPolyh_SectionLine::NbStartPoints() const { 
//  return(nbstartpoints); 
  return mySeqOfSPoints.Length() - 1;
}

//=======================================================================
//function : IncrementNbStartPoints
//purpose  : 
//=======================================================================

void IntPolyh_SectionLine::IncrementNbStartPoints() { 
//  nbstartpoints++;
  IntPolyh_StartPoint aSP;
  mySeqOfSPoints.Append(aSP);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

// # ifdef DEB
//   #define BORNES1
// # endif
const IntPolyh_StartPoint& IntPolyh_SectionLine::Value(const Standard_Integer Index) const { 
//   IntPolyh_StartPoint *ptrstpoint = (IntPolyh_StartPoint *)ptr;
// #if BORNES
//   if(Index<0 || Index>=n) { 
//     cerr<<" Erreur1 "<<endl; 
//     printf("Value() from IntPolyh_SectionLine.cxx : ERROR : value out of array\n");
//   } 
// #endif
//   return(ptrstpoint[Index]);
  return mySeqOfSPoints(Index+1);
}

//=======================================================================
//function : ChangeValue
//purpose  : 
//=======================================================================

IntPolyh_StartPoint& IntPolyh_SectionLine::ChangeValue(const Standard_Integer Index) { 
//   IntPolyh_StartPoint *ptrstpoint = (IntPolyh_StartPoint *)ptr;
// #if BORNES
//     if(Index<0 || Index>=n) { 
//       cerr<<" Erreur1 "<<endl; 
//       printf("ChangeValue() from IntPolyh_SectionLine.cxx : ERROR : value out of array\n");
//     } 
// #endif
//     return(ptrstpoint[Index]); 
  return mySeqOfSPoints(Index+1);
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void IntPolyh_SectionLine::Destroy() { 
//   if(n) { 
//     if(ptr) { 
//       IntPolyh_StartPoint *ptrstpoint = (IntPolyh_StartPoint *)ptr;
//       delete [] ptrstpoint;
//       ptr=0;
//       n=0;
//     }
//   }
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

IntPolyh_SectionLine & IntPolyh_SectionLine::Copy(const IntPolyh_SectionLine& Other) { 
//   if(ptr==Other.ptr) return(*this);
//   Destroy();
//   n=Other.n;
//   ptr = (void *) (new IntPolyh_StartPoint[n]);
//   for(Standard_Integer i=0;i<=n;i++) { 
//     (*this)[i]=Other[i];
//   }
  mySeqOfSPoints = Other.mySeqOfSPoints;
  return(*this);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void IntPolyh_SectionLine::Dump() const{ 
  printf("\n SectionLine 0-> %d",/*nbstartpoints*/NbStartPoints()-1);
  for(Standard_Integer i=0;i<NbStartPoints();i++) { 
    //(*this)[i].Dump(i);
    Value(i).Dump(i);
//     const IntPolyh_StartPoint& SP = Value(i);
//     cout << "point P" << i << " " << SP.X() << " " << SP.Y() << " " << SP.Z() << endl;
  }
  printf("\n");
}

//=======================================================================
//function : Prepend
//purpose  : 
//=======================================================================

void IntPolyh_SectionLine::Prepend(const IntPolyh_StartPoint& SP)
{
  mySeqOfSPoints.Prepend(SP);
}



