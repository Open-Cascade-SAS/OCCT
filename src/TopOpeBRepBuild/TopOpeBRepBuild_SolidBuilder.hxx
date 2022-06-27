// Created on: 1995-12-21
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _TopOpeBRepBuild_SolidBuilder_HeaderFile
#define _TopOpeBRepBuild_SolidBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopOpeBRepBuild_LoopSet.hxx>
#include <TopOpeBRepBuild_BlockIterator.hxx>
#include <TopOpeBRepBuild_BlockBuilder.hxx>
#include <TopOpeBRepBuild_SolidAreaBuilder.hxx>
#include <Standard_Integer.hxx>
class TopOpeBRepBuild_ShellFaceSet;
class TopoDS_Shape;
class TopOpeBRepBuild_ShapeSet;



class TopOpeBRepBuild_SolidBuilder 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_SolidBuilder();
  
  //! Create a SolidBuilder to build the areas on
  //! the shapes (shells, blocks of faces) described by <LS>.
  Standard_EXPORT TopOpeBRepBuild_SolidBuilder(TopOpeBRepBuild_ShellFaceSet& FS, const Standard_Boolean ForceClass = Standard_False);
  
  Standard_EXPORT void InitSolidBuilder (TopOpeBRepBuild_ShellFaceSet& FS, const Standard_Boolean ForceClass);
  
  Standard_EXPORT Standard_Integer InitSolid();
  
  Standard_EXPORT Standard_Boolean MoreSolid() const;
  
  Standard_EXPORT void NextSolid();
  
  Standard_EXPORT Standard_Integer InitShell();
  
  Standard_EXPORT Standard_Boolean MoreShell() const;
  
  Standard_EXPORT void NextShell();
  
  Standard_EXPORT Standard_Boolean IsOldShell() const;
  
  //! Returns current shell
  //! This shell may be :
  //! * an old shell OldShell(), which has not been reconstructed;
  //! * a new shell made of faces described by ...NewFace() methods.
  Standard_EXPORT const TopoDS_Shape& OldShell() const;
  
  Standard_EXPORT Standard_Integer InitFace();
  
  Standard_EXPORT Standard_Boolean MoreFace() const;
  
  Standard_EXPORT void NextFace();
  
  //! Returns current new face of current new shell.
  Standard_EXPORT const TopoDS_Shape& Face() const;




protected:





private:

  
  Standard_EXPORT void MakeLoops (TopOpeBRepBuild_ShapeSet& SS);


  TopOpeBRepBuild_LoopSet myLoopSet;
  TopOpeBRepBuild_BlockIterator myBlockIterator;
  TopOpeBRepBuild_BlockBuilder myBlockBuilder;
  TopOpeBRepBuild_SolidAreaBuilder mySolidAreaBuilder;


};







#endif // _TopOpeBRepBuild_SolidBuilder_HeaderFile
