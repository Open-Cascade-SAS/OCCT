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

#include <TopoDS_Shell.hxx>
#include <TopoDS_Compound.hxx>
#include <ShapeFix_Root.hxx>
#include <ShapeExtend_Status.hxx>
#include <Message_ProgressRange.hxx>

class ShapeFix_Face;
class ShapeExtend_BasicMsgRegistrator;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Fixing orientation of faces in shell
class ShapeFix_Shell : public ShapeFix_Root
{

public:
  //! Empty constructor
  Standard_EXPORT ShapeFix_Shell();

  //! Initializes by shell.
  Standard_EXPORT ShapeFix_Shell(const TopoDS_Shell& shape);

  //! Initializes by shell.
  Standard_EXPORT void Init(const TopoDS_Shell& shell);

  //! Iterates on subshapes and performs fixes
  //! (for each face calls ShapeFix_Face::Perform and
  //! then calls FixFaceOrientation). The passed progress
  //! indicator allows user to consult the current progress
  //! stage and abort algorithm if needed.
  Standard_EXPORT bool
    Perform(const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Fixes orientation of faces in shell.
  //! Changes orientation of face in the shell, if it is oriented opposite
  //! to neighbouring faces. If it is not possible to orient all faces in the
  //! shell (like in case of mebious band), this method orients only subset
  //! of faces. Other faces are stored in Error compound.
  //! Modes :
  //! isAccountMultiConex - mode for account cases of multiconnexity.
  //! If this mode is equal to true, separate shells will be created
  //! in the cases of multiconnexity. If this mode is equal to false,
  //! one shell will be created without account of multiconnexity.By default - true;
  //! NonManifold - mode for creation of non-manifold shells.
  //! If this mode is equal to true one non-manifold will be created from shell
  //! contains multishared edges. Else if this mode is equal to false only
  //! manifold shells will be created. By default - false.
  Standard_EXPORT bool
    FixFaceOrientation(const TopoDS_Shell&    shell,
                       const bool isAccountMultiConex = true,
                       const bool NonManifold         = false);

  //! Returns fixed shell (or subset of oriented faces).
  Standard_EXPORT TopoDS_Shell Shell();

  //! In case of multiconnexity returns compound of fixed shells
  //! else returns one shell..
  Standard_EXPORT TopoDS_Shape Shape();

  //! Returns Number of obtainrd shells;
  Standard_EXPORT int NbShells() const;

  //! Returns not oriented subset of faces.
  Standard_EXPORT TopoDS_Compound ErrorFaces() const;

  //! Returns the status of the last Fix.
  Standard_EXPORT bool Status(const ShapeExtend_Status status) const;

  //! Returns tool for fixing faces.
  occ::handle<ShapeFix_Face> FixFaceTool();

  //! Sets message registrator
  Standard_EXPORT virtual void SetMsgRegistrator(
    const occ::handle<ShapeExtend_BasicMsgRegistrator>& msgreg) override;

  //! Sets basic precision value (also to FixWireTool)
  Standard_EXPORT virtual void SetPrecision(const double preci) override;

  //! Sets minimal allowed tolerance (also to FixWireTool)
  Standard_EXPORT virtual void SetMinTolerance(const double mintol) override;

  //! Sets maximal allowed tolerance (also to FixWireTool)
  Standard_EXPORT virtual void SetMaxTolerance(const double maxtol) override;

  //! Returns (modifiable) the mode for applying fixes of
  //! ShapeFix_Face, by default True.
  int& FixFaceMode();

  //! Returns (modifiable) the mode for applying
  //! FixFaceOrientation, by default True.
  int& FixOrientationMode();

  //! Sets NonManifold flag
  Standard_EXPORT virtual void SetNonManifoldFlag(const bool isNonManifold);

  DEFINE_STANDARD_RTTIEXT(ShapeFix_Shell, ShapeFix_Root)

protected:
  TopoDS_Shell          myShell;
  TopoDS_Compound       myErrFaces;
  int      myStatus;
  occ::handle<ShapeFix_Face> myFixFace;
  int      myFixFaceMode;
  int      myFixOrientationMode;
  int      myNbShells;
  bool      myNonManifold;

};

#include <ShapeFix_Shell.lxx>

#endif // _ShapeFix_Shell_HeaderFile
