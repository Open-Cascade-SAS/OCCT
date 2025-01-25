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

#ifndef _IGESAppli_FiniteElement_HeaderFile
#define _IGESAppli_FiniteElement_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESAppli_HArray1OfNode.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;
class IGESAppli_Node;

class IGESAppli_FiniteElement;
DEFINE_STANDARD_HANDLE(IGESAppli_FiniteElement, IGESData_IGESEntity)

//! defines FiniteElement, Type <136> Form <0>
//! in package IGESAppli
//! Used to define a finite element with the help of an
//! element topology.
class IGESAppli_FiniteElement : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESAppli_FiniteElement();

  //! This method is used to set the fields of the class
  //! FiniteElement
  //! - aType    : Indicates the topology type
  //! - allNodes : List of Nodes defining the element
  //! - aName    : Element type name
  Standard_EXPORT void Init(const Standard_Integer                  aType,
                            const Handle(IGESAppli_HArray1OfNode)&  allNodes,
                            const Handle(TCollection_HAsciiString)& aName);

  //! returns Topology type
  Standard_EXPORT Standard_Integer Topology() const;

  //! returns the number of nodes defining the element
  Standard_EXPORT Standard_Integer NbNodes() const;

  //! returns Node defining element entity
  //! raises exception if Index <= 0 or Index > NbNodes()
  Standard_EXPORT Handle(IGESAppli_Node) Node(const Standard_Integer Index) const;

  //! returns Element Type Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_FiniteElement, IGESData_IGESEntity)

protected:
private:
  Standard_Integer                 theTopology;
  Handle(IGESAppli_HArray1OfNode)  theNodes;
  Handle(TCollection_HAsciiString) theName;
};

#endif // _IGESAppli_FiniteElement_HeaderFile
