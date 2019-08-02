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

#ifndef _TopOpeBRep_traceSIFF_HeaderFile
#define _TopOpeBRep_traceSIFF_HeaderFile

#ifdef OCCT_DEBUG

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
  std::filebuf myfilebuf;
  Standard_Boolean myopen;
};

// #ifdef OCCT_DEBUG
#endif
// #define _TopOpeBRep_traceSIFF_HeaderFile
#endif
