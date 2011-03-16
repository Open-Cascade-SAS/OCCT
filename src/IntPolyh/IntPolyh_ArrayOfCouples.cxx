// File:	IntPolyh_ArrayOfCouples.cxx
// Created:	Thu Apr  8 1999
// Author:	Fabrice SERVANT
//		<fst@cleox.paris1.matra-dtv.fr>


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







