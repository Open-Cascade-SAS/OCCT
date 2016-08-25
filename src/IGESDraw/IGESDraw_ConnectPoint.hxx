// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Niraj RANGWALA )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESDraw_ConnectPoint_HeaderFile
#define _IGESDraw_ConnectPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
class IGESData_IGESEntity;
class TCollection_HAsciiString;
class IGESGraph_TextDisplayTemplate;
class gp_XYZ;
class gp_Pnt;


class IGESDraw_ConnectPoint;
DEFINE_STANDARD_HANDLE(IGESDraw_ConnectPoint, IGESData_IGESEntity)

//! defines IGESConnectPoint, Type <132> Form Number <0>
//! in package IGESDraw
//!
//! Connect Point Entity describes a point of connection for
//! zero, one or more entities. Its referenced from Composite
//! curve, or Network Subfigure Definition/Instance, or Flow
//! Associative Instance, or it may stand alone.
class IGESDraw_ConnectPoint : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDraw_ConnectPoint();
  
  //! This method is used to set the fields of the class
  //! ConnectPoint
  //! - aPoint               : A Coordinate point
  //! - aDisplaySymbol       : Display symbol Geometry
  //! - aTypeFlag            : Type of the connection
  //! - aFunctionFlag        : Function flag for the connection
  //! - aFunctionIdentifier  : Connection Point Function Identifier
  //! - anIdentifierTemplate : Connection Point Function Template
  //! - aFunctionName        : Connection Point Function Name
  //! - aFunctionTemplate    : Connection Point Function Template
  //! - aPointIdentifier     : Unique Connect Point Identifier
  //! - aFunctionCode        : Connect Point Function Code
  //! - aSwapFlag            : Connect Point Swap Flag
  //! - anOwnerSubfigure     : Pointer to the "Owner" Entity
  Standard_EXPORT void Init (const gp_XYZ& aPoint, const Handle(IGESData_IGESEntity)& aDisplaySymbol, const Standard_Integer aTypeFlag, const Standard_Integer aFunctionFlag, const Handle(TCollection_HAsciiString)& aFunctionIdentifier, const Handle(IGESGraph_TextDisplayTemplate)& anIdentifierTemplate, const Handle(TCollection_HAsciiString)& aFunctionName, const Handle(IGESGraph_TextDisplayTemplate)& aFunctionTemplate, const Standard_Integer aPointIdentifier, const Standard_Integer aFunctionCode, const Standard_Integer aSwapFlag, const Handle(IGESData_IGESEntity)& anOwnerSubfigure);
  
  //! returns the coordinate of the connection point
  Standard_EXPORT gp_Pnt Point() const;
  
  //! returns the Transformed coordinate of the connection point
  Standard_EXPORT gp_Pnt TransformedPoint() const;
  
  //! returns True if Display symbol is specified
  //! else returns False
  Standard_EXPORT Standard_Boolean HasDisplaySymbol() const;
  
  //! if display symbol specified returns display symbol geometric entity
  //! else returns NULL Handle
  Standard_EXPORT Handle(IGESData_IGESEntity) DisplaySymbol() const;
  
  //! return value specifies a particular type of connection :
  //! Type Flag = 0   : Not Specified(default)
  //! 1   : Nonspecific logical  point of connection
  //! 2   : Nonspecific physical point of connection
  //! 101 : Logical component pin
  //! 102 : Logical part connector
  //! 103 : Logical offpage connector
  //! 104 : Logical global signal connector
  //! 201 : Physical PWA surface mount pin
  //! 202 : Physical PWA blind pin
  //! 203 : Physical PWA thru-pin
  //! 5001-9999 : Implementor defined.
  Standard_EXPORT Standard_Integer TypeFlag() const;
  
  //! returns Function Code that specifies a particular function for the
  //! ECO576 connection :
  //! e.g.,        Function Flag = 0 : Unspecified(default)
  //! = 1 : Electrical Signal
  //! = 2 : Fluid flow Signal
  Standard_EXPORT Standard_Integer FunctionFlag() const;
  
  //! return HAsciiString identifying Pin Number or Nozzle Label etc.
  Standard_EXPORT Handle(TCollection_HAsciiString) FunctionIdentifier() const;
  
  //! returns True if Text Display Template is specified for Identifier
  //! else returns False
  Standard_EXPORT Standard_Boolean HasIdentifierTemplate() const;
  
  //! if Text Display Template for the Function Identifier is defined,
  //! returns TestDisplayTemplate
  //! else returns NULL Handle
  Standard_EXPORT Handle(IGESGraph_TextDisplayTemplate) IdentifierTemplate() const;
  
  //! returns Connection Point Function Name
  Standard_EXPORT Handle(TCollection_HAsciiString) FunctionName() const;
  
  //! returns True if Text Display Template is specified for Function Name
  //! else returns False
  Standard_EXPORT Standard_Boolean HasFunctionTemplate() const;
  
  //! if Text Display Template for the Function Name is defined,
  //! returns TestDisplayTemplate
  //! else returns NULL Handle
  Standard_EXPORT Handle(IGESGraph_TextDisplayTemplate) FunctionTemplate() const;
  
  //! returns the Unique Connect Point Identifier
  Standard_EXPORT Standard_Integer PointIdentifier() const;
  
  //! returns the Connect Point Function Code
  Standard_EXPORT Standard_Integer FunctionCode() const;
  
  //! return value = 0 : Connect point may be swapped(default)
  //! = 1 : Connect point may not be swapped
  Standard_EXPORT Standard_Boolean SwapFlag() const;
  
  //! returns True if Network Subfigure Instance/Definition Entity
  //! is specified
  //! else returns False
  Standard_EXPORT Standard_Boolean HasOwnerSubfigure() const;
  
  //! returns "owner" Network Subfigure Instance Entity,
  //! or Network Subfigure Definition Entity, or NULL Handle.
  Standard_EXPORT Handle(IGESData_IGESEntity) OwnerSubfigure() const;




  DEFINE_STANDARD_RTTIEXT(IGESDraw_ConnectPoint,IGESData_IGESEntity)

protected:




private:


  gp_XYZ thePoint;
  Handle(IGESData_IGESEntity) theDisplaySymbol;
  Standard_Integer theTypeFlag;
  Standard_Integer theFunctionFlag;
  Handle(TCollection_HAsciiString) theFunctionIdentifier;
  Handle(IGESGraph_TextDisplayTemplate) theIdentifierTemplate;
  Handle(TCollection_HAsciiString) theFunctionName;
  Handle(IGESGraph_TextDisplayTemplate) theFunctionTemplate;
  Standard_Integer thePointIdentifier;
  Standard_Integer theFunctionCode;
  Standard_Boolean theSwapFlag;
  Handle(IGESData_IGESEntity) theOwnerSubfigure;


};







#endif // _IGESDraw_ConnectPoint_HeaderFile
