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
