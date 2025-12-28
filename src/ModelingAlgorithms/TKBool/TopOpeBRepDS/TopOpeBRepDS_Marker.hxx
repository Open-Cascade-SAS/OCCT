// Created on: 1999-04-01
// Created by: Jean Yves LEBEY
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_Marker_HeaderFile
#define _TopOpeBRepDS_Marker_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>

class TopOpeBRepDS_Marker : public Standard_Transient
{

public:
  Standard_EXPORT TopOpeBRepDS_Marker();

  Standard_EXPORT void Reset();

  Standard_EXPORT void Set(const int i, const bool b);

  Standard_EXPORT void Set(const bool b, const int n, void* const a);

  Standard_EXPORT bool GetI(const int i) const;

  Standard_EXPORT void Allocate(const int n);

  DEFINE_STANDARD_RTTIEXT(TopOpeBRepDS_Marker, Standard_Transient)

private:
  occ::handle<NCollection_HArray1<bool>> myhe;
  int                                    myne;
};

#endif // _TopOpeBRepDS_Marker_HeaderFile
