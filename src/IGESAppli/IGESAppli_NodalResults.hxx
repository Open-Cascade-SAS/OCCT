// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Arun  MENON )
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

#ifndef _IGESAppli_NodalResults_HeaderFile
#define _IGESAppli_NodalResults_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESAppli_HArray1OfNode.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESDimen_GeneralNote;
class IGESAppli_Node;

class IGESAppli_NodalResults;
DEFINE_STANDARD_HANDLE(IGESAppli_NodalResults, IGESData_IGESEntity)

//! defines NodalResults, Type <146>
//! in package IGESAppli
//! Used to store the Analysis Data results per FEM Node
class IGESAppli_NodalResults : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESAppli_NodalResults();

  //! This method is used to set the fields of the class
  //! NodalResults
  //! - aNote              : General Note that describes the
  //! analysis case
  //! - aNumber            : Analysis Subcase number
  //! - aTime              : Analysis time
  //! - allNodeIdentifiers : Node identifiers for the nodes
  //! - allNodes           : List of FEM Node Entities
  //! - allData            : Values of the Finite Element analysis
  //! result data
  //! raises exception if Lengths of allNodeIdentifiers, allNodes and
  //! allData (Cols) are not same
  Standard_EXPORT void Init(const Handle(IGESDimen_GeneralNote)&    aNote,
                            const Standard_Integer                  aNumber,
                            const Standard_Real                     aTime,
                            const Handle(TColStd_HArray1OfInteger)& allNodeIdentifiers,
                            const Handle(IGESAppli_HArray1OfNode)&  allNodes,
                            const Handle(TColStd_HArray2OfReal)&    allData);

  //! Changes the FormNumber (which indicates Type of Result)
  //! Error if not in range [0-34]
  Standard_EXPORT void SetFormNumber(const Standard_Integer form);

  //! returns the General Note Entity that describes the analysis case
  Standard_EXPORT Handle(IGESDimen_GeneralNote) Note() const;

  //! returns zero if there is no subcase
  Standard_EXPORT Standard_Integer SubCaseNumber() const;

  //! returns the Analysis time value for this subcase. It is the time
  //! at which transient analysis results occur in the mathematical
  //! FEM model.
  Standard_EXPORT Standard_Real Time() const;

  //! returns number of real values in array V for a FEM node
  Standard_EXPORT Standard_Integer NbData() const;

  //! returns number of FEM nodes for which data is to be read.
  Standard_EXPORT Standard_Integer NbNodes() const;

  //! returns FEM node number identifier for the (Index)th node
  //! raises exception if Index <= 0 or Index > NbNodes
  Standard_EXPORT Standard_Integer NodeIdentifier(const Standard_Integer Index) const;

  //! returns the node as specified by the Index
  //! raises exception if Index <= 0 or Index > NbNodes
  Standard_EXPORT Handle(IGESAppli_Node) Node(const Standard_Integer Index) const;

  //! returns the finite element analysis result value
  //! raises exception if (NodeNum <= 0 or NodeNum > NbNodes()) or
  //! if (DataNum <=0 or DataNum > NbData())
  Standard_EXPORT Standard_Real Data(const Standard_Integer NodeNum,
                                     const Standard_Integer DataNum) const;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_NodalResults, IGESData_IGESEntity)

protected:
private:
  Handle(IGESDimen_GeneralNote)    theNote;
  Standard_Integer                 theSubCaseNum;
  Standard_Real                    theTime;
  Handle(TColStd_HArray1OfInteger) theNodeIdentifiers;
  Handle(IGESAppli_HArray1OfNode)  theNodes;
  Handle(TColStd_HArray2OfReal)    theData;
};

#endif // _IGESAppli_NodalResults_HeaderFile
