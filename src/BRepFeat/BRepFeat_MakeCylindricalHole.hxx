// Created on: 1995-06-13
// Created by: Jacques GOUSSARD
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

#ifndef _BRepFeat_MakeCylindricalHole_HeaderFile
#define _BRepFeat_MakeCylindricalHole_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Ax1.hxx>
#include <BRepFeat_Status.hxx>
#include <TopoDS_Face.hxx>
#include <BRepFeat_Builder.hxx>

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Provides a tool to make cylindrical holes on a shape.
class BRepFeat_MakeCylindricalHole  : public BRepFeat_Builder
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
    BRepFeat_MakeCylindricalHole();
  
  //! Sets the axis of the hole(s).
    void Init (const gp_Ax1& Axis);
  
  //! Sets the shape and  axis on which hole(s)  will be
  //! performed.
    void Init (const TopoDS_Shape& S, const gp_Ax1& Axis);

  //! Performs every  holes of   radius  <Radius>.  This
  //! command  has the  same effect as   a cut operation
  //! with an  infinite cylinder   defined by the  given
  //! axis and <Radius>.
  Standard_EXPORT void Perform (const Standard_Real Radius);
  
  //! Performs evry   hole  of  radius  <Radius> located
  //! between PFrom  and  PTo  on the  given  axis.   If
  //! <WithControl> is set  to Standard_False no control
  //! are  done  on   the  resulting  shape   after  the
  //! operation is performed.
  Standard_EXPORT void Perform (const Standard_Real Radius, const Standard_Real PFrom, const Standard_Real PTo, const Standard_Boolean WithControl = Standard_True);
  
  //! Performs the first hole of radius <Radius>, in the
  //! direction of  the defined axis. First hole signify
  //! first encountered after the origin of the axis. If
  //! <WithControl> is set  to Standard_False no control
  //! are  done  on   the  resulting  shape   after  the
  //! operation is performed.
  Standard_EXPORT void PerformThruNext (const Standard_Real Radius, const Standard_Boolean WithControl = Standard_True);
  
  //! Performs  evry  holes of  radius  <Radius> located
  //! after  the   origin  of   the given    axis.    If
  //! <WithControl> is  set to Standard_False no control
  //! are done   on   the  resulting  shape   after  the
  //! operation is performed.
  Standard_EXPORT void PerformUntilEnd (const Standard_Real Radius, const Standard_Boolean WithControl = Standard_True);
  
  //! Performs a  blind   hole of radius    <Radius> and
  //! length <Length>.  The length is  measured from the
  //! origin of the given  axis. If <WithControl> is set
  //! to  Standard_False no  control  are done after the
  //! operation is performed.
  Standard_EXPORT void PerformBlind (const Standard_Real Radius, const Standard_Real Length, const Standard_Boolean WithControl = Standard_True);
  
  //! Returns the status after a hole is performed.
    BRepFeat_Status Status() const;
  
  //! Builds the    resulting shape  (redefined     from
  //! MakeShape). Invalidates the  given parts  of tools
  //! if  any,   and performs the  result   of the local
  //! operation.
  Standard_EXPORT void Build();




protected:

  //! Unhide the base class member to avoid Clang warnings
  using BRepFeat_Builder::Perform;


private:

  
  Standard_EXPORT BRepFeat_Status Validate();


  gp_Ax1 myAxis;
  Standard_Boolean myAxDef;
  BRepFeat_Status myStatus;
  Standard_Boolean myIsBlind;
  Standard_Boolean myValidate;
  TopoDS_Face myTopFace;
  TopoDS_Face myBotFace;


};


#include <BRepFeat_MakeCylindricalHole.lxx>





#endif // _BRepFeat_MakeCylindricalHole_HeaderFile
