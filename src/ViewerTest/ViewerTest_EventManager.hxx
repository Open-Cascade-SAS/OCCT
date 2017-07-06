// Created on: 1998-08-27
// Created by: Robert COUBLANC
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

#ifndef _ViewerTest_EventManager_HeaderFile
#define _ViewerTest_EventManager_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
class AIS_InteractiveContext;
class V3d_View;


class ViewerTest_EventManager;
DEFINE_STANDARD_HANDLE(ViewerTest_EventManager, Standard_Transient)

//! used to manage mouse event (move,select,shiftselect)
//! By default the events are transmitted to interactive context.
class ViewerTest_EventManager : public Standard_Transient
{

public:

  
  Standard_EXPORT ViewerTest_EventManager(const Handle(V3d_View)& aView, const Handle(AIS_InteractiveContext)& aCtx);
  
  Standard_EXPORT virtual void MoveTo (const Standard_Integer xpix, const Standard_Integer ypix);
  
  Standard_EXPORT virtual void Select();
  
  Standard_EXPORT virtual void ShiftSelect();
  
  Standard_EXPORT virtual void Select (const Standard_Integer theXPressed, const Standard_Integer theYPressed, const Standard_Integer theXMotion, const Standard_Integer theYMotion, const Standard_Boolean theIsAutoAllowOverlap = Standard_True);
  
  Standard_EXPORT virtual void ShiftSelect (const Standard_Integer theXPressed, const Standard_Integer theYPressed, const Standard_Integer theXMotion, const Standard_Integer theYMotion, const Standard_Boolean theIsAutoAllowOverlap = Standard_True);
  
  Standard_EXPORT virtual void Select (const TColgp_Array1OfPnt2d& thePolyline);
  
  Standard_EXPORT virtual void ShiftSelect (const TColgp_Array1OfPnt2d& thePolyline);
  
    const Handle(AIS_InteractiveContext)& Context() const;
  
  //! Gets current mouse position. It tracks change of mouse position
  //! with mouse drugging or with DRAW command call (vmoveto).
  Standard_EXPORT void GetCurrentPosition (Standard_Integer& theXPix, Standard_Integer& theYPix) const;




  DEFINE_STANDARD_RTTIEXT(ViewerTest_EventManager,Standard_Transient)

protected:




private:


  Handle(AIS_InteractiveContext) myCtx;
  Handle(V3d_View) myView;
  Standard_Integer myX;
  Standard_Integer myY;


};


#include <ViewerTest_EventManager.lxx>





#endif // _ViewerTest_EventManager_HeaderFile
