#include <XSControl.ixx>

    Handle(XSControl_WorkSession)  XSControl::Session
  (const Handle(IFSelect_SessionPilot)& pilot)
      {  return Handle(XSControl_WorkSession)::DownCast(pilot->Session());  }


    Handle(XSControl_Vars)  XSControl::Vars
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(XSControl_Vars) avars;
  Handle(XSControl_WorkSession) WS = XSControl::Session(pilot);
  if (!WS.IsNull()) avars = WS->Vars();
  return avars;
}
