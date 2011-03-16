// File:	IntPolyh_ArrayOfStartPoints.cxx
// Created:	Tue Apr  6 11:05:39 1999
// Author:	Fabrice SERVANT
//		<fst@cleox.paris1.matra-dtv.fr>


#include <IntPolyh_StartPoint.ixx>
#include <IntPolyh_ArrayOfStartPoints.ixx>
#include <stdio.h>



IntPolyh_ArrayOfStartPoints::IntPolyh_ArrayOfStartPoints() : n(0),ptr(0) { }

IntPolyh_ArrayOfStartPoints::IntPolyh_ArrayOfStartPoints(const Standard_Integer N){ 
  Init(N);
}

void IntPolyh_ArrayOfStartPoints::Init(const Standard_Integer N) { 
  Destroy();
  ptr = (void*) (new IntPolyh_StartPoint [N]);
  n=N;
}

Standard_Integer IntPolyh_ArrayOfStartPoints::NbPoints() const { 
    return(n); 
}


#define BORNES1
const IntPolyh_StartPoint& IntPolyh_ArrayOfStartPoints::Value(const Standard_Integer Index) const { 
  IntPolyh_StartPoint *ptrstpoint = (IntPolyh_StartPoint *)ptr;
#if BORNES
  if(Index<0 || Index>=n) { cerr<<" Erreur1 "<<endl; cout<<"1dd";} 
#endif
  return(ptrstpoint[Index]); 
}

IntPolyh_StartPoint& IntPolyh_ArrayOfStartPoints::ChangeValue(const Standard_Integer Index) { 
  IntPolyh_StartPoint *ptrstpoint = (IntPolyh_StartPoint *)ptr;
#if BORNES
    if(Index<0 || Index>=n) { cerr<<" Erreur1 "<<endl; cout<<"1dd"; } 
#endif
    return(ptrstpoint[Index]); 
}
  
void IntPolyh_ArrayOfStartPoints::Destroy() { 
  if(n) { 
    if(ptr) { 
      IntPolyh_StartPoint *ptrstpoint = (IntPolyh_StartPoint *)ptr;
      delete [] ptrstpoint;
      ptrstpoint=0;
      ptr=0;
      n=0;
      }
  }
}
  
IntPolyh_ArrayOfStartPoints & IntPolyh_ArrayOfStartPoints::Copy(const IntPolyh_ArrayOfStartPoints& Other) { 
  if(ptr==Other.ptr) return(*this);
  Destroy();
  n=Other.NbPoints();
  ptr = (void *) (new IntPolyh_StartPoint[n]);
  for(Standard_Integer i=0;i<=n;i++) { 
    (*this)[i]=Other[i];
  }
  return(*this);
}

void IntPolyh_ArrayOfStartPoints::Dump() const{ 
  printf("\n ArrayOfStartPoints 0-> %d",n-1);
  for(Standard_Integer i=0;i<n;i++) { 
    (*this)[i].Dump(i);
  }
  printf("\n");
}

