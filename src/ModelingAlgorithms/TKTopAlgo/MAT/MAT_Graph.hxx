// Created on: 1993-04-29
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

#ifndef _MAT_Graph_HeaderFile
#define _MAT_Graph_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <MAT_Arc.hxx>
#include <NCollection_DataMap.hxx>
#include <MAT_BasicElt.hxx>
#include <MAT_Node.hxx>
#include <Standard_Transient.hxx>
class MAT_ListOfBisector;
class MAT_Arc;
class MAT_BasicElt;
class MAT_Node;

//! The Class Graph permits the exploration of the
//! Bisector Locus.
class MAT_Graph : public Standard_Transient
{

public:
  //! Empty constructor.
  Standard_EXPORT MAT_Graph();

  //! Construct <me> from the result of the method
  //! <CreateMat> of the class <MAT> from <MAT>.
  //!
  //! <SemiInfinite> : if some bisector are infinites.
  //! <TheRoots>     : Set of the bisectors.
  //! <NbBasicElts>  : Number of Basic Elements.
  //! <NbArcs>       : Number of Arcs = Number of Bisectors.
  Standard_EXPORT void Perform(const bool                             SemiInfinite,
                               const occ::handle<MAT_ListOfBisector>& TheRoots,
                               const int                              NbBasicElts,
                               const int                              NbArcs);

  //! Return the Arc of index <Index> in <theArcs>.
  Standard_EXPORT occ::handle<MAT_Arc> Arc(const int Index) const;

  //! Return the BasicElt of index <Index> in <theBasicElts>.
  Standard_EXPORT occ::handle<MAT_BasicElt> BasicElt(const int Index) const;

  //! Return the Node of index <Index> in <theNodes>.
  Standard_EXPORT occ::handle<MAT_Node> Node(const int Index) const;

  //! Return the number of arcs of <me>.
  Standard_EXPORT int NumberOfArcs() const;

  //! Return the number of nodes of <me>.
  Standard_EXPORT int NumberOfNodes() const;

  //! Return the number of basic elements of <me>.
  Standard_EXPORT int NumberOfBasicElts() const;

  //! Return the number of infinites nodes of <me>.
  Standard_EXPORT int NumberOfInfiniteNodes() const;

  //! Merge two BasicElts. The End of the BasicElt Elt1
  //! of IndexElt1 becomes The End of the BasicElt Elt2
  //! of IndexElt2. Elt2 is replaced in the arcs by
  //! Elt1, Elt2 is eliminated.
  //!
  //! <MergeArc1> is True if the fusion of the BasicElts =>
  //! a fusion of two Arcs which separated the same elements.
  //! In this case <GeomIndexArc1> and <GeomIndexArc2> are the
  //! Geometric Index of this arcs.
  //!
  //! If the BasicElt corresponds to a close line,
  //! the StartArc and the EndArc of Elt1 can separate the same
  //! elements.
  //! In this case there is a fusion of this arcs, <MergeArc2>
  //! is true and <GeomIndexArc3> and <GeomIndexArc4> are the
  //! Geometric Index of this arcs.
  Standard_EXPORT void FusionOfBasicElts(const int IndexElt1,
                                         const int IndexElt2,
                                         bool&     MergeArc1,
                                         int&      GeomIndexArc1,
                                         int&      GeomIndexArc2,
                                         bool&     MergeArc2,
                                         int&      GeomIndexArc3,
                                         int&      GeomIndexArc4);

  Standard_EXPORT void CompactArcs();

  Standard_EXPORT void CompactNodes();

  Standard_EXPORT void ChangeBasicElts(
    const NCollection_DataMap<int, occ::handle<MAT_BasicElt>>& NewMap);

  Standard_EXPORT occ::handle<MAT_BasicElt> ChangeBasicElt(const int Index);

  DEFINE_STANDARD_RTTIEXT(MAT_Graph, Standard_Transient)

private:
  //! Merge two Arcs. the second node of <Arc2> becomes
  //! the first node of <Arc1>. Update of the first
  //! node and the neighbours of <Arc1>.
  //! <Arc2> is eliminated.
  Standard_EXPORT void FusionOfArcs(const occ::handle<MAT_Arc>& Arc1,
                                    const occ::handle<MAT_Arc>& Arc2);

  Standard_EXPORT void UpDateNodes(int& Index);

  NCollection_DataMap<int, occ::handle<MAT_Arc>>      theArcs;
  NCollection_DataMap<int, occ::handle<MAT_BasicElt>> theBasicElts;
  NCollection_DataMap<int, occ::handle<MAT_Node>>     theNodes;
  int                                                 numberOfArcs;
  int                                                 numberOfNodes;
  int                                                 numberOfBasicElts;
  int                                                 numberOfInfiniteNodes;
};

#endif // _MAT_Graph_HeaderFile
