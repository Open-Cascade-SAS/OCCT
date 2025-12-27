// Created on: 1993-11-18
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

#ifndef _MAT2d_Circuit_HeaderFile
#define _MAT2d_Circuit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <MAT2d_Connexion.hxx>
#include <NCollection_DataMap.hxx>
#include <MAT2d_BiInt.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <GeomAbs_JoinType.hxx>
#include <Standard_Transient.hxx>
#include <Geom2d_Geometry.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <MAT2d_Connexion.hxx>
#include <NCollection_Sequence.hxx>
class Geom2d_Geometry;
class MAT2d_Connexion;
class MAT2d_BiInt;
class MAT2d_MiniPath;

//! Constructs a circuit on a set of lines.
//! EquiCircuit gives a Circuit passing by all the lines
//! in a set and all the connexions of the minipath associated.
class MAT2d_Circuit : public Standard_Transient
{

public:
  Standard_EXPORT MAT2d_Circuit(const GeomAbs_JoinType aJoinType    = GeomAbs_Arc,
                                const bool IsOpenResult = false);

  Standard_EXPORT void Perform(NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Geometry>>>& aFigure,
                               const NCollection_Sequence<bool>&    IsClosed,
                               const int              IndRefLine,
                               const bool              Trigo);

  //! Returns the Number of Items .
  Standard_EXPORT int NumberOfItems() const;

  //! Returns the item at position <Index> in <me>.
  Standard_EXPORT occ::handle<Geom2d_Geometry> Value(const int Index) const;

  //! Returns the number of items on the line <IndexLine>.
  Standard_EXPORT int LineLength(const int IndexLine) const;

  //! Returns the set of index of the items in <me>corresponding
  //! to the curve <IndCurve> on the line <IndLine> from the
  //! initial figure.
  Standard_EXPORT const NCollection_Sequence<int>& RefToEqui(const int IndLine,
                                                             const int IndCurve) const;

  //! Returns the Connexion on the item <Index> in me.
  Standard_EXPORT occ::handle<MAT2d_Connexion> Connexion(const int Index) const;

  //! Returns <True> is there is a connexion on the item <Index>
  //! in <me>.
  Standard_EXPORT bool ConnexionOn(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(MAT2d_Circuit, Standard_Transient)

private:
  Standard_EXPORT bool IsSharpCorner(const occ::handle<Geom2d_Geometry>& Geom1,
                                                 const occ::handle<Geom2d_Geometry>& Geom2,
                                                 const double            Direction) const;

  Standard_EXPORT bool PassByLast(const occ::handle<MAT2d_Connexion>& C1,
                                              const occ::handle<MAT2d_Connexion>& C2) const;

  Standard_EXPORT double Side(const occ::handle<MAT2d_Connexion>&       C,
                                     const NCollection_Sequence<occ::handle<Geom2d_Geometry>>& Line) const;

  Standard_EXPORT void UpDateLink(const int IFirst,
                                  const int ILine,
                                  const int ICurveFirst,
                                  const int ICurveLast);

  Standard_EXPORT void SortRefToEqui(const MAT2d_BiInt& aBiInt);

  Standard_EXPORT void InitOpen(NCollection_Sequence<occ::handle<Geom2d_Geometry>>& Line) const;

  Standard_EXPORT void InsertCorner(NCollection_Sequence<occ::handle<Geom2d_Geometry>>& Line) const;

  Standard_EXPORT void DoubleLine(NCollection_Sequence<occ::handle<Geom2d_Geometry>>& Line,
                                  NCollection_Sequence<occ::handle<MAT2d_Connexion>>&     Connexions,
                                  const occ::handle<MAT2d_Connexion>& Father,
                                  const double            Side) const;

  Standard_EXPORT void ConstructCircuit(const NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Geometry>>>& aFigure,
                                        const int                    IndRefLine,
                                        const MAT2d_MiniPath&                     aPath);

  double                         direction;
  NCollection_Sequence<occ::handle<Geom2d_Geometry>>         geomElements;
  NCollection_DataMap<int, occ::handle<MAT2d_Connexion>>       connexionMap;
  NCollection_DataMap<MAT2d_BiInt, NCollection_Sequence<int>> linkRefEqui;
  NCollection_Sequence<int>             linesLength;
  GeomAbs_JoinType                      myJoinType;
  bool                      myIsOpenResult;
};

#endif // _MAT2d_Circuit_HeaderFile
