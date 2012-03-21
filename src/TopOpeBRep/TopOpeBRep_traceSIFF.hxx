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


#ifndef _TopOpeBRep_traceSIFF_HeaderFile
#define _TopOpeBRep_traceSIFF_HeaderFile

#ifdef DEB

#include <TopOpeBRepTool_define.hxx>
#include <Standard_OStream.hxx>
#include <TopoDS_Shape.hxx>

class TopOpeBRep_traceSIFF {
public:

  TopOpeBRep_traceSIFF();

  void Reset();

  void Set(const Standard_Boolean b,
	   Standard_Integer n,
	   char**a);

  void Set(const TCollection_AsciiString& brep1,
	   const TCollection_AsciiString& brep2,
	   const TCollection_AsciiString& n);
  
  TCollection_AsciiString Name1(const Standard_Integer I) const;
  TCollection_AsciiString Name2(const Standard_Integer I) const;
  
  const TCollection_AsciiString& File() const;
  
  Standard_Boolean Start(const TCollection_AsciiString& s,
			 Standard_OStream& OS);
  void Add(const Standard_Integer I1,
	   const Standard_Integer I2);
  void End(const TCollection_AsciiString& s,
	   Standard_OStream& OS);
private: 
  TCollection_AsciiString mybrep1,mybrep2,myfilename;
  filebuf myfilebuf;
  Standard_Boolean myopen;
};

// #ifdef DEB
#endif
// #define _TopOpeBRep_traceSIFF_HeaderFile
#endif
