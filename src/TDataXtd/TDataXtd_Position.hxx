// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _TDataXtd_Position_HeaderFile
#define _TDataXtd_Position_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <TDF_Attribute.hxx>
class TDF_Label;
class Standard_GUID;
class TDF_RelocationTable;


class TDataXtd_Position;
DEFINE_STANDARD_HANDLE(TDataXtd_Position, TDF_Attribute)

//! Position of a Label
class TDataXtd_Position : public TDF_Attribute
{

public:

  
  //! Create if not found the TDataXtd_Position attribute set its position to <aPos>
  Standard_EXPORT static void Set (const TDF_Label& aLabel, const gp_Pnt& aPos);
  
  //! Find an existing, or create an empty, Position.
  //! the Position attribute is returned.
  Standard_EXPORT static Handle(TDataXtd_Position) Set (const TDF_Label& aLabel);
  
  //! Search label <aLabel) for the TDataXtd_Position attribute and get its position
  //! if found returns True
  Standard_EXPORT static Standard_Boolean Get (const TDF_Label& aLabel, gp_Pnt& aPos);
  
  Standard_EXPORT TDataXtd_Position();
  
  //! Returns the ID of the attribute.
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  //! Returns the ID of the attribute.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Restores the contents from <anAttribute> into this
  //! one. It is used when aborting a transaction.
  Standard_EXPORT virtual void Restore (const Handle(TDF_Attribute)& anAttribute) Standard_OVERRIDE;
  
  //! Returns an new empty attribute from the good end
  //! type. It is used by the copy algorithm.
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  //! This method is different from the "Copy" one,
  //! because it is used when copying an attribute from
  //! a source structure into a target structure. This
  //! method pastes the current attribute to the label
  //! corresponding to the insertor. The pasted
  //! attribute may be a brand new one or a new version
  //! of the previous one.
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& intoAttribute, const Handle(TDF_RelocationTable)& aRelocTationable) const Standard_OVERRIDE;
  
  Standard_EXPORT const gp_Pnt& GetPosition() const;
  
  Standard_EXPORT void SetPosition (const gp_Pnt& aPos);




  DEFINE_STANDARD_RTTIEXT(TDataXtd_Position,TDF_Attribute)

protected:




private:


  gp_Pnt myPosition;


};







#endif // _TDataXtd_Position_HeaderFile
