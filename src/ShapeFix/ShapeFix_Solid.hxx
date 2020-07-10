// Created on: 1998-06-03
// Created by: data exchange team
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

#ifndef _ShapeFix_Solid_HeaderFile
#define _ShapeFix_Solid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <ShapeFix_Root.hxx>
#include <ShapeExtend_Status.hxx>
#include <Standard_Real.hxx>
class ShapeFix_Shell;
class TopoDS_Solid;
class Message_ProgressScope;
class TopoDS_Shell;
class ShapeExtend_BasicMsgRegistrator;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ShapeFix_Solid;
DEFINE_STANDARD_HANDLE(ShapeFix_Solid, ShapeFix_Root)

//! Provides method to build a solid from a shells and
//! orients them in order to have a valid solid with finite volume
class ShapeFix_Solid : public ShapeFix_Root
{

public:

  
  //! Empty constructor;
  Standard_EXPORT ShapeFix_Solid();
  
  //! Initializes by solid.
  Standard_EXPORT ShapeFix_Solid(const TopoDS_Solid& solid);
  
  //! Initializes by solid .
  Standard_EXPORT virtual void Init (const TopoDS_Solid& solid);
  
  //! Iterates on shells and performs fixes
  //! (calls ShapeFix_Shell for each subshell). The passed
  //! progress indicator allows user to consult the current
  //! progress stage and abort algorithm if needed.
  Standard_EXPORT virtual Standard_Boolean Perform (const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Calls MakeSolid and orients the solid to be "not infinite"
  Standard_EXPORT TopoDS_Solid SolidFromShell (const TopoDS_Shell& shell);
  
  //! Returns the status of the last Fix.
  Standard_EXPORT Standard_Boolean Status (const ShapeExtend_Status status) const;
  
  //! Returns resulting solid.
  Standard_EXPORT TopoDS_Shape Solid() const;
  
  //! Returns tool for fixing shells.
  Handle(ShapeFix_Shell) FixShellTool() const
  {
    return myFixShell;
  }
  
  //! Sets message registrator
  Standard_EXPORT virtual void SetMsgRegistrator (const Handle(ShapeExtend_BasicMsgRegistrator)& msgreg) Standard_OVERRIDE;
  
  //! Sets basic precision value (also to FixShellTool)
  Standard_EXPORT virtual void SetPrecision (const Standard_Real preci) Standard_OVERRIDE;
  
  //! Sets minimal allowed tolerance (also to FixShellTool)
  Standard_EXPORT virtual void SetMinTolerance (const Standard_Real mintol) Standard_OVERRIDE;
  
  //! Sets maximal allowed tolerance (also to FixShellTool)
  Standard_EXPORT virtual void SetMaxTolerance (const Standard_Real maxtol) Standard_OVERRIDE;
  
  //! Returns (modifiable) the mode for applying fixes of
  //! ShapeFix_Shell, by default True.
  Standard_Integer& FixShellMode()
  {
    return myFixShellMode;
  }
  
  //! Returns (modifiable) the mode for applying analysis and fixes of
  //! orientation of shells in the solid; by default True.
  Standard_Integer& FixShellOrientationMode()
  {
    return myFixShellOrientationMode;
  }
  
  //! Returns (modifiable) the mode for creation of solids.
  //! If mode myCreateOpenSolidMode is equal to true
  //! solids are created from open shells
  //! else solids are created  from closed shells only.
  //! ShapeFix_Shell, by default False.
  Standard_Boolean& CreateOpenSolidMode()
  {
    return myCreateOpenSolidMode;
  }
  
  //! In case of multiconnexity returns compound of fixed solids
  //! else returns one solid.
  Standard_EXPORT TopoDS_Shape Shape();

  DEFINE_STANDARD_RTTIEXT(ShapeFix_Solid,ShapeFix_Root)

protected:
  TopoDS_Shape mySolid;
  Handle(ShapeFix_Shell) myFixShell;
  Standard_Integer myStatus;
  Standard_Integer myFixShellMode;
  Standard_Integer myFixShellOrientationMode;
  Standard_Boolean myCreateOpenSolidMode;
};

#endif // _ShapeFix_Solid_HeaderFile
