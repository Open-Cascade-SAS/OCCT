// Created on: 1994-08-03
// Created by: Christophe MARION
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _HLRTopoBRep_OutLiner_HeaderFile
#define _HLRTopoBRep_OutLiner_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <HLRTopoBRep_Data.hxx>
#include <Standard_Transient.hxx>
#include <BRepTopAdaptor_MapOfShapeTool.hxx>
#include <Standard_Integer.hxx>
class HLRAlgo_Projector;
class TopoDS_Face;


class HLRTopoBRep_OutLiner;
DEFINE_STANDARD_HANDLE(HLRTopoBRep_OutLiner, Standard_Transient)


class HLRTopoBRep_OutLiner : public Standard_Transient
{

public:

  
  Standard_EXPORT HLRTopoBRep_OutLiner();
  
  Standard_EXPORT HLRTopoBRep_OutLiner(const TopoDS_Shape& OriSh);
  
  Standard_EXPORT HLRTopoBRep_OutLiner(const TopoDS_Shape& OriS, const TopoDS_Shape& OutS);
  
    void OriginalShape (const TopoDS_Shape& OriS);
  
    TopoDS_Shape& OriginalShape();
  
    void OutLinedShape (const TopoDS_Shape& OutS);
  
    TopoDS_Shape& OutLinedShape();
  
    HLRTopoBRep_Data& DataStructure();
  
  Standard_EXPORT void Fill (const HLRAlgo_Projector& P, BRepTopAdaptor_MapOfShapeTool& MST, const Standard_Integer nbIso);




  DEFINE_STANDARD_RTTIEXT(HLRTopoBRep_OutLiner,Standard_Transient)

protected:




private:

  
  //! Builds faces from F and add them to S.
  Standard_EXPORT void ProcessFace (const TopoDS_Face& F, TopoDS_Shape& S, BRepTopAdaptor_MapOfShapeTool& M);
  
  Standard_EXPORT void BuildShape (BRepTopAdaptor_MapOfShapeTool& M);

  TopoDS_Shape myOriginalShape;
  TopoDS_Shape myOutLinedShape;
  HLRTopoBRep_Data myDS;


};


#include <HLRTopoBRep_OutLiner.lxx>





#endif // _HLRTopoBRep_OutLiner_HeaderFile
