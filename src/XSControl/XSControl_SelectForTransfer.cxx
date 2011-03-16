#include <XSControl_SelectForTransfer.ixx>

    XSControl_SelectForTransfer::XSControl_SelectForTransfer ()    {  }

    XSControl_SelectForTransfer::XSControl_SelectForTransfer
  (const Handle(XSControl_TransferReader)& TR)
      {  theTR = TR;  }


    void  XSControl_SelectForTransfer::SetReader
  (const Handle(XSControl_TransferReader)& TR)
      {  theTR = TR;  }

    void  XSControl_SelectForTransfer::SetActor
  (const Handle(Transfer_ActorOfTransientProcess)& act)
      {  theAC = act;  }

    Handle(Transfer_ActorOfTransientProcess)  XSControl_SelectForTransfer::Actor () const
      {  return theAC;  }

    Handle(XSControl_TransferReader)  XSControl_SelectForTransfer::Reader () const
      {  return theTR;  }

    Standard_Boolean  XSControl_SelectForTransfer::Sort
  (const Standard_Integer /*rank*/, const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& /*model*/) const
{
  Handle(Transfer_ActorOfTransientProcess) act = theAC;
  if (act.IsNull() && !theTR.IsNull()) act = theTR->Actor();
  if (!act.IsNull()) return act->Recognize(ent);//,theTR->TransientProcess());//act->Recognize(ent);
  return Standard_False;
}

 
    TCollection_AsciiString  XSControl_SelectForTransfer::ExtractLabel () const
{
  if (!theTR.IsNull()) return TCollection_AsciiString
    ("Recognized for Transfer (current actor)");
  return TCollection_AsciiString("Recognized for Transfer");
}
