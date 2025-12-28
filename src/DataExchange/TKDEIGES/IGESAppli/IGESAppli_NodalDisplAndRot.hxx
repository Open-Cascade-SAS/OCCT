// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
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

#ifndef _IGESAppli_NodalDisplAndRot_HeaderFile
#define _IGESAppli_NodalDisplAndRot_HeaderFile

#include <Standard.hxx>

#include <IGESDimen_GeneralNote.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESBasic_HArray1OfHArray1OfXYZ;
class IGESDimen_GeneralNote;
class IGESAppli_Node;
class gp_XYZ;

//! defines NodalDisplAndRot, Type <138> Form <0>
//! in package IGESAppli
//! Used to communicate finite element post processing
//! data.
class IGESAppli_NodalDisplAndRot : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESAppli_NodalDisplAndRot();

  //! This method is used to set the fields of the class
  //! NodalDisplAndRot
  //! - allNotes       : Used to store the general note describing
  //! the analysis cases
  //! - allIdentifiers : Used to store the node number
  //! identifier for the nodes
  //! - allNodes       : Used to store the nodes
  //! - allRotParams   : Used to store the rotation for the nodes
  //! - allTransParams : Used to store the incremental
  //! displacements for the nodes
  //! raises exception if Lengths of allIdentifiers, allNodes,
  //! allRotParams, and allTransParams are not same
  //! or if length of allNotes and size of each element of allRotParams
  //! and allTransParam are not same
  Standard_EXPORT void Init(
    const occ::handle<NCollection_HArray1<occ::handle<IGESDimen_GeneralNote>>>& allNotes,
    const occ::handle<NCollection_HArray1<int>>&                                allIdentifiers,
    const occ::handle<NCollection_HArray1<occ::handle<IGESAppli_Node>>>&        allNodes,
    const occ::handle<IGESBasic_HArray1OfHArray1OfXYZ>&                         allRotParams,
    const occ::handle<IGESBasic_HArray1OfHArray1OfXYZ>&                         allTransParams);

  //! returns the number of analysis cases
  Standard_EXPORT int NbCases() const;

  //! returns the number of nodes
  Standard_EXPORT int NbNodes() const;

  //! returns the General Note that describes the Index analysis case
  //! raises exception if Index <= 0 or Index > NbCases
  Standard_EXPORT occ::handle<IGESDimen_GeneralNote> Note(const int Index) const;

  //! returns the node identifier as specified by the Index
  //! raises exception if Index <= 0 or Index > NbNodes
  Standard_EXPORT int NodeIdentifier(const int Index) const;

  //! returns the node as specified by the Index
  //! raises exception if Index <= 0 or Index > NbNodes
  Standard_EXPORT occ::handle<IGESAppli_Node> Node(const int Index) const;

  //! returns the Translational Parameters for the particular Index
  //! Exception raised if NodeNum <= 0 or NodeNum > NbNodes()
  //! or CaseNum <= 0 or CaseNum > NbCases()
  Standard_EXPORT gp_XYZ TranslationParameter(const int NodeNum, const int CaseNum) const;

  //! returns the Rotational Parameters for Index
  //! Exception raised if NodeNum <= 0 or NodeNum > NbNodes()
  //! or CaseNum <= 0 or CaseNum > NbCases()
  Standard_EXPORT gp_XYZ RotationalParameter(const int NodeNum, const int CaseNum) const;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_NodalDisplAndRot, IGESData_IGESEntity)

private:
  occ::handle<NCollection_HArray1<occ::handle<IGESDimen_GeneralNote>>> theNotes;
  occ::handle<NCollection_HArray1<int>>                                theNodeIdentifiers;
  occ::handle<NCollection_HArray1<occ::handle<IGESAppli_Node>>>        theNodes;
  occ::handle<IGESBasic_HArray1OfHArray1OfXYZ>                         theTransParam;
  occ::handle<IGESBasic_HArray1OfHArray1OfXYZ>                         theRotParam;
};

#endif // _IGESAppli_NodalDisplAndRot_HeaderFile
