#include <Interface_HGraph.ixx>



    Interface_HGraph::Interface_HGraph (const Interface_Graph& agraph)
    :  thegraph(agraph)    {  }


    Interface_HGraph::Interface_HGraph
  (const Handle(Interface_InterfaceModel)& amodel,
   const Interface_GeneralLib& lib)
     :  thegraph (amodel,lib)    {  }


     Interface_HGraph::Interface_HGraph
  (const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_Protocol)& protocol)
     :  thegraph (amodel,protocol)    {  }


     Interface_HGraph::Interface_HGraph
  (const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_GTool)& gtool)
     :  thegraph (amodel,gtool)    {  }


     Interface_HGraph::Interface_HGraph
  (const Handle(Interface_InterfaceModel)& amodel)
     :  thegraph (amodel)    {  }


    const Interface_Graph&  Interface_HGraph::Graph () const
      {  return thegraph;  }


    Interface_Graph&  Interface_HGraph::CGraph ()
      {  return thegraph;  }
