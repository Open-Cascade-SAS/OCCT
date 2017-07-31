// Created on: 1998-08-12
// Created by: Galina KULIKOVA
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeFix_Shell_HeaderFile
#define _ShapeFix_Shell_HeaderFile

#include <Standard.hxx>

#include <Message_ProgressIndicator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Compound.hxx>
#include <ShapeFix_Root.hxx>
#include <ShapeExtend_Status.hxx>

class ShapeFix_Face;
class ShapeExtend_BasicMsgRegistrator;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ShapeFix_Shell;
DEFINE_STANDARD_HANDLE(ShapeFix_Shell, ShapeFix_Root)

//! Fixing orientation of faces in shell
class ShapeFix_Shell : public ShapeFix_Root
{

public:

  
  //! Empty constructor
  Standard_EXPORT ShapeFix_Shell();
  
  //! Initializes by shell.
  Standard_EXPORT ShapeFix_Shell(const TopoDS_Shell& shape);
  
  //! Initializes by shell.
  Standard_EXPORT void Init (const TopoDS_Shell& shell);
  
  //! Iterates on subshapes and performs fixes
  //! (for each face calls ShapeFix_Face::Perform and
  //! then calls FixFaceOrientation). The passed progress
  //! indicator allows user to consult the current progress
  //! stage and abort algorithm if needed.
  Standard_EXPORT Standard_Boolean Perform (const Handle(Message_ProgressIndicator)& theProgress = 0);
  
  //! Fixes orientation of faces in shell.
  //! Changes orientation of face in the shell, if it is oriented opposite
  //! to neigbouring faces. If it is not possible to orient all faces in the
  //! shell (like in case of mebious band), this method orients only subset
  //! of faces. Other faces are stored in Error compound.
  //! Modes :
  //! isAccountMultiConex - mode for account cases of multiconnexity.
  //! If this mode is equal to Standard_True, separate shells will be created
  //! in the cases of multiconnexity. If this mode is equal to Standard_False,
  //! one shell will be created without account of multiconnexity.By defautt - Standard_True;
  //! NonManifold - mode for creation of non-manifold shells.
  //! If this mode is equal to Standard_True one non-manifold will be created from shell
  //! contains multishared edges. Else if this mode is equal to Standard_False only
  //! manifold shells will be created. By default - Standard_False.
  Standard_EXPORT Standard_Boolean FixFaceOrientation (
      const TopoDS_Shell& shell,
      const Standard_Boolean isAccountMultiConex = Standard_True,
      const Standard_Boolean NonManifold = Standard_False);
  
  //! Returns fixed shell (or subset of oriented faces).
  Standard_EXPORT TopoDS_Shell Shell();
  
  //! In case of multiconnexity returns compound of fixed shells
  //! else returns one shell..
  Standard_EXPORT TopoDS_Shape Shape();
  
  //! Returns Number of obtainrd shells;
  Standard_EXPORT Standard_Integer NbShells() const;
  
  //! Returns not oriented subset of faces.
  Standard_EXPORT TopoDS_Compound ErrorFaces() const;
  
  //! Returns the status of the last Fix.
  Standard_EXPORT Standard_Boolean Status (const ShapeExtend_Status status) const;
  
  //! Returns tool for fixing faces.
    Handle(ShapeFix_Face) FixFaceTool();
  
  //! Sets message registrator
  Standard_EXPORT virtual void SetMsgRegistrator (const Handle(ShapeExtend_BasicMsgRegistrator)& msgreg) Standard_OVERRIDE;
  
  //! Sets basic precision value (also to FixWireTool)
  Standard_EXPORT virtual void SetPrecision (const Standard_Real preci) Standard_OVERRIDE;
  
  //! Sets minimal allowed tolerance (also to FixWireTool)
  Standard_EXPORT virtual void SetMinTolerance (const Standard_Real mintol) Standard_OVERRIDE;
  
  //! Sets maximal allowed tolerance (also to FixWireTool)
  Standard_EXPORT virtual void SetMaxTolerance (const Standard_Real maxtol) Standard_OVERRIDE;
  
  //! Returns (modifiable) the mode for applying fixes of
  //! ShapeFix_Face, by default True.
    Standard_Integer& FixFaceMode();
  
  //! Returns (modifiable) the mode for applying
  //! FixFaceOrientation, by default True.
    Standard_Integer& FixOrientationMode();

  //! Sets NonManifold flag
  Standard_EXPORT virtual void SetNonManifoldFlag(const Standard_Boolean isNonManifold);


  DEFINE_STANDARD_RTTIEXT(ShapeFix_Shell,ShapeFix_Root)

protected:


  TopoDS_Shell myShell;
  TopoDS_Compound myErrFaces;
  Standard_Integer myStatus;
  Handle(ShapeFix_Face) myFixFace;
  Standard_Integer myFixFaceMode;
  Standard_Integer myFixOrientationMode;
  Standard_Integer myNbShells;
  Standard_Boolean myNonManifold;

private:




};


#include <ShapeFix_Shell.lxx>





#endif // _ShapeFix_Shell_HeaderFile
