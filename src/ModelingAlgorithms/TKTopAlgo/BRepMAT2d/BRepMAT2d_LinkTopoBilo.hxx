// Created on: 1994-10-07
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepMAT2d_LinkTopoBilo_HeaderFile
#define _BRepMAT2d_LinkTopoBilo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepMAT2d_DataMapOfShapeSequenceOfBasicElt.hxx>
#include <BRepMAT2d_DataMapOfBasicEltShape.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
class BRepMAT2d_Explorer;
class BRepMAT2d_BisectingLocus;
class MAT_BasicElt;
class TopoDS_Wire;

//! Constructs links between the Wire or the Face of the explorer and
//! the BasicElts contained in the bisecting locus.
class BRepMAT2d_LinkTopoBilo
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepMAT2d_LinkTopoBilo();

  //! Constructs the links Between S and BiLo.
  //!
  //! raises if <S> is not a face.
  Standard_EXPORT BRepMAT2d_LinkTopoBilo(const BRepMAT2d_Explorer&       Explo,
                                         const BRepMAT2d_BisectingLocus& BiLo);

  //! Constructs the links Between S and BiLo.
  //!
  //! raises if <S> is not a face or a wire.
  Standard_EXPORT void Perform(const BRepMAT2d_Explorer&       Explo,
                               const BRepMAT2d_BisectingLocus& BiLo);

  //! Initialise the Iterator on <S>
  //! <S> is an edge or a vertex of the initial
  //! wire or face.
  //! raises if <S> is not an edge or a vertex.
  Standard_EXPORT void Init(const TopoDS_Shape& S);

  //! Returns True if there  is a current  BasicElt.
  Standard_EXPORT Standard_Boolean More();

  //! Proceed to the next BasicElt.
  Standard_EXPORT void Next();

  //! Returns the current BasicElt.
  Standard_EXPORT Handle(MAT_BasicElt) Value() const;

  //! Returns the Shape linked to <aBE>.
  Standard_EXPORT TopoDS_Shape GeneratingShape(const Handle(MAT_BasicElt)& aBE) const;

protected:
private:
  Standard_EXPORT void LinkToWire(const TopoDS_Wire&              W,
                                  const BRepMAT2d_Explorer&       Explo,
                                  const Standard_Integer          IndexContour,
                                  const BRepMAT2d_BisectingLocus& BiLo);

  BRepMAT2d_DataMapOfShapeSequenceOfBasicElt myMap;
  BRepMAT2d_DataMapOfBasicEltShape           myBEShape;
  TopoDS_Shape                               myKey;
  Standard_Integer                           current;
  Standard_Boolean                           isEmpty;
};

#endif // _BRepMAT2d_LinkTopoBilo_HeaderFile
