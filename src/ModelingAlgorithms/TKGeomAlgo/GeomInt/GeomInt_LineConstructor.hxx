// Created on: 1995-02-07
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomInt_LineConstructor_HeaderFile
#define _GeomInt_LineConstructor_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <NCollection_Sequence.hxx>

class Adaptor3d_TopolTool;
class IntPatch_Line;

//! Splits given Line.
class GeomInt_LineConstructor
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  GeomInt_LineConstructor();

  //! Initializes me by two surfaces and corresponding
  //! tools which represent boundaries of surfaces
  void Load(const occ::handle<Adaptor3d_TopolTool>& D1,
            const occ::handle<Adaptor3d_TopolTool>& D2,
            const occ::handle<GeomAdaptor_Surface>& S1,
            const occ::handle<GeomAdaptor_Surface>& S2);

  //! Splits line
  Standard_EXPORT void Perform(const occ::handle<IntPatch_Line>& L);

  //! Returns True if splitting was successful
  bool IsDone() const;

  //! Returns number of splits
  int NbParts() const;

  //! Return first and last parameters
  //! for given index of split
  void Part(const int I, double& WFirst, double& WLast) const;

protected:
  Standard_EXPORT void TreatCircle(const occ::handle<IntPatch_Line>& aLine, const double aTol);

private:
  bool            done;
  NCollection_Sequence<double>      seqp;
  occ::handle<Adaptor3d_TopolTool> myDom1;
  occ::handle<Adaptor3d_TopolTool> myDom2;
  occ::handle<GeomAdaptor_Surface> myHS1;
  occ::handle<GeomAdaptor_Surface> myHS2;
};

#include <GeomInt_LineConstructor.lxx>

#endif // _GeomInt_LineConstructor_HeaderFile
