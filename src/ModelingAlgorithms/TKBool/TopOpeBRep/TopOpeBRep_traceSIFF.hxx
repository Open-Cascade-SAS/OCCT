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

  #include <TopAbs_ShapeEnum.hxx>

  #include <TopAbs_Orientation.hxx>

  #include <TopAbs_State.hxx>

  #include <TopoDS_Shape.hxx>

  #include <TopTools_ShapeMapHasher.hxx>

  #include <NCollection_Map.hxx>

  #include <NCollection_List.hxx>

  #include <NCollection_IndexedMap.hxx>

  #include <NCollection_DataMap.hxx>

  #include <Standard_Integer.hxx>

  #include <NCollection_IndexedDataMap.hxx>

  #include <TopoDS_Face.hxx>

  #include <TopoDS_Edge.hxx>

  #include <TopoDS_Vertex.hxx>

  #include <TCollection_AsciiString.hxx>
  #include <Standard_OStream.hxx>

class TopOpeBRep_traceSIFF
{
public:
  TopOpeBRep_traceSIFF();

  void Reset();

  void Set(const bool b, int n, char** a);

  void Set(const TCollection_AsciiString& brep1,
           const TCollection_AsciiString& brep2,
           const TCollection_AsciiString& n);

  TCollection_AsciiString Name1(const int I) const;
  TCollection_AsciiString Name2(const int I) const;

  const TCollection_AsciiString& File() const;

  bool Start(const TCollection_AsciiString& s, Standard_OStream& OS);
  void Add(const int I1, const int I2);
  void End(const TCollection_AsciiString& s, Standard_OStream& OS);

private:
  TCollection_AsciiString mybrep1, mybrep2, myfilename;
  std::filebuf            myfilebuf;
  bool                    myopen;
};

// #ifdef OCCT_DEBUG
#endif
// #define _TopOpeBRep_traceSIFF_HeaderFile
#endif
