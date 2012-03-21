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

#include <Transfer_FinderProcess.ixx>
#include <Interface_MSG.hxx>
#include <Transfer_Binder.hxx>
#include <Interface_Check.hxx>


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
