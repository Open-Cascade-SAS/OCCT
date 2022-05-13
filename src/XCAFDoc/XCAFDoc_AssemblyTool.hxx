// Created on: 2022-05-11
// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_AssemblyTool_HeaderFile
#define _XCAFDoc_AssemblyTool_HeaderFile

#include <Standard.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>
#include <XCAFDoc_AssemblyIterator.hxx>
#include <XCAFDoc_AssemblyGraph.hxx>

class TDocStd_Document;
class XCAFDoc_ShapeTool;

//! Provides generic methods for traversing assembly tree and graph
class XCAFDoc_AssemblyTool
{
public:

  //! \brief Generic method for traversing assembly tree.
  //! Performs in-depth traversing of the assembly tree and calls
  //! user defined function for each assembly tree node.
  //! User function takes single argument of XCAFDoc_AssemblyItemId type
  //! and returns true/false to continue/break.
  //! ~~~~~{.cpp}
  //! Standard_Boolean Print(const XCAFDoc_AssemblyItemId& theItem)
  //! {
  //!   std::cout << theItem.ToString() << std::endl;
  //!   return Standard_True;
  //! }
  //! ~~~~~
  //! \param [in] theIterator - starting position in the assembly tree.
  //! \param [in] theFunc     - user function called for each assembly tree node.
  template <typename Func>
  static void Traverse(XCAFDoc_AssemblyIterator theIterator,
                       Func                     theFunc)
  {
    for (; theIterator.More(); theIterator.Next())
    {
      if (!theFunc(theIterator.Current()))
        break;
    }
  }

  //! \brief Generic method for traversing assembly graph.
  //! Performs in-depth traversing of the assembly graph beginning from root nodes
  //! and calls user defined function for each assembly graph node accepted
  //! by the user defined filtering function. Filtering function takes
  //! the assembly graph passed for traversing, current graph node ID 
  //! and returns true/false to accept/reject node.
  //! ~~~~~{.cpp}
  //! Standard_Boolean AcceptPartOnly(const Handle(XCAFDoc_AssemblyGraph)& theGraph,
  //!                                 const Standard_Integer               theNode)
  //! {
  //!   return (theGraph->GetNodeType(theNode) == XCAFDoc_AssemblyGraph::NodeType_Part);
  //! }
  //! ~~~~~
  //! User function theFunc takes the assembly graph passed for traversing, current
  //! graph node ID and returns true/false to continue/break.
  //! \param [in] theGraph  - assembly graph.
  //! \param [in] theFilter - user filtering function called for each assembly graph node.
  //! \param [in] theFunc   - user function called for accepted assembly graph node.
  //! \param [in] theNode   - starting positive one-based graph node ID.
  template <typename Func, typename Filter>
  static void Traverse(const Handle(XCAFDoc_AssemblyGraph)& theGraph,
                       Filter                               theFilter,
                       Func                                 theFunc,
                       const Standard_Integer               theNode = 1)
  {
    Standard_NullObject_Raise_if(theGraph.IsNull(), "Null assembly graph!");

    for (XCAFDoc_AssemblyGraph::Iterator anIt(theGraph, theNode); anIt.More(); anIt.Next())
    {
      const Standard_Integer aN = anIt.Current();
      if (theFilter(theGraph, aN))
      {
        if (!theFunc(theGraph, aN))
          break;
      }
    }
  }

};

#endif // _XCAFDoc_AssemblyTool_HeaderFile
