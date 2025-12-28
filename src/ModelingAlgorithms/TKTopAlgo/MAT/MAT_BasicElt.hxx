// Created on: 1993-04-30
// Created by: Yves FRICAUD
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

#ifndef _MAT_BasicElt_HeaderFile
#define _MAT_BasicElt_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
class MAT_Arc;

//! A BasicELt is associated to each elementary
//! constituent of the figure.
class MAT_BasicElt : public Standard_Transient
{

public:
  //! Constructor, <anInteger> is the <index> of <me>.
  Standard_EXPORT MAT_BasicElt(const int anInteger);

  //! Return <startArcLeft> or <startArcRight> corresponding
  //! to <aSide>.
  Standard_EXPORT occ::handle<MAT_Arc> StartArc() const;

  //! Return <endArcLeft> or <endArcRight> corresponding
  //! to <aSide>.
  Standard_EXPORT occ::handle<MAT_Arc> EndArc() const;

  //! Return the <index> of <me> in Graph.TheBasicElts.
  Standard_EXPORT int Index() const;

  //! Return the <GeomIndex> of <me>.
  Standard_EXPORT int GeomIndex() const;

  Standard_EXPORT void SetStartArc(const occ::handle<MAT_Arc>& anArc);

  Standard_EXPORT void SetEndArc(const occ::handle<MAT_Arc>& anArc);

  Standard_EXPORT void SetIndex(const int anInteger);

  Standard_EXPORT void SetGeomIndex(const int anInteger);

  DEFINE_STANDARD_RTTIEXT(MAT_BasicElt, Standard_Transient)

private:
  void* startLeftArc;
  void* endLeftArc;
  int   index;
  int   geomIndex;
};

#endif // _MAT_BasicElt_HeaderFile
