// Created on: 2000-08-22
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _ShapeProcess_ShapeContext_HeaderFile
#define _ShapeProcess_ShapeContext_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <ShapeProcess_Context.hxx>
#include <Message_Gravity.hxx>
#include <GeomAbs_Shape.hxx>
class ShapeExtend_MsgRegistrator;
class ShapeBuild_ReShape;
class BRepTools_Modifier;
class Message_Msg;


class ShapeProcess_ShapeContext;
DEFINE_STANDARD_HANDLE(ShapeProcess_ShapeContext, ShapeProcess_Context)

//! Extends Context to handle shapes
//! Contains map of shape-shape, and messages
//! attached to shapes
class ShapeProcess_ShapeContext : public ShapeProcess_Context
{

public:

  
  Standard_EXPORT ShapeProcess_ShapeContext(const Standard_CString file, const Standard_CString seq = "");
  
  //! Initializes a tool by resource file and shape
  //! to be processed
  Standard_EXPORT ShapeProcess_ShapeContext(const TopoDS_Shape& S, const Standard_CString file, const Standard_CString seq = "");
  
  //! Initializes tool by a new shape and clears all results
  Standard_EXPORT void Init (const TopoDS_Shape& S);
  
  //! Returns shape being processed
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  //! Returns current result
  Standard_EXPORT const TopoDS_Shape& Result() const;
  
  //! Returns map of replacements shape -> shape
  //! This map is not recursive
  Standard_EXPORT const TopTools_DataMapOfShapeShape& Map() const;
  
  Standard_EXPORT const Handle(ShapeExtend_MsgRegistrator)& Messages() const;
  
  //! Returns messages recorded during shape processing
  //! It can be nullified before processing in order to
  //! avoid recording messages
  Standard_EXPORT Handle(ShapeExtend_MsgRegistrator)& Messages();
  
  Standard_EXPORT void SetDetalisation (const TopAbs_ShapeEnum level);
  
  //! Set and get value for detalisation level
  //! Only shapes of types from TopoDS_COMPOUND and until
  //! specified detalisation level will be recorded in maps
  //! To cancel mapping, use TopAbs_SHAPE
  //! To force full mapping, use TopAbs_VERTEX
  //! The default level is TopAbs_FACE
  Standard_EXPORT TopAbs_ShapeEnum GetDetalisation() const;
  
  //! Sets a new result shape
  //! NOTE: this method should be used very carefully
  //! to keep consistency of modifications
  //! It is recommended to use RecordModification() methods
  //! with explicit definition of mapping from current
  //! result to a new one
  Standard_EXPORT void SetResult (const TopoDS_Shape& S);
  
  Standard_EXPORT void RecordModification (const TopTools_DataMapOfShapeShape& repl, const Handle(ShapeExtend_MsgRegistrator)& msg = 0);
  
  Standard_EXPORT void RecordModification (const Handle(ShapeBuild_ReShape)& repl, const Handle(ShapeExtend_MsgRegistrator)& msg);
  
  Standard_EXPORT void RecordModification (const Handle(ShapeBuild_ReShape)& repl);
  
  //! Records modifications and resets result accordingly
  //! NOTE: modification of resulting shape should be explicitly
  //! defined in the maps along with modifications of subshapes
  //!
  //! In the last function, sh is the shape on which Modifier
  //! was run. It can be different from the whole shape,
  //! but in that case result as a whole should be reset later
  //! either by call to SetResult(), or by another call to
  //! RecordModification() which contains mapping of current
  //! result to a new one explicitly
  Standard_EXPORT void RecordModification (const TopoDS_Shape& sh, const BRepTools_Modifier& repl, const Handle(ShapeExtend_MsgRegistrator)& msg = 0);
  
  //! Record a message for shape S
  //! Shape S should be one of subshapes of original shape
  //! (or whole one), but not one of intermediate shapes
  //! Records only if Message() is not Null
  Standard_EXPORT void AddMessage (const TopoDS_Shape& S, const Message_Msg& msg, const Message_Gravity gravity = Message_Warning);
  
  //! Get value of parameter as being of the type GeomAbs_Shape
  //! Returns False if parameter is not defined or has a wrong type
  Standard_EXPORT Standard_Boolean GetContinuity (const Standard_CString param, GeomAbs_Shape& val) const;
  
  //! Get value of parameter as being of the type GeomAbs_Shape
  //! If parameter is not defined or does not have expected
  //! type, returns default value as specified
  Standard_EXPORT GeomAbs_Shape ContinuityVal (const Standard_CString param, const GeomAbs_Shape def) const;
  
  //! Prints statistics on Shape Processing onto the current Messenger.
  Standard_EXPORT void PrintStatistics() const;

  //! Set NonManifold flag
  void SetNonManifold(Standard_Boolean theNonManifold)
  {
      myNonManifold = theNonManifold;
  }

  //! Get NonManifold flag
  Standard_Boolean IsNonManifold()
  {
      return myNonManifold;
  }


  DEFINE_STANDARD_RTTIEXT(ShapeProcess_ShapeContext,ShapeProcess_Context)

protected:




private:


  TopoDS_Shape myShape;
  TopoDS_Shape myResult;
  TopTools_DataMapOfShapeShape myMap;
  Handle(ShapeExtend_MsgRegistrator) myMsg;
  TopAbs_ShapeEnum myUntil;
  Standard_Boolean myNonManifold;

};







#endif // _ShapeProcess_ShapeContext_HeaderFile
