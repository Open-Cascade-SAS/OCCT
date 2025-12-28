// Created on: 1993-10-07
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

#ifndef _MAT2d_MiniPath_HeaderFile
#define _MAT2d_MiniPath_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <MAT2d_Connexion.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_DataMap.hxx>
#include <Geom2d_Geometry.hxx>
class MAT2d_Connexion;

//! MiniPath computes a path to link all the lines in
//! a set of lines. The path is described as a set of
//! connexions.
//!
//! The set of connexions can be seen as an arbitrary Tree.
//! The node of the tree are the lines. The arcs of the
//! tree are the connexions. The ancestror of a line is
//! the connexion which ends on it. The children of a line
//! are the connexions which start on it.
//!
//! The children of a line are ordered by the relation
//! <IsAfter> defined on the connexions.
//! (See MAT2s_Connexion.cdl).
class MAT2d_MiniPath
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT MAT2d_MiniPath();

  //! Computes the path to link the lines in <Figure>.
  //! the path starts on the line of index <IndStart>
  //! <Sense> = True if the Circuit turns in the
  //! trigonometric sense.
  Standard_EXPORT void Perform(
    const NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Geometry>>>& Figure,
    const int                                                                       IndStart,
    const bool                                                                      Sense);

  //! Run on the set of connexions to compute the path.
  //! the path is an exploration of the tree which contains
  //! the connexions and their reverses.
  //! if the tree of connexions is
  //! A
  //! / |
  //! B  E
  //! / |  |
  //! C  D  F
  //!
  //! the path is A->B, B->C, C->B, B->D, D->B, B->A, A->E,
  //! E->F, F->E, E->A.
  Standard_EXPORT void RunOnConnexions();

  //! Returns the sequence of connexions corresponding to
  //! the path.
  Standard_EXPORT const NCollection_Sequence<occ::handle<MAT2d_Connexion>>& Path() const;

  //! Returns <True> if there is one Connexion which starts
  //! on line designed by <Index>.
  Standard_EXPORT bool IsConnexionsFrom(const int Index) const;

  //! Returns the connexions which start on line
  //! designed by <Index>.
  Standard_EXPORT NCollection_Sequence<occ::handle<MAT2d_Connexion>>& ConnexionsFrom(
    const int Index);

  //! Returns <True> if the line designed by <Index> is
  //! the root.
  Standard_EXPORT bool IsRoot(const int Index) const;

  //! Returns the connexion which ends on line
  //! designed by <Index>.
  Standard_EXPORT occ::handle<MAT2d_Connexion> Father(const int Index);

private:
  //! Add a connexion to the path.
  Standard_EXPORT void Append(const occ::handle<MAT2d_Connexion>& Connexion);

  Standard_EXPORT void ExploSons(NCollection_Sequence<occ::handle<MAT2d_Connexion>>& aPath,
                                 const occ::handle<MAT2d_Connexion>&                 aConnexion);

  //! Returns the connexion which realises the minimum of
  //! distance between the lines of index <L1> and <L2> in
  //! <aFigure>. The connexion is oriented from <L1> to <L2>.
  Standard_EXPORT occ::handle<MAT2d_Connexion> MinimumL1L2(
    const NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Geometry>>>& Figure,
    const int                                                                       L1,
    const int                                                                       L2) const;

  NCollection_DataMap<int, NCollection_Sequence<occ::handle<MAT2d_Connexion>>> theConnexions;
  NCollection_DataMap<int, occ::handle<MAT2d_Connexion>>                       theFather;
  NCollection_Sequence<occ::handle<MAT2d_Connexion>>                           thePath;
  double                                                                       theDirection;
  int                                                                          indStart;
};

#endif // _MAT2d_MiniPath_HeaderFile
