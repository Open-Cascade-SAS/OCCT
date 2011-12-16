#include <Interface_HGraph.ixx>


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
