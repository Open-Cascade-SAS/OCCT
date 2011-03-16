// File:	IntPolyh_ArrayOfSectionLines.cxx
// Created:	Tue Apr  6 11:05:39 1999
// Author:	Fabrice SERVANT
//		<fst@cleox.paris1.matra-dtv.fr>


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

