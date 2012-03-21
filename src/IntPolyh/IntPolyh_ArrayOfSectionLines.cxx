// Created on: 1999-04-06
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



#include <IntPolyh_StartPoint.ixx>
#include <IntPolyh_ArrayOfSectionLines.ixx>
#include <stdio.h>


IntPolyh_ArrayOfSectionLines::IntPolyh_ArrayOfSectionLines() : n(0),nbsectionlines(0),ptr(0) { }

IntPolyh_ArrayOfSectionLines::IntPolyh_ArrayOfSectionLines(const Standard_Integer N) : nbsectionlines(0){ 
  Init(N);
}

void IntPolyh_ArrayOfSectionLines::Init(const Standard_Integer N) { 
  Destroy();
  ptr = (void*) (new IntPolyh_SectionLine [N]);
  n=N;
}

Standard_Integer IntPolyh_ArrayOfSectionLines::GetN() const { 
  return(n); 
}

Standard_Integer IntPolyh_ArrayOfSectionLines::NbSectionLines() const { 
  return(nbsectionlines); 
}

void IntPolyh_ArrayOfSectionLines::IncrementNbSectionLines() { 
  nbsectionlines++; 
}

#define BORNES1
const IntPolyh_SectionLine& IntPolyh_ArrayOfSectionLines::Value(const Standard_Integer Index) const { 
  IntPolyh_SectionLine *ptrstpoint = (IntPolyh_SectionLine *)ptr;
#if BORNES
  if(Index<0 || Index>=n) { cerr<<" Erreur1 "<<endl; cout<<"1dd";} 
#endif
  return(ptrstpoint[Index]); 
}

IntPolyh_SectionLine& IntPolyh_ArrayOfSectionLines::ChangeValue(const Standard_Integer Index) { 
  IntPolyh_SectionLine *ptrstpoint = (IntPolyh_SectionLine *)ptr;
#if BORNES
    if(Index<0 || Index>=n) { cerr<<" Erreur1 "<<endl; cout<<"1dd"; } 
#endif
    return(ptrstpoint[Index]); 
}
  
void IntPolyh_ArrayOfSectionLines::Destroy() { 
  if(n) { 
    if(ptr) { 
      IntPolyh_SectionLine *ptrsectionline = (IntPolyh_SectionLine *)ptr;
      for(Standard_Integer i=0; i<n; i++)
	ptrsectionline[i].Destroy();
      delete [] ptrsectionline;
      ptrsectionline=0;
      ptr=0;
      n=0;
    }
  }
}
  
IntPolyh_ArrayOfSectionLines & IntPolyh_ArrayOfSectionLines::Copy(const IntPolyh_ArrayOfSectionLines& Other) { 
  if(ptr==Other.ptr) return(*this);
  Destroy();
  n=Other.n;
  ptr = (void *) (new IntPolyh_SectionLine[n]);
  for(Standard_Integer i=0;i<=n;i++) { 
    (*this)[i]=Other[i];
  }
  return(*this);
}

void IntPolyh_ArrayOfSectionLines::Dump() const{ 
  printf("\n ArrayOfSectionLines 0-> %d",nbsectionlines-1);
  for(Standard_Integer i=0;i<nbsectionlines;i++) { 
    (*this)[i].Dump();
  }
  printf("\n");
}

