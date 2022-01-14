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

#include <MAT_DataMapOfIntegerArc.hxx>
#include <MAT_DataMapOfIntegerBasicElt.hxx>
#include <MAT_DataMapOfIntegerNode.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
class MAT_ListOfBisector;
class MAT_Arc;
class MAT_BasicElt;
class MAT_Node;


class MAT_Graph;
DEFINE_STANDARD_HANDLE(MAT_Graph, Standard_Transient)

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
  Standard_EXPORT void Perform (const Standard_Boolean SemiInfinite, const Handle(MAT_ListOfBisector)& TheRoots, const Standard_Integer NbBasicElts, const Standard_Integer NbArcs);
  
  //! Return the Arc of index <Index> in <theArcs>.
  Standard_EXPORT Handle(MAT_Arc) Arc (const Standard_Integer Index) const;
  
  //! Return the BasicElt of index <Index> in <theBasicElts>.
  Standard_EXPORT Handle(MAT_BasicElt) BasicElt (const Standard_Integer Index) const;
  
  //! Return the Node of index <Index> in <theNodes>.
  Standard_EXPORT Handle(MAT_Node) Node (const Standard_Integer Index) const;
  
  //! Return the number of arcs of <me>.
  Standard_EXPORT Standard_Integer NumberOfArcs() const;
  
  //! Return the number of nodes of <me>.
  Standard_EXPORT Standard_Integer NumberOfNodes() const;
  
  //! Return the number of basic elements of <me>.
  Standard_EXPORT Standard_Integer NumberOfBasicElts() const;
  
  //! Return the number of infinites nodes of <me>.
  Standard_EXPORT Standard_Integer NumberOfInfiniteNodes() const;
  
  //! Merge two BasicElts.  The End of the BasicElt Elt1
  //! of  IndexElt1 becomes The End of the BasicElt Elt2
  //! of  IndexElt2.   Elt2 is replaced in  the  arcs  by
  //! Elt1, Elt2 is eliminated.
  //!
  //! <MergeArc1> is True  if the fusion  of the BasicElts  =>
  //! a fusion  of two Arcs which separated  the same  elements.
  //! In this case <GeomIndexArc1> and  <GeomIndexArc2>  are the
  //! Geometric  Index of this  arcs.
  //!
  //! If the  BasicElt corresponds to a close line ,
  //! the StartArc and the EndArc of Elt1 can separate the same
  //! elements .
  //! In this case there is a fusion of this arcs, <MergeArc2>
  //! is true and <GeomIndexArc3> and  <GeomIndexArc4>  are the
  //! Geometric  Index of this  arcs.
  Standard_EXPORT void FusionOfBasicElts (const Standard_Integer IndexElt1, const Standard_Integer IndexElt2, Standard_Boolean& MergeArc1, Standard_Integer& GeomIndexArc1, Standard_Integer& GeomIndexArc2, Standard_Boolean& MergeArc2, Standard_Integer& GeomIndexArc3, Standard_Integer& GeomIndexArc4);
  
  Standard_EXPORT void CompactArcs();
  
  Standard_EXPORT void CompactNodes();
  
  Standard_EXPORT void ChangeBasicElts (const MAT_DataMapOfIntegerBasicElt& NewMap);
  
  Standard_EXPORT Handle(MAT_BasicElt) ChangeBasicElt (const Standard_Integer Index);




  DEFINE_STANDARD_RTTIEXT(MAT_Graph,Standard_Transient)

protected:




private:

  
  //! Merge two Arcs.  the second node of <Arc2> becomes
  //! the  first node  of <Arc1>.  Update  of the  first
  //! node and the neighbours of <Arc1>.
  //! <Arc2> is eliminated.
  Standard_EXPORT void FusionOfArcs (const Handle(MAT_Arc)& Arc1, const Handle(MAT_Arc)& Arc2);
  
  Standard_EXPORT void UpDateNodes (Standard_Integer& Index);

  MAT_DataMapOfIntegerArc theArcs;
  MAT_DataMapOfIntegerBasicElt theBasicElts;
  MAT_DataMapOfIntegerNode theNodes;
  Standard_Integer numberOfArcs;
  Standard_Integer numberOfNodes;
  Standard_Integer numberOfBasicElts;
  Standard_Integer numberOfInfiniteNodes;


};







#endif // _MAT_Graph_HeaderFile
