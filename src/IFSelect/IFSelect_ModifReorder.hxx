// Created on: 1996-03-15
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _IFSelect_ModifReorder_HeaderFile
#define _IFSelect_ModifReorder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Modifier.hxx>
class IFSelect_ContextModif;
class Interface_InterfaceModel;
class Interface_Protocol;
class Interface_CopyTool;
class TCollection_AsciiString;


class IFSelect_ModifReorder;
DEFINE_STANDARD_HANDLE(IFSelect_ModifReorder, IFSelect_Modifier)

//! This modifier reorders a whole model from its roots, i.e.
//! according to <rootlast> status, it considers each of its
//! roots, then it orders all its shared entities at any level,
//! the result begins by the lower level entities ... ends by
//! the roots.
class IFSelect_ModifReorder : public IFSelect_Modifier
{

public:

  
  //! Creates a ModifReorder. It may change the graph (it does !)
  //! If <rootlast> is True (D), roots are set at the end of packets
  //! Else, they are set at beginning (as done by AddWithRefs)
  Standard_EXPORT IFSelect_ModifReorder(const Standard_Boolean rootlast = Standard_True);
  
  //! Acts by computing orders (by method All from ShareTool) then
  //! forcing them in the model. Remark that selection is ignored :
  //! ALL the model is processed in once
  Standard_EXPORT void Perform (IFSelect_ContextModif& ctx, const Handle(Interface_InterfaceModel)& target, const Handle(Interface_Protocol)& protocol, Interface_CopyTool& TC) const Standard_OVERRIDE;
  
  //! Returns Label as "Reorder, Roots (last or first)"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_ModifReorder,IFSelect_Modifier)

protected:




private:


  Standard_Boolean thertl;


};







#endif // _IFSelect_ModifReorder_HeaderFile
