// Created on: 1999-10-01
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
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

#ifndef _QANewBRepNaming_ImportShape_HeaderFile
#define _QANewBRepNaming_ImportShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <QANewBRepNaming_TopNaming.hxx>
#include <Standard_Integer.hxx>
#include <TDF_LabelMap.hxx>
class TDF_Label;
class TopoDS_Shape;
class TDF_TagSource;


//! This class provides a topological naming
//! of a Shape
class QANewBRepNaming_ImportShape  : public QANewBRepNaming_TopNaming
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT QANewBRepNaming_ImportShape();
  
  Standard_EXPORT QANewBRepNaming_ImportShape(const TDF_Label& ResultLabel);
  
  Standard_EXPORT void Init (const TDF_Label& ResultLabel);
  
  //! Use this method for a topological naming of a Shape
  Standard_EXPORT void Load (const TopoDS_Shape& S) const;
  
  Standard_EXPORT void LoadPrime (const TopoDS_Shape& S) const;
  
  Standard_EXPORT void LoadFirstLevel (const TopoDS_Shape& S, const Handle(TDF_TagSource)& Tagger) const;
  
  Standard_EXPORT void LoadNextLevels (const TopoDS_Shape& S, const Handle(TDF_TagSource)& Tagger) const;
  
  //! Method for internal use. It is used by Load().
  //! It loads the edges which couldn't be uniquely identified as
  //! an intersection of two faces.
  Standard_EXPORT void LoadC0Edges (const TopoDS_Shape& S, const Handle(TDF_TagSource)& Tagger) const;
  
  Standard_EXPORT void LoadC0Vertices (const TopoDS_Shape& S, const Handle(TDF_TagSource)& Tagger) const;
  
  Standard_EXPORT Standard_Integer NamedFaces (TDF_LabelMap& theNamedFaces) const;
  
  Standard_EXPORT Standard_Integer NamedEdges (TDF_LabelMap& theNamedEdges) const;
  
  Standard_EXPORT Standard_Integer NamedVertices (TDF_LabelMap& theNamedVertices) const;




protected:





private:





};







#endif // _QANewBRepNaming_ImportShape_HeaderFile
