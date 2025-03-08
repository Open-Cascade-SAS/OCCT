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

#include <IGESDimen_HArray1OfGeneralNote.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESAppli_HArray1OfNode.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESBasic_HArray1OfHArray1OfXYZ;
class IGESDimen_GeneralNote;
class IGESAppli_Node;
class gp_XYZ;

class IGESAppli_NodalDisplAndRot;
DEFINE_STANDARD_HANDLE(IGESAppli_NodalDisplAndRot, IGESData_IGESEntity)

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
  Standard_EXPORT void Init(const Handle(IGESDimen_HArray1OfGeneralNote)&  allNotes,
                            const Handle(TColStd_HArray1OfInteger)&        allIdentifiers,
                            const Handle(IGESAppli_HArray1OfNode)&         allNodes,
                            const Handle(IGESBasic_HArray1OfHArray1OfXYZ)& allRotParams,
                            const Handle(IGESBasic_HArray1OfHArray1OfXYZ)& allTransParams);

  //! returns the number of analysis cases
  Standard_EXPORT Standard_Integer NbCases() const;

  //! returns the number of nodes
  Standard_EXPORT Standard_Integer NbNodes() const;

  //! returns the General Note that describes the Index analysis case
  //! raises exception if Index <= 0 or Index > NbCases
  Standard_EXPORT Handle(IGESDimen_GeneralNote) Note(const Standard_Integer Index) const;

  //! returns the node identifier as specified by the Index
  //! raises exception if Index <= 0 or Index > NbNodes
  Standard_EXPORT Standard_Integer NodeIdentifier(const Standard_Integer Index) const;

  //! returns the node as specified by the Index
  //! raises exception if Index <= 0 or Index > NbNodes
  Standard_EXPORT Handle(IGESAppli_Node) Node(const Standard_Integer Index) const;

  //! returns the Translational Parameters for the particular Index
  //! Exception raised if NodeNum <= 0 or NodeNum > NbNodes()
  //! or CaseNum <= 0 or CaseNum > NbCases()
  Standard_EXPORT gp_XYZ TranslationParameter(const Standard_Integer NodeNum,
                                              const Standard_Integer CaseNum) const;

  //! returns the Rotational Parameters for Index
  //! Exception raised if NodeNum <= 0 or NodeNum > NbNodes()
  //! or CaseNum <= 0 or CaseNum > NbCases()
  Standard_EXPORT gp_XYZ RotationalParameter(const Standard_Integer NodeNum,
                                             const Standard_Integer CaseNum) const;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_NodalDisplAndRot, IGESData_IGESEntity)

protected:
private:
  Handle(IGESDimen_HArray1OfGeneralNote)  theNotes;
  Handle(TColStd_HArray1OfInteger)        theNodeIdentifiers;
  Handle(IGESAppli_HArray1OfNode)         theNodes;
  Handle(IGESBasic_HArray1OfHArray1OfXYZ) theTransParam;
  Handle(IGESBasic_HArray1OfHArray1OfXYZ) theRotParam;
};

#endif // _IGESAppli_NodalDisplAndRot_HeaderFile
