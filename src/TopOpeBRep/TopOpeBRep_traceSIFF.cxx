// File:	TopOpeBRep_traceSIFF.cxx
// Created:	Wed Oct 22 18:57:02 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#ifdef DEB

#include <TopOpeBRep_traceSIFF.hxx>
#include <stdio.h>
TopOpeBRep_traceSIFF::TopOpeBRep_traceSIFF()
{ 
  Reset();
}

void TopOpeBRep_traceSIFF::Reset()
{ 
  mybrep1 = ""; 
  mybrep2 = ""; 
  myfilename = ""; 
  myopen = Standard_False;
}

void TopOpeBRep_traceSIFF::Set(const Standard_Boolean b,
			       Standard_Integer n,
			       char**a)
{
  if (n < 3 || !b || a == NULL) { 
    Reset();
    return;
  }
  mybrep1 = a[0];
  mybrep2 = a[1];
  myfilename = a[2];
}

void TopOpeBRep_traceSIFF::Set(const TCollection_AsciiString& brep1,
			       const TCollection_AsciiString& brep2,
			       const TCollection_AsciiString& filename)
{ 
  mybrep1 = brep1; 
  mybrep2 = brep2; 
  myfilename = filename;
}

TCollection_AsciiString TopOpeBRep_traceSIFF::Name1(const Standard_Integer I) const
{
  TCollection_AsciiString s = mybrep1 + "_" + I; 
  return s;
} 

TCollection_AsciiString TopOpeBRep_traceSIFF::Name2(const Standard_Integer I) const
{
  TCollection_AsciiString s = mybrep2 + "_" + I; 
  return s;
} 

const TCollection_AsciiString& TopOpeBRep_traceSIFF::File() const 
{ 
  return myfilename; 
}

Standard_Boolean TopOpeBRep_traceSIFF::Start(const TCollection_AsciiString& s,
					     Standard_OStream& OS) 
{
  Standard_CString cs = myfilename.ToCString();
  myopen = Standard_True;
  if (!myfilebuf.open(cs,ios::out)) {
    myopen = Standard_False;
  }
  if (!myopen) {
    return myopen;
  }
  ostream osfic(&myfilebuf); osfic.precision(15);
  if (s.Length()) {
    OS<<s<<myfilename<<endl;
  }
  return myopen;
}

void TopOpeBRep_traceSIFF::Add(const Standard_Integer I1,
			       const Standard_Integer I2)
{
  if (!myopen) {
    return;
  }
  TCollection_AsciiString n1 = Name1(I1);
  TCollection_AsciiString n2 = Name2(I2);
  ostream osfic(&myfilebuf);
  osfic<<n1<<" "<<n2<<"\n";
}

void TopOpeBRep_traceSIFF::End(const TCollection_AsciiString& s,
			       Standard_OStream& OS)
{
  if (!myopen) {
    return;
  }
  if (s.Length()) {
    OS<<s<<myfilename<<endl;
  }
  myopen = Standard_False;
}  
//////////////////////////////////////////////////////////////////////////

Standard_EXPORT TopOpeBRep_traceSIFF SIFF;

Standard_EXPORT void TopOpeBRep_SettraceSIFF(const Standard_Boolean b,
					     Standard_Integer n,char**a)
{
  SIFF.Set(b,n,a);
}

Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceSIFF()
{
  Standard_Boolean b = (SIFF.File().Length() != 0);
  return b;
}

// #ifdef DEB
#endif
