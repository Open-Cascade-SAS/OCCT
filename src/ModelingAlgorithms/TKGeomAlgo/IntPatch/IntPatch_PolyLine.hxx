// Created on: 1993-01-29
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IntPatch_PolyLine_HeaderFile
#define _IntPatch_PolyLine_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt2d.hxx>
#include <IntPatch_IType.hxx>
#include <Standard_Boolean.hxx>
#include <IntPatch_Polygo.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class IntPatch_WLine;
class IntPatch_RLine;

class IntPatch_PolyLine : public IntPatch_Polygo
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntPatch_PolyLine();

  Standard_EXPORT IntPatch_PolyLine(const double InitDefle);

  Standard_EXPORT void SetWLine(const bool OnFirst, const occ::handle<IntPatch_WLine>& Line);

  Standard_EXPORT void SetRLine(const bool OnFirst, const occ::handle<IntPatch_RLine>& Line);

  Standard_EXPORT void ResetError();

  Standard_EXPORT int NbPoints() const;

  Standard_EXPORT gp_Pnt2d Point(const int Index) const;

private:
  Standard_EXPORT void Prepare();

  gp_Pnt2d               pnt;
  IntPatch_IType         typ;
  bool       onfirst;
  occ::handle<IntPatch_WLine> wpoly;
  occ::handle<IntPatch_RLine> rpoly;
};

#endif // _IntPatch_PolyLine_HeaderFile
