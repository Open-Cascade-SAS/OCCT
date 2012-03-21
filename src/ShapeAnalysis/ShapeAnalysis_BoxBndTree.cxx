// Created on: 2005-02-14
// Created by: Alexey MORENOV
// Copyright (c) 2005-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
    Standard_NoSuchObject::Raise
      ("ShapeAnalysis_BoxBndTreeSelector::Accept : no such object for current index");
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

