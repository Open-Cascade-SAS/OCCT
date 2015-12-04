// Created on: 1995-04-05
// Created by: Christophe MARION
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

#ifndef _HLRTest_Projector_HeaderFile
#define _HLRTest_Projector_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <HLRAlgo_Projector.hxx>
#include <Draw_Drawable3D.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>
class HLRAlgo_Projector;
class Draw_Display;
class Draw_Drawable3D;


class HLRTest_Projector;
DEFINE_STANDARD_HANDLE(HLRTest_Projector, Draw_Drawable3D)

//! Draw Variable Projector to test.
class HLRTest_Projector : public Draw_Drawable3D
{

public:

  
  Standard_EXPORT HLRTest_Projector(const HLRAlgo_Projector& P);
  
    const HLRAlgo_Projector& Projector() const;
  
  //! Does nothhing,
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;
  
  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;
  
  //! For variable whatis command. Set  as a result  the
  //! type of the variable.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(HLRTest_Projector,Draw_Drawable3D)

protected:




private:


  HLRAlgo_Projector myProjector;


};


#include <HLRTest_Projector.lxx>





#endif // _HLRTest_Projector_HeaderFile
