// Created on: 2000-09-27
// Created by: Pavel TELKOV.
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_GraphNode_HeaderFile
#define _XCAFDoc_GraphNode_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XCAFDoc_GraphNodeSequence.hxx>
#include <Standard_GUID.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class Standard_GUID;
class TDF_Attribute;
class TDF_RelocationTable;
class TDF_DataSet;


class XCAFDoc_GraphNode;
DEFINE_STANDARD_HANDLE(XCAFDoc_GraphNode, TDF_Attribute)

//! This attribute allow user multirelation tree of labels.
//! This GraphNode is experimental Graph that not control looping and redundance.
//! Attribute containg sequence of father's and child's labels.
//! Provide create and work with Graph in XCAFDocument.
class XCAFDoc_GraphNode : public TDF_Attribute
{

public:

  
  //! class  methods working on the node
  //! ===================================
  //! Shortcut to search  a Graph node attribute with default
  //! GraphID.  Returns true if found.
  Standard_EXPORT static Standard_Boolean Find (const TDF_Label& L, Handle(XCAFDoc_GraphNode)& G);
  
  //! Finds or Creates a GraphNode attribute on the label <L>
  //! with  the  default Graph  ID,   returned by the method
  //! <GetDefaultGraphID>.  Returns the created/found     GraphNode
  //! attribute.
  Standard_EXPORT static Handle(XCAFDoc_GraphNode) Set (const TDF_Label& L);
  
  //! Finds  or Creates a   GraphNode attribute on  the label
  //! <L>, with an   explicit tree ID.  <ExplicitGraphID>  is
  //! the  ID   returned by    <TDF_Attribute::ID>   method.
  //! Returns the found/created GraphNode attribute.
  Standard_EXPORT static Handle(XCAFDoc_GraphNode) Set (const TDF_Label& L, const Standard_GUID& ExplicitGraphID);
  
  //! returns a default  Graph ID.  this  ID is  used by the
  //! <Set> method without explicit tree ID.
  //! Instance methods:
  //! ================
  Standard_EXPORT static const Standard_GUID& GetDefaultGraphID();
  
  Standard_EXPORT XCAFDoc_GraphNode();
  
  Standard_EXPORT void SetGraphID (const Standard_GUID& explicitID);
  
  //! Set GraphNode <F> as father of me and returns index of <F>
  //! in Sequence that containing Fathers GraphNodes.
  //! return index of <F> from GraphNodeSequnece
  Standard_EXPORT Standard_Integer SetFather (const Handle(XCAFDoc_GraphNode)& F);
  
  //! Set GraphNode <Ch> as child of me and returns index of <Ch>
  //! in Sequence that containing Children GraphNodes.
  //! return index of <Ch> from GraphNodeSequnece
  Standard_EXPORT Standard_Integer SetChild (const Handle(XCAFDoc_GraphNode)& Ch);
  
  //! Remove <F> from Fathers GraphNodeSequence.
  //! and remove link between father and child.
  Standard_EXPORT void UnSetFather (const Handle(XCAFDoc_GraphNode)& F);
  
  //! Remove Father GraphNode by index from Fathers GraphNodeSequence.
  //! and remove link between father and child.
  Standard_EXPORT void UnSetFather (const Standard_Integer Findex);
  
  //! Remove <Ch> from GraphNodeSequence.
  //! and remove link between father and child.
  Standard_EXPORT void UnSetChild (const Handle(XCAFDoc_GraphNode)& Ch);
  
  //! Remove Child GraphNode by index from Children GraphNodeSequence.
  //! and remove link between father and child.
  Standard_EXPORT void UnSetChild (const Standard_Integer Chindex);
  
  //! Return GraphNode by index from GraphNodeSequence.
  Standard_EXPORT Handle(XCAFDoc_GraphNode) GetFather (const Standard_Integer Findex) const;
  
  //! Return GraphNode by index from GraphNodeSequence.
  Standard_EXPORT Handle(XCAFDoc_GraphNode) GetChild (const Standard_Integer Chindex) const;
  
  //! Return index of <F>, or zero if there is no such Graphnode.
  Standard_EXPORT Standard_Integer FatherIndex (const Handle(XCAFDoc_GraphNode)& F) const;
  
  //! Return index of <Ch>, or zero if there is no such Graphnode.
  Standard_EXPORT Standard_Integer ChildIndex (const Handle(XCAFDoc_GraphNode)& Ch) const;
  
  //! returns TRUE if <me> is father of <Ch>.
  Standard_EXPORT Standard_Boolean IsFather (const Handle(XCAFDoc_GraphNode)& Ch) const;
  
  //! returns TRUE if <me> is child of <F>.
  Standard_EXPORT Standard_Boolean IsChild (const Handle(XCAFDoc_GraphNode)& F) const;
  
  //! return Number of Fathers GraphNodes.
  Standard_EXPORT Standard_Integer NbFathers() const;
  
  //! return Number of Childrens GraphNodes.
  //! Implementation of Attribute methods:
  //! ===================================
  Standard_EXPORT Standard_Integer NbChildren() const;
  
  //! Returns the Graph ID (default or explicit one depending
  //! onthe Set method used).
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void References (const Handle(TDF_DataSet)& aDataSet) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void BeforeForget() Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XCAFDoc_GraphNode,TDF_Attribute)

protected:




private:

  
  //! remove link between father and child.
  Standard_EXPORT void UnSetFatherlink (const Handle(XCAFDoc_GraphNode)& F);
  
  //! remove link between father and child.
  Standard_EXPORT void UnSetChildlink (const Handle(XCAFDoc_GraphNode)& C);

  XCAFDoc_GraphNodeSequence myFathers;
  XCAFDoc_GraphNodeSequence myChildren;
  Standard_GUID myGraphID;


};







#endif // _XCAFDoc_GraphNode_HeaderFile
