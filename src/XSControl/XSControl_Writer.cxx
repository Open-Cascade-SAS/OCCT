// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//:i1 gka 03.04.99 BUC60301 

#include <XSControl_Writer.ixx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferWriter.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>


XSControl_Writer::XSControl_Writer ()
{
  SetWS (new XSControl_WorkSession);
}

    XSControl_Writer::XSControl_Writer (const Standard_CString norm)
{
  SetNorm (norm);
}

    XSControl_Writer::XSControl_Writer
  (const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch)
{
  SetWS (WS,scratch);
}

    Standard_Boolean  XSControl_Writer::SetNorm (const Standard_CString norm)
{
  if (thesession.IsNull()) SetWS (new XSControl_WorkSession);
  Standard_Boolean sess =  thesession->SelectNorm (norm);
  Handle(Interface_InterfaceModel) model = Model ();  //:i1 gka 03.04.99 BUC60301 
  return sess;
}

    void  XSControl_Writer::SetWS
  (const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch)
{
  thesession = WS;
//  Un controller doit etre defini ...
  thesession->InitTransferReader(0);
  Handle(Interface_InterfaceModel) model = Model (scratch);
}

    Handle(XSControl_WorkSession)  XSControl_Writer::WS () const
      {  return thesession;  }

     Handle(Interface_InterfaceModel)  XSControl_Writer::Model
  (const Standard_Boolean newone)
{
  Handle(Interface_InterfaceModel) model = thesession->Model();
  if (newone || model.IsNull()) model = thesession->NewModel();
  return model;
}

    IFSelect_ReturnStatus  XSControl_Writer::TransferShape
  (const TopoDS_Shape& sh, const Standard_Integer mode)
{
  thesession->SetModeWriteShape (mode);
  return thesession->TransferWriteShape (sh);
}

    IFSelect_ReturnStatus  XSControl_Writer::WriteFile
  (const Standard_CString filename)
      {  return thesession->SendAll(filename);  }

    void  XSControl_Writer::PrintStatsTransfer
  (const Standard_Integer what, const Standard_Integer mode) const
      {  thesession->TransferWriter()->PrintStats (what,mode);  }
