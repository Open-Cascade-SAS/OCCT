// File:	IntPolyh_ArrayOfTangentZones.cxx
// Created:	Tue Apr  6 11:05:39 1999
// Author:	Fabrice SERVANT
//		<fst@cleox.paris1.matra-dtv.fr>


#include <IntPolyh_StartPoint.ixx>
#include <IntPolyh_ArrayOfTangentZones.ixx>
#include <stdio.h>

IntPolyh_ArrayOfTangentZones::IntPolyh_ArrayOfTangentZones() : n(0),nbtangentzones(0),ptr(0) { }

IntPolyh_ArrayOfTangentZones::IntPolyh_ArrayOfTangentZones(const Standard_Integer N) : nbtangentzones(0){ 
  Init(N);
}

void IntPolyh_ArrayOfTangentZones::Init(const Standard_Integer N) { 
  Destroy();
  ptr = (void*) (new IntPolyh_StartPoint [N]);
  n=N;
}

Standard_Integer IntPolyh_ArrayOfTangentZones::GetN() const { 
  return(n); 
}

Standard_Integer IntPolyh_ArrayOfTangentZones::NbTangentZones() const { 
  return(nbtangentzones); 
}

void IntPolyh_ArrayOfTangentZones::IncrementNbTangentZones() { 
  nbtangentzones++; 
}

# ifdef DEB
  # define BORNES1
# endif
const IntPolyh_StartPoint& IntPolyh_ArrayOfTangentZones::Value(const Standard_Integer Index) const { 
  IntPolyh_StartPoint *ptrstpoint = (IntPolyh_StartPoint *)ptr;
#if BORNES
  if(Index<0 || Index>=n) { 
    cerr<<" Erreur1 "<<endl;
    printf("Value() from IntPolyh_ArrayOfTangentZones :ERROR value out of array\n");
  } 
#endif
  return(ptrstpoint[Index]); 
}

IntPolyh_StartPoint& IntPolyh_ArrayOfTangentZones::ChangeValue(const Standard_Integer Index) { 
  IntPolyh_StartPoint *ptrstpoint = (IntPolyh_StartPoint *)ptr;
#if BORNES
    if(Index<0 || Index>=n) { 
      cerr<<" Erreur1 "<<endl; 
      printf("Value() from IntPolyh_ArrayOfTangentZones :ERROR value out of array\n");
    } 
#endif
    return(ptrstpoint[Index]); 
}
  
void IntPolyh_ArrayOfTangentZones::Destroy() { 
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
  
IntPolyh_ArrayOfTangentZones & IntPolyh_ArrayOfTangentZones::Copy(const IntPolyh_ArrayOfTangentZones& Other) { 
  if(ptr==Other.ptr) return(*this);
  Destroy();
  n=Other.n;
  ptr = (void *) (new IntPolyh_StartPoint[n]);
  for(Standard_Integer i=0;i<=n;i++) { 
    (*this)[i]=Other[i];
  }
  return(*this);
}

void IntPolyh_ArrayOfTangentZones::Dump() const{ 
  printf("\n ArrayOfTangentZones 0-> %d",nbtangentzones-1);
  for(Standard_Integer i=0;i<nbtangentzones;i++) { 
    (*this)[i].Dump(i);
  }
  printf("\n");
}



