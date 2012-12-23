// Created on: 1997-10-22
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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

TopOpeBRep_traceSIFF SIFF;

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
