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


#include <Interface_Check.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_MSG.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_TransientMapper.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_FinderProcess,Transfer_ProcessForFinder)

Transfer_FinderProcess::Transfer_FinderProcess (const Standard_Integer nb)
    : Transfer_ProcessForFinder (nb)    {  }
 
    void Transfer_FinderProcess::SetModel
  (const Handle(Interface_InterfaceModel)& model)
      {  themodel = model;  }
 
    Handle(Interface_InterfaceModel)  Transfer_FinderProcess::Model () const
      {  return themodel;  }
 

    Standard_Integer  Transfer_FinderProcess::NextMappedWithAttribute
  (const Standard_CString name, const Standard_Integer num0) const
{
  Standard_Integer num, nb = NbMapped();
  for (num = num0+1; num <= nb; num ++) {
    Handle(Transfer_Finder) fnd = Mapped (num);
    if (fnd.IsNull()) continue;
    if (!fnd->Attribute(name).IsNull()) return num;
  }
  return 0;
}

    Handle(Transfer_TransientMapper)  Transfer_FinderProcess::TransientMapper
  (const Handle(Standard_Transient)& obj) const
{
  Handle(Transfer_TransientMapper) mapper = new Transfer_TransientMapper(obj);
  Standard_Integer index = MapIndex (mapper);
  if (index == 0) return mapper;
  return Handle(Transfer_TransientMapper)::DownCast(Mapped(index));
}


    void  Transfer_FinderProcess::PrintTrace
  (const Handle(Transfer_Finder)& start, const Handle(Message_Messenger)& S) const
      {  if (!start.IsNull()) S<<" Type:"<<start->ValueTypeName();  }

    void  Transfer_FinderProcess::PrintStats
  (const Standard_Integer mode, const Handle(Message_Messenger)& S) const
{
  S<<"\n*******************************************************************\n";
  if (mode == 1) {    //  Statistiques de base
    S << "********                 Basic Statistics                  ********"<<endl;

    Standard_Integer nbr = 0, nbe = 0, nbw = 0;
    Standard_Integer i, max = NbMapped(), nbroots = NbRoots();
    S << "****        Nb Final Results    : "<<nbroots<<endl;

    for (i = 1; i <= max; i ++) {
      const Handle(Transfer_Binder)& binder = MapItem(i);
      if (binder.IsNull()) continue;
      const Handle(Interface_Check) ach = binder->Check();
      Transfer_StatusExec stat = binder->StatusExec();
      if (stat != Transfer_StatusInitial && stat != Transfer_StatusDone)
	nbe ++;
      else {
	if (ach->NbWarnings() > 0) nbw  ++;
	if (binder->HasResult())  nbr ++;
      }
    }
    if (nbr > nbroots)
      S<<"****      ( Itermediate Results : "<<nbr-nbroots<<" )\n";
    if (nbe > 0)
      S<<"****                  Errors on :"<<Interface_MSG::Blanks(nbe,4)<<nbe<<" Entities\n";
    if (nbw > 0)
      S<<"****                Warnings on : "<<Interface_MSG::Blanks(nbw,4)<<nbw<<" Entities\n";
    S<<"*******************************************************************";
  }
  S<<endl;
}
