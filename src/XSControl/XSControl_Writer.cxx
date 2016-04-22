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

//:i1 gka 03.04.99 BUC60301 

#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <TopoDS_Shape.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferWriter.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSControl_Writer.hxx>

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
  thesession->TransferWriter()->SetTransferMode (mode);
  return thesession->TransferWriteShape (sh);
}

    IFSelect_ReturnStatus  XSControl_Writer::WriteFile
  (const Standard_CString filename)
      {  return thesession->SendAll(filename);  }

    void  XSControl_Writer::PrintStatsTransfer
  (const Standard_Integer what, const Standard_Integer mode) const
      {  thesession->TransferWriter()->PrintStats (what,mode);  }
