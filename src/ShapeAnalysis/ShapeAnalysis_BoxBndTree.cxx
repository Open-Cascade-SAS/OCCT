// Created on: 2005-02-14
// Created by: Alexey MORENOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <ShapeAnalysis_BoxBndTree.hxx>
#include <Standard_NoSuchObject.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>
#include <ShapeAnalysis.hxx>
#include <gp_Pnt.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>

//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_BoxBndTreeSelector::
  Reject (const Bnd_Box& theBnd) const
{
  Standard_Boolean fch = myFBox.IsOut(theBnd);
  Standard_Boolean lch = myLBox.IsOut(theBnd);
  if (fch == Standard_False || lch == Standard_False) return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : Accept
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_BoxBndTreeSelector::
  Accept (const Standard_Integer& theObj)
{
  if (theObj < 1 || theObj > mySeq->Length())
    throw Standard_NoSuchObject("ShapeAnalysis_BoxBndTreeSelector::Accept : no such object for current index");
  Standard_Boolean IsAccept = Standard_False;
  if (myList.Contains(theObj))
    return Standard_False;
  enum
  {
    First = 1,
    Last = 2
  };
   
  TopoDS_Wire W = TopoDS::Wire (mySeq->Value (theObj));
  TopoDS_Vertex V1,V2;                         
  ShapeAnalysis::FindBounds (W,V1,V2);
  if(myShared){
    if (myLVertex.IsSame(V1)){      
      myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
      IsAccept = Standard_True;
      myArrIndices(Last) = theObj;
    }
    else {
      if (myLVertex.IsSame(V2)){
        myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE2);
        IsAccept = Standard_True;
        myArrIndices(Last) = theObj;
      }
      else {
        if (myFVertex.IsSame(V2)){
          myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE3);
          IsAccept = Standard_True;
          myArrIndices(First) = theObj;
        }
        else {
          if (myFVertex.IsSame(V1)){
            myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE4);
            IsAccept = Standard_True;
            myArrIndices(First) = theObj;
          }
          else myStatus = ShapeExtend::EncodeStatus (ShapeExtend_FAIL2);

          
        }
      }
    }
    
    if (IsAccept){
      SetNb(theObj);
      if(myArrIndices(Last))
        myStop = Standard_True;
      return Standard_True;
    }
    else myStop = Standard_False;
  }
  
  else{
    gp_Pnt p1 = BRep_Tool::Pnt(V1);
    gp_Pnt p2 = BRep_Tool::Pnt(V2);
    
    Standard_Real tailhead, tailtail, headhead, headtail;
    tailhead = p1.Distance(myLPnt);
    tailtail = p2.Distance(myLPnt);
    headhead = p1.Distance(myFPnt);
    headtail = p2.Distance(myFPnt);
    Standard_Real dm1 = tailhead, dm2 = headtail;
    Standard_Integer res1 = 0, res2 = 0;
    if (tailhead > tailtail) {res1 = 1; dm1 = tailtail;}
    if (headtail > headhead) {res2 = 1; dm2 = headhead;}
    Standard_Integer result = res1;
    Standard_Real min3d;
    min3d = Min (dm1, dm2);
    if (min3d > myMin3d)
      return Standard_False;

    Standard_Integer minInd = (dm1 > dm2 ?  First : Last );
    Standard_Integer maxInd = (dm1 > dm2 ? Last : First);
    myArrIndices(minInd) = theObj;
    if((min3d - myMin3d) > RealSmall())
      myArrIndices(maxInd) = 0;
      
    myMin3d = min3d;
    if (min3d > myTol)
    {
       myStatus = ShapeExtend::EncodeStatus (ShapeExtend_FAIL2);
       return Standard_False;
    }
    
    Standard_Integer anObj = (myArrIndices(Last) ? myArrIndices(Last) : myArrIndices(First));
    SetNb(anObj);
    
    if (min3d == 0 && minInd == Last)
      myStop = Standard_True;
   
    if (dm1 > dm2) 
    {
      dm1 = dm2; 
      result = res2 + 2;
    }
    if(anObj == theObj)
    {
      switch (result) {
        case 0: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1); break; 
        case 1: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE2);  break;
        case 2: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE3);  break;
        case 3: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE4);  break;
      }
    }
      return Standard_True;
    
  }  
   
  return Standard_False;
}

