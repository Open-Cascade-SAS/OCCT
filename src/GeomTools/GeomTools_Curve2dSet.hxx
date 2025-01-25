// Created on: 1993-07-19
// Created by: Remi LEQUETTE
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

#ifndef _GeomTools_Curve2dSet_HeaderFile
#define _GeomTools_Curve2dSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_IndexedMapOfTransient.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <Message_ProgressRange.hxx>

class Geom2d_Curve;

//! Stores a set of Curves from Geom2d.
class GeomTools_Curve2dSet
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns an empty set of Curves.
  Standard_EXPORT GeomTools_Curve2dSet();

  //! Clears the content of the set.
  Standard_EXPORT void Clear();

  //! Incorporate a new Curve in the  set and returns
  //! its index.
  Standard_EXPORT Standard_Integer Add(const Handle(Geom2d_Curve)& C);

  //! Returns the Curve of index <I>.
  Standard_EXPORT Handle(Geom2d_Curve) Curve2d(const Standard_Integer I) const;

  //! Returns the index of <L>.
  Standard_EXPORT Standard_Integer Index(const Handle(Geom2d_Curve)& C) const;

  //! Dumps the content of me on the stream <OS>.
  Standard_EXPORT void Dump(Standard_OStream& OS) const;

  //! Writes the content of  me  on the stream <OS> in a
  //! format that can be read back by Read.
  Standard_EXPORT void Write(
    Standard_OStream&            OS,
    const Message_ProgressRange& theProgress = Message_ProgressRange()) const;

  //! Reads the content of me from the  stream  <IS>. me
  //! is first cleared.
  Standard_EXPORT void Read(Standard_IStream&            IS,
                            const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Dumps the curve on the stream,  if compact is True
  //! use the compact format that can be read back.
  Standard_EXPORT static void PrintCurve2d(const Handle(Geom2d_Curve)& C,
                                           Standard_OStream&           OS,
                                           const Standard_Boolean      compact = Standard_False);

  //! Reads the curve  from  the stream.  The  curve  is
  //! assumed   to have  been  written  with  the Print
  //! method (compact = True).
  Standard_EXPORT static Handle(Geom2d_Curve) ReadCurve2d(Standard_IStream& IS);

private:
  TColStd_IndexedMapOfTransient myMap;
};

#endif // _GeomTools_Curve2dSet_HeaderFile
