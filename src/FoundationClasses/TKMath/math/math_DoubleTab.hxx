// Created on: 1992-02-07
// Created by: Laurent PAINNOT
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _math_DoubleTab_HeaderFile
#define _math_DoubleTab_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>

class math_DoubleTab
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT math_DoubleTab(const Standard_Integer LowerRow,
                                 const Standard_Integer UpperRow,
                                 const Standard_Integer LowerCol,
                                 const Standard_Integer UpperCol);

  Standard_EXPORT math_DoubleTab(const Standard_Address Tab,
                                 const Standard_Integer LowerRow,
                                 const Standard_Integer UpperRow,
                                 const Standard_Integer LowerCol,
                                 const Standard_Integer UpperCol);

  Standard_EXPORT void Init(const Standard_Real InitValue);

  Standard_EXPORT math_DoubleTab(const math_DoubleTab& Other);

  void Copy(math_DoubleTab& Other) const;

  Standard_EXPORT void SetLowerRow(const Standard_Integer LowerRow);

  Standard_EXPORT void SetLowerCol(const Standard_Integer LowerCol);

  Standard_Real& Value(const Standard_Integer RowIndex, const Standard_Integer ColIndex) const;

  Standard_Real& operator()(const Standard_Integer RowIndex, const Standard_Integer ColIndex) const
  {
    return Value(RowIndex, ColIndex);
  }

  Standard_EXPORT void Free();

  ~math_DoubleTab() { Free(); }

protected:
private:
  Standard_EXPORT void Allocate();

  Standard_Address Addr;
  Standard_Real    Buf[16]{};
  Standard_Boolean isAllocated;
  Standard_Integer LowR;
  Standard_Integer UppR;
  Standard_Integer LowC;
  Standard_Integer UppC;
};

#include <math_DoubleTab.lxx>

#endif // _math_DoubleTab_HeaderFile
