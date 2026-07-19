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

#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Transfer_DispatchControl.hxx>
#include <Transfer_TransientProcess.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_DispatchControl, Interface_CopyControl)

Transfer_DispatchControl::Transfer_DispatchControl(
  const occ::handle<Interface_InterfaceModel>&  model,
  const occ::handle<Transfer_TransientProcess>& TP)
{
  themodel = model;
  theTP    = TP;
}

const occ::handle<Transfer_TransientProcess>& Transfer_DispatchControl::TransientProcess() const
{
  return theTP;
}

const occ::handle<Interface_InterfaceModel>& Transfer_DispatchControl::StartingModel() const
{
  return themodel;
}

void Transfer_DispatchControl::Clear()
{
  theTP->Clear();
}

void Transfer_DispatchControl::Bind(const occ::handle<Standard_Transient>& ent,
                                    const occ::handle<Standard_Transient>& res)
{
  theTP->BindTransient(ent, res);
}

bool Transfer_DispatchControl::Search(const occ::handle<Standard_Transient>& ent,
                                      occ::handle<Standard_Transient>&       res) const
{
  res = theTP->FindTransient(ent);
  return !res.IsNull();
}
