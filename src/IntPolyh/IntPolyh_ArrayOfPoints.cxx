// File:	IntPolyh_ArrayOfPoints.cxx
// Created:	Mon Mar  8 09:32:00 1999
// Author:	Fabrice SERVANT
//		<fst@cleox.paris1.matra-dtv.fr>


#include <IntPolyh_ArrayOfPoints.ixx>

#include <stdio.h>


IntPolyh_ArrayOfPoints::IntPolyh_ArrayOfPoints() : n(0),fintp(0),ptr(0) { }

IntPolyh_ArrayOfPoints::IntPolyh_ArrayOfPoints(const Standard_Integer N): fintp(0){ 
  Init(N);
}

void IntPolyh_ArrayOfPoints::Init(const Standard_Integer N) { 
  Destroy();
  ptr = (void*) (new IntPolyh_Point [N]);
  n = N;
}

Standard_Integer IntPolyh_ArrayOfPoints::GetN() const { 
    return(n); 
}

Standard_Integer IntPolyh_ArrayOfPoints::NbPoints() const { 
    return(fintp); 
}

void IntPolyh_ArrayOfPoints::IncNbPoints() {
  fintp++;
}

void IntPolyh_ArrayOfPoints::SetNbPoints(const Standard_Integer endaop) {
  fintp = endaop;
}

# ifdef DEB
  #define BORNES 1
# endif

const IntPolyh_Point& IntPolyh_ArrayOfPoints::Value(const Standard_Integer Index) const { 
  IntPolyh_Point *ptrpoint = (IntPolyh_Point *)ptr;
#if BORNES
  if(Index<0 || Index>=n) { 
    cerr<<" Erreur1 "<<endl; 
    printf("Value() from IntPolyh_ArrayOfPoints : ERROR value outside of the array\n");
  } 
#endif
  return(ptrpoint[Index]); 
}

IntPolyh_Point& IntPolyh_ArrayOfPoints::ChangeValue(const Standard_Integer Index) { 
  IntPolyh_Point *ptrpoint = (IntPolyh_Point *)ptr;
#if BORNES
    if(Index<0 || Index>=n) { 
      cerr<<" Erreur1 "<<endl; 
      printf("ChangeValue() from IntPolyh_ArrayOfPoints : ERROR value outside of the array\n");
    } 
#endif
    return(ptrpoint[Index]); 
}
  
void IntPolyh_ArrayOfPoints::Destroy() { 
  if(n) { 
    if(ptr) { 
      IntPolyh_Point *ptrpoint = (IntPolyh_Point *)ptr;
      delete [] ptrpoint;
      ptrpoint=0;
      ptr=0;
      n=0;
      }
  }
}
  
IntPolyh_ArrayOfPoints & IntPolyh_ArrayOfPoints::Copy(const IntPolyh_ArrayOfPoints& Other) { 
  if(ptr==Other.ptr) return(*this);
  Destroy();
  n=Other.n;
  ptr = (void *) (new IntPolyh_Point[n]);
  for(Standard_Integer i=0;i<=n;i++) { 
    (*this)[i]=Other[i];
  }
  return(*this);
}

void IntPolyh_ArrayOfPoints::Dump() const{ 
  printf("\n ArrayOfPoints 0-> %d\n",fintp-1);
  printf("size %d, room left%d", n, n-fintp);
  for(Standard_Integer i=0;i<fintp;i++) { 
    (*this)[i].Dump(i);
  }
  printf("\n");
}







