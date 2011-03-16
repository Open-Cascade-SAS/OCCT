#include <Transfer_DispatchControl.ixx>



    Transfer_DispatchControl::Transfer_DispatchControl
  (const Handle(Interface_InterfaceModel)& model,
   const Handle(Transfer_TransientProcess)& TP)
      { themodel = model;  theTP = TP;  }


    const Handle(Transfer_TransientProcess)&
  Transfer_DispatchControl::TransientProcess () const
      { return theTP;  }

    const Handle(Interface_InterfaceModel)&
  Transfer_DispatchControl::StartingModel () const
      { return themodel; }

    void Transfer_DispatchControl::Clear ()  { theTP->Clear();  }


    void Transfer_DispatchControl::Bind
  (const Handle(Standard_Transient)& ent,
   const Handle(Standard_Transient)& res)
      {  theTP->BindTransient(ent,res);  }


    Standard_Boolean  Transfer_DispatchControl::Search
  (const Handle(Standard_Transient)& ent,
   Handle(Standard_Transient)&res) const
      {  res = theTP->FindTransient(ent);  return !res.IsNull();  }
