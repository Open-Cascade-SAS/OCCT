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


#ifndef _TopOpeBRepDS_traceDSX_HeaderFile
#define _TopOpeBRepDS_traceDSX_HeaderFile

#ifdef DEB

#include <TopOpeBRepDS_define.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <TCollection_AsciiString.hxx>
#include <MMgt_TShared.hxx>

class TopOpeBRepDS_traceDS : public MMgt_TShared {
public:
  TopOpeBRepDS_traceDS();
  virtual Standard_Integer Nelem() const;
  void SetHDS(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  const Handle(TopOpeBRepDS_HDataStructure)& GetHDS() const;
  void Set(const Standard_Integer i, const Standard_Boolean b);
  void Set(const Standard_Boolean b, Standard_Integer n, char** a);
  Standard_Boolean GetI(const Standard_Integer i) const;
  void Allocate();
protected: 
  Handle_TopOpeBRepDS_HDataStructure myHDS;
  Standard_Integer myne; Handle_TColStd_HArray1OfBoolean myhe;
};

class TopOpeBRepDS_traceCURVE : public TopOpeBRepDS_traceDS {  
public:
  TopOpeBRepDS_traceCURVE();
  Standard_Integer Nelem() const;
};

class TopOpeBRepDS_traceSHAPE : public TopOpeBRepDS_traceDS {
public: 
  TopOpeBRepDS_traceSHAPE();
  Standard_Integer Nelem() const;
  void SetSS(const TopoDS_Shape&,const TopoDS_Shape&);
  void SetSSHDS(const TopoDS_Shape&,const TopoDS_Shape&,const Handle(TopOpeBRepDS_HDataStructure)&);
  Standard_Integer Index(const TopoDS_Shape&) const;
  Standard_Boolean GetS(const TopoDS_Shape&) const;
private:
  Standard_Integer myns;
};

// #ifdef DEB
#endif

// #define _TopOpeBRepDS_traceDSX_HeaderFile
#endif
