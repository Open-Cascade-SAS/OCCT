// Created on: 1994-05-31
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#include <IGESSelect_ModelModifier.hxx>

#include <Standard_Type.hxx>

#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IFSelect_ContextModif.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_Check.hxx>

IGESSelect_ModelModifier::IGESSelect_ModelModifier(const bool grf)
    : IFSelect_Modifier(grf)
{
}

void IGESSelect_ModelModifier::Perform(IFSelect_ContextModif&                  ctx,
                                       const occ::handle<Interface_InterfaceModel>& target,
                                       const occ::handle<Interface_Protocol>&       protocol,
                                       Interface_CopyTool&                     TC) const
{
  ctx.TraceModifier(this);
  occ::handle<IGESData_IGESModel> targ = occ::down_cast<IGESData_IGESModel>(target);
  occ::handle<IGESData_Protocol>  prot = occ::down_cast<IGESData_Protocol>(protocol);
  if (targ.IsNull())
  {
    ctx.CCheck()->AddFail("Model to Modify : unproper type");
    return;
  }
  PerformProtocol(ctx, targ, prot, TC);
}

void IGESSelect_ModelModifier::PerformProtocol(IFSelect_ContextModif&            ctx,
                                               const occ::handle<IGESData_IGESModel>& target,
                                               const occ::handle<IGESData_Protocol>&  protocol,
                                               Interface_CopyTool&               TC) const
{
  ctx.SetProtocol(protocol);
  Performing(ctx, target, TC);
}
