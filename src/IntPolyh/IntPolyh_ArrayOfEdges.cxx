// Created on: 1999-03-08
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



#include <IntPolyh_ArrayOfEdges.ixx>
#include <stdio.h>
#include <Standard_Stream.hxx>

IntPolyh_ArrayOfEdges::IntPolyh_ArrayOfEdges() : n(0),finte(0),ptr(0) { }

IntPolyh_ArrayOfEdges::IntPolyh_ArrayOfEdges(const Standard_Integer N) :  n(N),finte(0) { 
  Init(N);
}

void IntPolyh_ArrayOfEdges::Init(const Standard_Integer N) { 
  Destroy();
  n=N;
  ptr = (void*) (new IntPolyh_Edge [n]);
}

const Standard_Integer IntPolyh_ArrayOfEdges::GetN() const { 
  return(n); 
}
const Standard_Integer IntPolyh_ArrayOfEdges::NbEdges() const { 
  return(finte); 
}

void IntPolyh_ArrayOfEdges::SetNbEdges(const Standard_Integer endaoe) {
  finte = endaoe;
}

void IntPolyh_ArrayOfEdges::IncNbEdges(){
  finte++;
}

# ifdef DEB
  #define BORNES 1
# endif

const IntPolyh_Edge& IntPolyh_ArrayOfEdges::Value(const Standard_Integer Index) const { 
  IntPolyh_Edge* ptredge = (IntPolyh_Edge*) ptr; 
#if BORNES
  if(Index<0 || Index>=n) { 
    cerr<<" Erreur2 value"<<endl;  
    printf("Value() from IntPolyh_ArrayOfEdges : ERROR value outside of the array\n");
  } 
#endif
  return(ptredge[Index]); 
}

IntPolyh_Edge& IntPolyh_ArrayOfEdges::ChangeValue(const Standard_Integer Index) { 
  IntPolyh_Edge* ptredge = (IntPolyh_Edge*) ptr; 
#if BORNES
  if(Index<0 || Index>=n) { 
    cerr<<" Erreur2 chgVal"<<endl; 
    printf("ChangeValue() from IntPolyh_ArrayOfEdges : ERROR value outside of the array\n");
  }
#endif
  return(ptredge[Index]); 
}

void IntPolyh_ArrayOfEdges::Destroy() { 
  if(n) { 
    if(ptr) { 
      IntPolyh_Edge* ptredge = (IntPolyh_Edge*) ptr; 
      delete [] ptredge;
      ptredge=0;
      ptr=0;
      n=0;
    }
  }
}
  
IntPolyh_ArrayOfEdges & IntPolyh_ArrayOfEdges::Copy(const IntPolyh_ArrayOfEdges& Other) { 
  if(ptr==Other.ptr) return(*this);
    Destroy();
  n=Other.n;
  ptr = (void*) (new IntPolyh_Edge[n]);
  for(Standard_Integer i=0;i<n;i++) { 
    (*this)[i]=Other[i];
  }
  return(*this);
}

void IntPolyh_ArrayOfEdges::Dump() const{ 
  printf("\n ArrayOfEdges 0-> %d",n-1);
    for(Standard_Integer i=0;i<n;i++) { 
      (*this)[i].Dump(i);
    }
    printf("\n");
}




