// Created on: 1996-08-27
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#include <BRepClass3d_SolidClassifier.hxx>
#include <gp_Pnt.hxx>
#include <TopOpeBRepTool_SolidClassifier.hxx>

//=================================================================================================

TopOpeBRepTool_SolidClassifier::TopOpeBRepTool_SolidClassifier()
{
  Clear();
}

// modified by NIZNHY-PKV Mon Dec 16 10:39:00 2002 f
//=================================================================================================

TopOpeBRepTool_SolidClassifier::~TopOpeBRepTool_SolidClassifier()
{
  int i, aNb;

  aNb = myShapeClassifierMap.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    void*                        anAddr = myShapeClassifierMap(i);
    BRepClass3d_SolidClassifier* pClsf  = (BRepClass3d_SolidClassifier*)anAddr;
    delete pClsf;
  }
  myShapeClassifierMap.Clear();
}

//=================================================================================================

void TopOpeBRepTool_SolidClassifier::LoadSolid(const TopoDS_Solid& SOL)
{
  bool found = myShapeClassifierMap.Contains(SOL);

  if (!found)
  {
    myPClassifier = new BRepClass3d_SolidClassifier(SOL);
    myShapeClassifierMap.Add(SOL, (void*)myPClassifier);
  }
  else
  {
    myPClassifier = (BRepClass3d_SolidClassifier*)myShapeClassifierMap.ChangeFromKey(SOL);
  }
}

//=================================================================================================

void TopOpeBRepTool_SolidClassifier::LoadShell(const TopoDS_Shell& SHE)
{
  bool found = myShapeClassifierMap.Contains(SHE);
  if (!found)
  {
    myBuilder.MakeSolid(mySolid);
    myBuilder.Add(mySolid, SHE);

    myPClassifier = new BRepClass3d_SolidClassifier(mySolid);
    myShapeClassifierMap.Add(SHE, (void*)myPClassifier);
  }
  else
  {
    myPClassifier = (BRepClass3d_SolidClassifier*)myShapeClassifierMap.ChangeFromKey(SHE);
  }
}

// modified by NIZNHY-PKV Mon Dec 16 10:53:41 2002 t

//=================================================================================================

void TopOpeBRepTool_SolidClassifier::Clear()
{
  myPClassifier = nullptr;
  // modified by NIZNHY-PKV Mon Dec 16 10:46:04 2002 f
  // myClassifierMap.Clear();
  myShapeClassifierMap.Clear();
  // modified by NIZNHY-PKV Mon Dec 16 10:46:06 2002 t
  myState = TopAbs_UNKNOWN;
  myShell.Nullify();
  mySolid.Nullify();
}

//=================================================================================================

TopAbs_State TopOpeBRepTool_SolidClassifier::Classify(const TopoDS_Solid& SOL,
                                                      const gp_Pnt&       P,
                                                      const double        Tol)
{
  myPClassifier = nullptr;
  myState       = TopAbs_UNKNOWN;
  LoadSolid(SOL);
  if (myPClassifier == nullptr)
    return myState;
  myPClassifier->Perform(P, Tol);
  myState                  = myPClassifier->State();
  const TopoDS_Shape& fres = myPClassifier->Face();
  if (fres.IsNull())
  {
    // NYI: in case of elimination of EXTERNAL and INTERNAL faces by the
    // BRepClass3d_SolidClassifier classifier, still handle these faces
    // to generate ON/Solid state when the point is IN/face INTERNAL or EXTERNAL
    return myState;
  }
  TopAbs_Orientation ofres = fres.Orientation();
  if (ofres == TopAbs_EXTERNAL)
  {
    if (myState == TopAbs_IN)
      myState = TopAbs_OUT;
    else if (myState == TopAbs_OUT)
      myState = TopAbs_OUT;
    else if (myState == TopAbs_ON)
      myState = TopAbs_ON;
    else if (myState == TopAbs_UNKNOWN)
      myState = TopAbs_OUT;
  }
  else if (ofres == TopAbs_INTERNAL)
  {
    if (myState == TopAbs_IN)
      myState = TopAbs_IN;
    else if (myState == TopAbs_OUT)
      myState = TopAbs_IN;
    else if (myState == TopAbs_ON)
      myState = TopAbs_ON;
    else if (myState == TopAbs_UNKNOWN)
      myState = TopAbs_IN;
  }
  return myState;
}

//=================================================================================================

TopAbs_State TopOpeBRepTool_SolidClassifier::Classify(const TopoDS_Shell& SHE,
                                                      const gp_Pnt&       P,
                                                      const double        Tol)
{
  myPClassifier = nullptr;
  myState       = TopAbs_UNKNOWN;
  LoadShell(SHE);
  if (myPClassifier == nullptr)
    return myState;
  myPClassifier->Perform(P, Tol);
  myState = myPClassifier->State();
  return myState;
}

//=================================================================================================

TopAbs_State TopOpeBRepTool_SolidClassifier::State() const
{
  return myState;
}

// modified by NIZNHY-PKV Mon Dec 16 10:55:47 2002 f
/*
//=================================================================================================

void TopOpeBRepTool_SolidClassifier::LoadSolid(const TopoDS_Solid& SOL)
{
  bool found = myClassifierMap.Contains(SOL);
  if ( !found ) {
    myPClassifier = new BRepClass3d_SolidClassifier(SOL);
    myClassifierMap.Add(SOL,*myPClassifier);
  }
  else {
    myPClassifier = &myClassifierMap.ChangeFromKey(SOL);
  }
#ifdef OCCT_DEBUG
  int i =
#endif
                       myClassifierMap.FindIndex(SOL); // DEB
}

//=================================================================================================

void TopOpeBRepTool_SolidClassifier::LoadShell(const TopoDS_Shell& SHE)
{
  bool found = myClassifierMap.Contains(SHE);
  if ( !found ) {
    myBuilder.MakeSolid(mySolid);
    myBuilder.Add(mySolid,SHE);
    TopoDS_Shell* pshe = (TopoDS_Shell*)&SHE; (*pshe).Free(true);
    myPClassifier = new BRepClass3d_SolidClassifier(mySolid);
    myClassifierMap.Add(SHE,*myPClassifier);
  }
  else {
    myPClassifier = &myClassifierMap.ChangeFromKey(SHE);
  }
#ifdef OCCT_DEBUG
  int i =
#endif
                       myClassifierMap.FindIndex(SHE); // DEB
}
*/
// modified by NIZNHY-PKV Mon Dec 16 10:55:56 2002 t
