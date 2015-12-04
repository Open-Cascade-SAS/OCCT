// Created on: 1999-02-24
// Created by: Christian CAILLET
// Copyright (c) 1999 Matra Datavision
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

#ifndef _XSControl_ConnectedShapes_HeaderFile
#define _XSControl_ConnectedShapes_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExplore.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopAbs_ShapeEnum.hxx>
class XSControl_TransferReader;
class Standard_Transient;
class Interface_Graph;
class Interface_EntityIterator;
class TCollection_AsciiString;
class TopoDS_Shape;
class Transfer_TransientProcess;


class XSControl_ConnectedShapes;
DEFINE_STANDARD_HANDLE(XSControl_ConnectedShapes, IFSelect_SelectExplore)

//! From a TopoDS_Shape, or from the entity which has produced it,
//! searches for the shapes, and the entities which have produced
//! them in last transfer, which are adjacent to it by VERTICES
class XSControl_ConnectedShapes : public IFSelect_SelectExplore
{

public:

  
  //! Creates a Selection ConnectedShapes. It remains to be set a
  //! TransferReader
  Standard_EXPORT XSControl_ConnectedShapes();
  
  //! Creates a Selection ConnectedShapes, which will work with the
  //! current TransferProcess brought by the TransferReader
  Standard_EXPORT XSControl_ConnectedShapes(const Handle(XSControl_TransferReader)& TR);
  
  //! Sets a TransferReader to sort entities : it brings the
  //! TransferProcess which may change, while the TransferReader does not
  Standard_EXPORT void SetReader (const Handle(XSControl_TransferReader)& TR);
  
  //! Explores an entity : entities from which are connected to that
  //! produced by this entity, including itself
  Standard_EXPORT Standard_Boolean Explore (const Standard_Integer level, const Handle(Standard_Transient)& ent, const Interface_Graph& G, Interface_EntityIterator& explored) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium.
  //! "Connected Entities through produced Shapes"
  Standard_EXPORT TCollection_AsciiString ExploreLabel() const Standard_OVERRIDE;
  
  //! This functions considers a shape from a transfer and performs
  //! the search function explained above
  Standard_EXPORT static Handle(TColStd_HSequenceOfTransient) AdjacentEntities (const TopoDS_Shape& ashape, const Handle(Transfer_TransientProcess)& TP, const TopAbs_ShapeEnum type);




  DEFINE_STANDARD_RTTIEXT(XSControl_ConnectedShapes,IFSelect_SelectExplore)

protected:




private:


  Handle(XSControl_TransferReader) theTR;


};







#endif // _XSControl_ConnectedShapes_HeaderFile
