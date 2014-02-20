// Created on: 1996-02-05
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _TestTopOpe_VarsTopo_HeaderFile
#define _TestTopOpe_VarsTopo_HeaderFile

#include <TopOpeBRepTool_OutCurveType.hxx>
#include <TopOpeBRepTool_define.hxx>

class VarsTopo {

public:

  VarsTopo();
  void Init();
  void SetTolxDef();
  void SetTolx(const Standard_Real tol3x, const Standard_Real tol2x);
  void GetTolx(Standard_Real& tol3x, Standard_Real& tol2x) const;
  void SetToliDef();
  void SetToli(const Standard_Real tolarc, const Standard_Real toltan); 
  void GetToli(Standard_Real& tolarc, Standard_Real& toltan) const; Standard_Boolean GetForceToli() const;
  void SetClear(const Standard_Boolean b); Standard_Boolean GetClear() const;
  void SetOCT(char* namoct);
  void SetOCT(const TopOpeBRepTool_OutCurveType oct);
  TopOpeBRepTool_OutCurveType GetOCT() const;
  void SetC2D(char* namoct);
  void SetC2D(const Standard_Boolean c2d);
  Standard_Boolean GetC2D() const;
  void DumpOCT(Standard_OStream&) const;
  void DumpC2D(Standard_OStream&) const;
  void DumpTolx(Standard_OStream&) const;
  void DumpToli(Standard_OStream&) const;
  void DumpMode(Standard_OStream&) const;
  void DumpVerbose(Standard_OStream&) const;
  void DumpClear(Standard_OStream&) const;
  void Dump(Standard_OStream&) const;
  void SetMode(const Standard_Integer mode);
  Standard_Integer  GetMode() const;
  void SetVerbose(const Standard_Integer mode);
  Standard_Integer  GetVerbose() const;

private:

  TopOpeBRepTool_OutCurveType myOCT;
  Standard_Boolean myC2D;
  Standard_Real mytol3xdef,mytol2xdef;
  Standard_Real mytol3x,mytol2x;
  Standard_Boolean myforcetoli;
  Standard_Real mytolarc, mytoltan;
  Standard_Boolean myclear;
  Standard_Integer myexecmode;
  Standard_Integer myverbmode;

};

#endif
