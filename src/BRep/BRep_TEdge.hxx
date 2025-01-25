// Created on: 1992-05-27
// Created by: Remi LEQUETTE
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

#ifndef _BRep_TEdge_HeaderFile
#define _BRep_TEdge_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <TopoDS_TEdge.hxx>
class TopoDS_TShape;

class BRep_TEdge;
DEFINE_STANDARD_HANDLE(BRep_TEdge, TopoDS_TEdge)

//! The TEdge from BRep is  inherited from  the  TEdge
//! from TopoDS. It contains the geometric data.
//!
//! The TEdge contains :
//!
//! * A tolerance.
//!
//! * A same parameter flag.
//!
//! * A same range flag.
//!
//! * A Degenerated flag.
//!
//! *  A  list   of curve representation.
class BRep_TEdge : public TopoDS_TEdge
{

public:
  //! Creates an empty TEdge.
  Standard_EXPORT BRep_TEdge();

  Standard_Real Tolerance() const;

  void Tolerance(const Standard_Real T);

  //! Sets the tolerance  to the   max  of <T>  and  the
  //! current  tolerance.
  void UpdateTolerance(const Standard_Real T);

  Standard_EXPORT Standard_Boolean SameParameter() const;

  Standard_EXPORT void SameParameter(const Standard_Boolean S);

  Standard_EXPORT Standard_Boolean SameRange() const;

  Standard_EXPORT void SameRange(const Standard_Boolean S);

  Standard_EXPORT Standard_Boolean Degenerated() const;

  Standard_EXPORT void Degenerated(const Standard_Boolean S);

  const BRep_ListOfCurveRepresentation& Curves() const;

  BRep_ListOfCurveRepresentation& ChangeCurves();

  //! Returns a copy  of the  TShape  with no sub-shapes.
  Standard_EXPORT Handle(TopoDS_TShape) EmptyCopy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRep_TEdge, TopoDS_TEdge)

protected:
private:
  Standard_Real                  myTolerance;
  Standard_Integer               myFlags;
  BRep_ListOfCurveRepresentation myCurves;
};

#include <BRep_TEdge.lxx>

#endif // _BRep_TEdge_HeaderFile
