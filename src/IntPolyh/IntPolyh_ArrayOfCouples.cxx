// Created on: 1999-04-08
// Created by: Fabrice SERVANT
// Copyright (c) 1999-1999 Matra Datavision
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



#include <IntPolyh_ArrayOfCouples.ixx>
#include <stdio.h>

#include <Standard_Stream.hxx>

IntPolyh_ArrayOfCouples::IntPolyh_ArrayOfCouples() : n(0),eoa(0),ptr(0){
}

IntPolyh_ArrayOfCouples::IntPolyh_ArrayOfCouples(const Standard_Integer N) :  n(N),eoa(0){ 
  Init(N);
}

void IntPolyh_ArrayOfCouples::Init(const Standard_Integer N) { 
  Destroy();
  ptr = (void *) (new IntPolyh_Couple [N]);
  n=N;
}

Standard_Integer IntPolyh_ArrayOfCouples::NbCouples() const { 
  return(eoa); 
}

void IntPolyh_ArrayOfCouples::SetNbCouples(const Standard_Integer fint) {
  eoa=fint;
}

void IntPolyh_ArrayOfCouples::IncNbCouples() {
  eoa++;
}

# ifdef DEB
  #define BORNES 1
# endif

const IntPolyh_Couple& IntPolyh_ArrayOfCouples::Value(const Standard_Integer Index) const { 
  IntPolyh_Couple* ptrCouple = (IntPolyh_Couple*) ptr; 
#if BORNES
  if(Index<0 || Index>=n) { 
    cerr<<" Erreur4 "<<endl; 
    printf("Value() from IntPolyh_ArrayOfCouples : value out of array\n");
  }
#endif
  return(ptrCouple[Index]); 
}

IntPolyh_Couple& IntPolyh_ArrayOfCouples::ChangeValue(const Standard_Integer Index) { 
  IntPolyh_Couple* ptrCouple = (IntPolyh_Couple*) ptr; 
#if BORNES
  if(Index<0 || Index>=n) { 
    cerr<<" Erreur4"<<endl;
    printf("ChangeValue() from IntPolyh_ArrayOfCouples : value out of array\n");
  }
#endif
  return(ptrCouple[Index]);
}

void IntPolyh_ArrayOfCouples::Destroy() { 
  if(n) { 
    if(ptr) { 
      IntPolyh_Couple* ptrCouple = (IntPolyh_Couple*) ptr; 
      delete [] ptrCouple;
      ptrCouple=0;
      ptr=0;
      n=0;
    }
  }
}

void IntPolyh_ArrayOfCouples::Dump() const{ 
  printf("\n ArrayOfCouples 0-> %d",n-1);
    for(Standard_Integer i=0;i<n;i++) { 
      (*this)[i].Dump(i);
    }
    printf("\n");
}







