// File:	IntPolyh_ArrayOfTriangles.cxx
// Created:	Mon Mar  8 09:32:00 1999
// Author:	Fabrice SERVANT
//		<fst@cleox.paris1.matra-dtv.fr>


#include <IntPolyh_ArrayOfTriangles.ixx> 
#include <stdio.h>
#include <Standard_Stream.hxx>

IntPolyh_ArrayOfTriangles::IntPolyh_ArrayOfTriangles() : n(0),fintt(0),ptr(0) { }

IntPolyh_ArrayOfTriangles::IntPolyh_ArrayOfTriangles(const Standard_Integer N): fintt(0) { 
  Init(N);
}

void IntPolyh_ArrayOfTriangles::Init(const Standard_Integer N) { 
  Destroy();
  ptr = (void*) (new IntPolyh_Triangle [N]);
  n=N;
}

const Standard_Integer IntPolyh_ArrayOfTriangles::GetN() const { 
  return(n); 
}

const Standard_Integer IntPolyh_ArrayOfTriangles::NbTriangles() const { 
  return(fintt); 
}

void IntPolyh_ArrayOfTriangles::SetNbTriangles(const Standard_Integer endaot) { 
  fintt=endaot; 
}

void IntPolyh_ArrayOfTriangles::IncNbTriangles() { 
  fintt++; 
}

# ifdef DEB
  # define BORNES 1
# endif
const IntPolyh_Triangle& IntPolyh_ArrayOfTriangles::Value(const Standard_Integer Index) const { 
IntPolyh_Triangle* ptrtriangle = (IntPolyh_Triangle*)ptr;
#if BORNES
  if(Index<0 || Index>=n) { 
    cerr<<" Erreur31 "<<endl; 
    printf("Value() from IntPolyh_ArrayOfTriangles.cxx : ERROR value out of array\n");
  }
#endif
  return(ptrtriangle[Index]); 
}

IntPolyh_Triangle& IntPolyh_ArrayOfTriangles::ChangeValue(const Standard_Integer Index) { 
  IntPolyh_Triangle* ptrtriangle = (IntPolyh_Triangle*)ptr;
#if BORNES
  if(Index<0 || Index>=n) { 
    cerr<<" Erreur32 "<<endl; 
    printf("ChangeValue() from IntPolyh_ArrayOfTriangles.cxx : ERROR value out of array\n");
  }
#endif
  return(ptrtriangle[Index]); 
}

  
IntPolyh_ArrayOfTriangles & IntPolyh_ArrayOfTriangles::Copy(const IntPolyh_ArrayOfTriangles& Other) { 
  if(ptr==Other.ptr) return(*this);
  Destroy();
  n=Other.n;
  ptr = (void *)(new IntPolyh_Triangle[n]);
  for(Standard_Integer i=0;i<n;i++) { 
    (*this)[i]=Other[i];
  }
  return(*this);
}
    
void IntPolyh_ArrayOfTriangles::Destroy(){ 
  if(n) { 
    if(ptr) {
      IntPolyh_Triangle* ptrtriangle = (IntPolyh_Triangle*)ptr;
      delete [] ptrtriangle;
      ptrtriangle=0;
      ptr=0;
      n=0;
    }
  }
}


void IntPolyh_ArrayOfTriangles::Dump() const{ 
  printf("\n ArrayOfTriangles 0-> %d",n-1);
  for(Standard_Integer i=0;i<n;i++) { 
    ((*this)[i]).Dump(i);
  }
  printf("\n");
}

void IntPolyh_ArrayOfTriangles::DumpFleches() const{
  printf("\n ArrayOfTriangles 0-> %d",n-1);
  for(Standard_Integer i=0;i<n;i++) { 
    ((*this)[i]).DumpFleche(i);
  }
  printf("\n");
}

