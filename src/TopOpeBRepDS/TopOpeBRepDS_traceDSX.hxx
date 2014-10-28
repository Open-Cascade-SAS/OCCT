// Created on: 1997-10-22
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _TopOpeBRepDS_traceDSX_HeaderFile
#define _TopOpeBRepDS_traceDSX_HeaderFile

#ifdef OCCT_DEBUG

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
  Handle(TopOpeBRepDS_HDataStructure) myHDS;
  Standard_Integer myne; Handle(TColStd_HArray1OfBoolean) myhe;
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

// #ifdef OCCT_DEBUG
#endif

// #define _TopOpeBRepDS_traceDSX_HeaderFile
#endif
