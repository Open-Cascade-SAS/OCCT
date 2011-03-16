#include <IntSurf_PntOn2S.ixx>


IntSurf_PntOn2S::IntSurf_PntOn2S () : pt(0,0,0),u1(0),v1(0),u2(0),v2(0) {};

void IntSurf_PntOn2S::SetValue (const gp_Pnt& Pt,
				const Standard_Boolean OnFirst,
				const Standard_Real U,
				const Standard_Real V) {

  pt = Pt;
  if (OnFirst) {
    u1 = U;
    v1 = V;
  }
  else {
    u2 = U;
    v2 = V;
  }
}


void IntSurf_PntOn2S::SetValue (const Standard_Boolean OnFirst,
				const Standard_Real U,
				const Standard_Real V) {

  if (OnFirst) {
    u1 = U;
    v1 = V;
  }
  else {
    u2 = U;
    v2 = V;
  }
}



