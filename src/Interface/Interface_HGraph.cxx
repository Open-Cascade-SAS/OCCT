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


#include <Interface_GeneralLib.hxx>
#include <Interface_Graph.hxx>
#include <Interface_GTool.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_HGraph,Standard_Transient)

Interface_HGraph::Interface_HGraph (const Interface_Graph& agraph)
    :  thegraph(agraph)    {  }
   

    Interface_HGraph::Interface_HGraph
  (const Handle(Interface_InterfaceModel)& amodel,
   const Interface_GeneralLib& lib,
   const Standard_Boolean theModeStat)
     :  thegraph (amodel,lib,theModeStat)    {  }


     Interface_HGraph::Interface_HGraph
  (const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_Protocol)& protocol,
   const Standard_Boolean theModeStat)
     :  thegraph (amodel,protocol,theModeStat)    {  }


     Interface_HGraph::Interface_HGraph
  (const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_GTool)& gtool,
   const Standard_Boolean theModeStat)
     :  thegraph (amodel,gtool,theModeStat)    {  }


     Interface_HGraph::Interface_HGraph
  (const Handle(Interface_InterfaceModel)& amodel,
    const Standard_Boolean theModeStat)
     :  thegraph (amodel,theModeStat)    {  }


    const Interface_Graph&  Interface_HGraph::Graph () const
      {  return thegraph;  }


    Interface_Graph&  Interface_HGraph::CGraph ()
      {  return thegraph;  }
