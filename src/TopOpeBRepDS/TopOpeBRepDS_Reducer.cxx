// Copyright (c) 1997-1999 Matra Datavision
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


#include <TopOpeBRepDS_EIR.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_FIR.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Reducer.hxx>

//=======================================================================
//function : TopOpeBRepDS_Reducer
//purpose  : 
//=======================================================================
TopOpeBRepDS_Reducer::TopOpeBRepDS_Reducer
(const Handle(TopOpeBRepDS_HDataStructure)& HDS) : myHDS(HDS)
{}

//=======================================================================
//function : ProcessEdgeInterferences
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Reducer::ProcessEdgeInterferences()
{
  TopOpeBRepDS_EIR eir(myHDS);
  eir.ProcessEdgeInterferences();
}

//=======================================================================
//function : ProcessFaceInterferences
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Reducer::ProcessFaceInterferences
(const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& M)
{
  //modified by NIZHNY-MZV  Tue Nov 16 16:12:15 1999
  //FUN_ds_FEIGb1TO0(myHDS,M); //xpu250199

  TopOpeBRepDS_FIR fir(myHDS);
  fir.ProcessFaceInterferences(M);
}
