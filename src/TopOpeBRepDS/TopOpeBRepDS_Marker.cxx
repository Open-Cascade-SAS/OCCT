// File:	TopOpeBRepDS_Marker.cxx
// Created:	Wed Oct 22 18:57:02 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#include <TopOpeBRepDS_Marker.ixx>
#include <TopOpeBRepDS_define.hxx>
//#include <TopExp.hxx>
//#include <TopTools_IndexedMapOfShape.hxx>

TopOpeBRepDS_Marker::TopOpeBRepDS_Marker()
{
  Reset();
}

void TopOpeBRepDS_Marker::Reset()
{
  myhe.Nullify();
  myne = 0;
}

void TopOpeBRepDS_Marker::Set(const Standard_Integer ie, const Standard_Boolean b)
{
  Allocate(ie);
  if (!(ie>=1 && ie<=myne)) return;
  myhe->SetValue(ie,b); 
}

void TopOpeBRepDS_Marker::Set(const Standard_Boolean b, const Standard_Integer na, const Standard_Address aa)
{
  char ** a = (char**)aa;
//  Standard_Integer ia,ie;
  Standard_Integer ia;
  if (!na) myhe->Init(b);
  else for (ia=0; ia<na; ia++) Set(atoi(a[ia]),b);
}

Standard_Boolean TopOpeBRepDS_Marker::GetI(const Standard_Integer ie) const
{
  if (myhe.IsNull()) return Standard_False;
  if (!(ie>=1 && ie<=myne)) return Standard_False;
  return myhe->Value(ie);
}

void TopOpeBRepDS_Marker::Allocate(const Standard_Integer n)
{
  Standard_Boolean all = (n > myne);
  Standard_Integer nall = n;
  if (all) {
    if (myne == 0) nall = 1000;
    myhe = new TColStd_HArray1OfBoolean(0,nall);
    myhe->Init(Standard_False);
  }
  if (nall) myne = nall;
}
