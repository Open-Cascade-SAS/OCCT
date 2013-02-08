// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef NMTDS_BoxBndTree_HeaderFile
#define NMTDS_BoxBndTree_HeaderFile

#include <NCollection_UBTree.hxx>
#include <Bnd_Box.hxx>
#include <BOPCol_ListOfInteger.hxx>
/**
 * The instantiation of the algorithm of unbalanced binary tree 
 * of overlapped bounding boxes.
 *
*/
typedef NCollection_UBTree <Standard_Integer , Bnd_Box> BOPDS_BoxBndTree;

  class BOPDS_BoxBndTreeSelector : public BOPDS_BoxBndTree::Selector {
    public:
      Standard_EXPORT BOPDS_BoxBndTreeSelector();
      Standard_EXPORT virtual Standard_Boolean Reject(const Bnd_Box&) const;
      Standard_EXPORT virtual Standard_Boolean Accept(const Standard_Integer &);
      Standard_EXPORT virtual ~BOPDS_BoxBndTreeSelector();
      
      Standard_EXPORT void Clear();
      Standard_EXPORT void SetBox(const Bnd_Box&);
      Standard_EXPORT const BOPCol_ListOfInteger& Indices() const;

    protected:
      Bnd_Box  myBox;
      BOPCol_ListOfInteger myIndices;
      
  };

#endif
