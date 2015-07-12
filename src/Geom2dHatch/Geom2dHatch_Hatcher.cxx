// Created on: 1993-11-04
// Created by: Jean Marc LACHAUME
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Classifier.hxx>
#include <Geom2dHatch_Element.hxx>
#include <Geom2dHatch_Hatcher.hxx>
#include <Geom2dHatch_Hatching.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <HatchGen_Domain.hxx>
#include <HatchGen_Domains.hxx>
#include <HatchGen_PointOnElement.hxx>
#include <HatchGen_PointOnHatching.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <IntRes2d_Transition.hxx>
#include <Precision.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TopAbs.hxx>
#include <TopTrans_CurveTransition.hxx>

#define RAISE_IF_NOSUCHOBJECT 0
#define TRACE_HATCHER 0

//=======================================================================
//=======================================================================
//  Category : General use.
//=======================================================================
//=======================================================================

//=======================================================================
// Function : Geom2dHatch_Hatcher
// Purpose  : Constructor.
//=======================================================================

Geom2dHatch_Hatcher::Geom2dHatch_Hatcher (const Geom2dHatch_Intersector&  Intersector,
				    const Standard_Real    Confusion2d,
				    const Standard_Real    Confusion3d,
				    const Standard_Boolean KeepPnt,
				    const Standard_Boolean KeepSeg) :
       myIntersector  (Intersector) ,
       myConfusion2d  (Confusion2d) ,
       myConfusion3d  (Confusion3d) ,
       myKeepPoints   (KeepPnt) ,
       myKeepSegments (KeepSeg) ,
       myNbElements   (0) ,
       myNbHatchings  (0) 
{
}

//=======================================================================
// Function : Intersector
// Purpose  : Sets the associated intersector.
//=======================================================================

void Geom2dHatch_Hatcher::Intersector (const Geom2dHatch_Intersector& Intersector)
{
  myIntersector = Intersector ;
  for (Standard_Integer IndH = 1 ; IndH <= myNbHatchings ; IndH++) {
    if (myHatchings.IsBound (IndH)) {
      Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;
      Hatching.ClrPoints() ;
    }
  }
}


//=======================================================================
// Function : Confusion2d
// Purpose  : Sets the 2dconfusion tolerance.
//=======================================================================

void Geom2dHatch_Hatcher::Confusion2d (const Standard_Real Confusion)
{
  myConfusion2d = Confusion ;
  for (Standard_Integer IndH = 1 ; IndH <= myNbHatchings ; IndH++) {
    if (myHatchings.IsBound (IndH)) {
      Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;
      Hatching.ClrPoints() ;
    }
  }
}


//=======================================================================
// Function : Confusion3d
// Purpose  : Sets the 3d confusion tolerance.
//=======================================================================

void Geom2dHatch_Hatcher::Confusion3d (const Standard_Real Confusion)
{
  myConfusion3d = Confusion ;
  for (Standard_Integer IndH = 1 ; IndH <= myNbHatchings ; IndH++) {
    if (myHatchings.IsBound (IndH)) {
      Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;
      Hatching.ClrPoints() ;
    }
  }
}

//=======================================================================
// Function : KeepPoints
// Purpose  : Sets the above flag.
//=======================================================================

void Geom2dHatch_Hatcher::KeepPoints (const Standard_Boolean Keep)
{
  myKeepPoints = Keep ;
  for (Standard_Integer IndH = 1 ; IndH <= myNbHatchings ; IndH++) {
    if (myHatchings.IsBound (IndH)) {
      Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;
      Hatching.ClrDomains() ;
    }
  }
}


//=======================================================================
// Function : KeepSegments
// Purpose  : Sets the above flag.
//=======================================================================

void Geom2dHatch_Hatcher::KeepSegments (const Standard_Boolean Keep)
{
  myKeepSegments = Keep ;
  for (Standard_Integer IndH = 1 ; IndH <= myNbHatchings ; IndH++) {
    if (myHatchings.IsBound (IndH)) {
      Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;
      Hatching.ClrDomains() ;
    }
  }
}



//=======================================================================
//=======================================================================
//  Category : Element.
//=======================================================================
//=======================================================================


//=======================================================================
// Function : AddElement
// Purpose  : Adds an element to the Hatcher and returns its index.
//=======================================================================

Standard_Integer Geom2dHatch_Hatcher::AddElement (const Geom2dAdaptor_Curve& Curve,
					       const TopAbs_Orientation Orientation)
{
  Standard_Integer IndE ;
  for (IndE = 1 ; IndE <= myNbElements && myElements.IsBound(IndE) ; IndE++) ;
  if (IndE > myNbElements) {
    myNbElements++ ;
    IndE = myNbElements ;
  }
  Geom2dHatch_Element Element (Curve, Orientation) ;
  myElements.Bind (IndE, Element) ;
  for (Standard_Integer IndH = 1 ; IndH <= myNbHatchings; IndH++) {
    if (myHatchings.IsBound(IndH)) {
      Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;
      Hatching.ClrPoints () ;
    }
  }
  return IndE ;
}

//=======================================================================
// Function : RemElement
// Purpose  : Removes the IndE-th element from the hatcher.
//=======================================================================

void Geom2dHatch_Hatcher::RemElement (const Standard_Integer IndE)
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_NoSuchObject_Raise_if (!myElements.IsBound (IndE), "") ;
#endif
  for (Standard_Integer IndH = 1 ; IndH <= myNbHatchings ; IndH++) {
    if (myHatchings.IsBound (IndH)) {
      Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;
      Standard_Boolean DomainsToClear = Standard_False ;
      for (Standard_Integer IPntH = Hatching.NbPoints() ; IPntH > 0 ; IPntH--) {
	HatchGen_PointOnHatching PntH = Hatching.ChangePoint (IPntH) ;
	for (Standard_Integer IPntE = PntH.NbPoints() ; IPntE > 0 ; IPntE--) {
	  if (PntH.Point(IPntE).Index() == IndE) {
	    PntH.RemPoint (IPntE) ;
	    DomainsToClear = Standard_True ;
	  }
	}
	if (PntH.NbPoints() == 0) Hatching.RemPoint (IPntH) ;
      }
      if (DomainsToClear) Hatching.ClrDomains() ;
    }
  }
  myElements.UnBind (IndE) ;
  if (IndE == myNbElements) myNbElements-- ;
}

//=======================================================================
// Function : ClrElements
// Purpose  : Removes all the elements from the hatcher.
//=======================================================================

void Geom2dHatch_Hatcher::ClrElements ()
{
  if (myNbElements != 0) {
    if (myNbHatchings != 0) {
      for (Standard_Integer IndH = 1 ; IndH <= myNbHatchings ; IndH++) {
	if (myHatchings.IsBound(IndH)) {
	  Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;
	  Hatching.ClrPoints() ;
	}
      }
    }
    myElements.Clear() ;
    myNbElements = 0 ;
  }
}

//=======================================================================
//=======================================================================
//  Category : Hatching.
//=======================================================================
//=======================================================================


//=======================================================================
// Function : AddHatching
// Purpose  : Adds a hatching to the hatcher and returns its index.
//=======================================================================

Standard_Integer Geom2dHatch_Hatcher::AddHatching (const Geom2dAdaptor_Curve& Curve)
{
  Standard_Integer IndH ;
  for (IndH = 1 ; IndH <= myNbHatchings && myHatchings.IsBound(IndH) ; IndH++) ;
  if (IndH > myNbHatchings) {
    myNbHatchings++ ;
    IndH = myNbHatchings ;
  }
  Geom2dHatch_Hatching Hatching (Curve) ;
  myHatchings.Bind (IndH, Hatching) ;
  return IndH ;
}

//=======================================================================
// Function : RemHatching
// Purpose  : Removes the IndH-th hatching from the hatcher.
//=======================================================================

void Geom2dHatch_Hatcher::RemHatching (const Standard_Integer IndH)
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_NoSuchObject_Raise_if (!myHatchings.IsBound (IndH), "") ;
#endif
  Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;
  Hatching.ClrPoints() ;
  myHatchings.UnBind (IndH) ;
  if (IndH == myNbHatchings) myNbHatchings-- ;
}
  
//=======================================================================
// Function : ClrHatchings
// Purpose  : Removes all the hatchings from the hatcher.
//=======================================================================

void Geom2dHatch_Hatcher::ClrHatchings ()
{
  if (myNbHatchings != 0) {
    for (Standard_Integer IndH = 1 ; IndH <= myNbHatchings ; IndH++) {
      if (myHatchings.IsBound(IndH)) {
	Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;
	Hatching.ClrPoints() ;
      }
    }
    myHatchings.Clear() ;
    myNbHatchings = 0 ;
  }
}



//=======================================================================
//=======================================================================
//  Category : Computation - Trimming
//=======================================================================
//=======================================================================

//=======================================================================
// Function : Trim
// Purpose  : Trims all the hatchings of the hatcher by all the elements
//            of the hatcher.
//=======================================================================

void Geom2dHatch_Hatcher::Trim ()
{
  for (Standard_Integer IndH = 1 ; IndH <= myNbHatchings ; IndH++)
    if (myHatchings.IsBound (IndH)) 
      Trim (IndH) ;
}

//=======================================================================
// Function : Trim
// Purpose  : Adds a hatching to the hatcher and trims it by the elements
//            already given and returns its index.
//=======================================================================

Standard_Integer Geom2dHatch_Hatcher::Trim (const Geom2dAdaptor_Curve& Curve)
{
  Standard_Integer IndH = AddHatching (Curve) ;
  Trim (IndH) ;
  return IndH ;
}

//=======================================================================
// Function : Trim
// Purpose  : Trims the IndH-th hatching by the elements already given.
//=======================================================================

void Geom2dHatch_Hatcher::Trim (const Standard_Integer IndH)
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_NoSuchObject_Raise_if (!myHatchings.IsBound (IndH), "") ;
#endif

  Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;

  Hatching.ClrPoints() ;

  Standard_Boolean OK, AllOK ;

  AllOK = Standard_True ;
  for (Standard_Integer IndE = 1 ; IndE <= myNbElements ; IndE++) {
    if (myElements.IsBound (IndE)) {
      OK = Trim (IndH, IndE) ;
      AllOK = AllOK && OK ;
    }
  }
  Hatching.TrimDone (Standard_True) ;
  Hatching.TrimFailed (!AllOK) ;

  if (AllOK) {
    for (Standard_Integer IPnt = 1 ; IPnt <= Hatching.NbPoints() ; IPnt++) {
      HatchGen_PointOnHatching& PntH = Hatching.ChangePoint(IPnt) ;
      OK = GlobalTransition (PntH) ;
      AllOK = AllOK && OK ;
    }
    Hatching.Status (AllOK ? HatchGen_NoProblem : HatchGen_TransitionFailure) ;
  }
}

#if TRACE_HATCHER

//=======================================================================
// Function : IntersectionPointDump
// Purpose  : Dump of the intersection point.
//=======================================================================

static void IntersectionPointDump (const IntRes2d_IntersectionPoint& Pnt,
				   const Standard_Integer Index)
{
  Standard_Integer SavedPrecision = cout.precision() ;
  cout.precision (15) ;
  cout << "----- IntRes2d:: Point # " << setw(3) << Index << " ---------------" << endl ;
  cout << "-- U: "<<Pnt.Value().X()<<"    V: "<<Pnt.Value().Y()<<endl;
  cout << "-- Parameter on first   : " << Pnt.ParamOnFirst()  << endl ;
  cout << "-- Position  on first   : " ;
  switch (Pnt.TransitionOfFirst().PositionOnCurve()) {
      case IntRes2d_Head   : cout << "HEAD"   ; break ;
      case IntRes2d_Middle : cout << "MIDDLE" ; break ;
      case IntRes2d_End    : cout << "END"    ; break ;
  }
  cout << endl ;
  cout << "-- IntRes2d:: Transition on first  : " ;
  switch (Pnt.TransitionOfFirst().TransitionType()) {
      case IntRes2d_In        : cout << "IN"        ; break ;
      case IntRes2d_Out       : cout << "OUT"       ; break ;
      case IntRes2d_Touch     : cout << "TOUCH"     ; break ;
      case IntRes2d_Undecided : cout << "UNDECIDED" ; break ;
  }
  cout << endl ;
  if (Pnt.TransitionOfFirst().TransitionType() == IntRes2d_Touch) {
    cout << "-- IntRes2d:: Situation on first   : " ;
    switch (Pnt.TransitionOfFirst().Situation()) {
	case IntRes2d_Inside  : cout << "INSIDE"  ; break ;
	case IntRes2d_Outside : cout << "OUTSIDE" ; break ;
	case IntRes2d_Unknown : cout << "UNKNOWN" ; break ;
    }
    cout << endl ;
  }
  cout << "--------------------------------------------" << endl ;
  cout << "-- Parameter on second  : " << Pnt.ParamOnSecond() << endl ;
  cout << "-- Position  on second  : " ;
  switch (Pnt.TransitionOfSecond().PositionOnCurve()) {
      case IntRes2d_Head   : cout << "HEAD"   ; break ;
      case IntRes2d_Middle : cout << "MIDDLE" ; break ;
      case IntRes2d_End    : cout << "END"    ; break ;
  }
  cout << endl ;
  cout << "-- IntRes2d:: Transition on second : " ;
  switch (Pnt.TransitionOfSecond().TransitionType()) {
      case IntRes2d_In        : cout << "IN"        ; break ;
      case IntRes2d_Out       : cout << "OUT"       ; break ;
      case IntRes2d_Touch     : cout << "TOUCH"     ; break ;
      case IntRes2d_Undecided : cout << "UNDECIDED" ; break ;
  }
  cout << endl ;
  if (Pnt.TransitionOfSecond().TransitionType() == IntRes2d_Touch) {
    cout << "-- IntRes2d:: Situation on second  : " ;
    switch (Pnt.TransitionOfSecond().Situation()) {
	case IntRes2d_Inside  : cout << "INSIDE"  ; break ;
	case IntRes2d_Outside : cout << "OUTSIDE" ; break ;
	case IntRes2d_Unknown : cout << "UNKNOWN" ; break ;
    }
    cout << endl ;
  }
  cout << "--------------------------------------------" << endl ;
  cout.precision (SavedPrecision) ;
}

#endif

//=======================================================================
// Function : Trim
// Purpose  : Trims the IndH-th hatching of the hatcher by the IndE th
//            element.
//=======================================================================

Standard_Boolean Geom2dHatch_Hatcher::Trim (const Standard_Integer IndH,
					 const Standard_Integer IndE)
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_NoSuchObject_Raise_if (!myHatchings.IsBound (IndH), "") ;
  Standard_NoSuchObject_Raise_if (!myElements.IsBound (IndE), "") ;
#endif

  Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;
  Geom2dHatch_Element& Element   = myElements.ChangeFind  (IndE) ;

  Geom2dAdaptor_Curve hatching = Hatching.ChangeCurve() ;
  Geom2dAdaptor_Curve element  = Element.ChangeCurve() ;

  myIntersector.Intersect (hatching, element) ;
  
#if TRACE_HATCHER
  cout << "--- Hatcher - Trim:: Hatching # " << setw(3);
  cout << IndH << " with Element # " << setw(3);
  cout << IndE << " ----------" << endl ;
#endif    
  
  if (!myIntersector.IsDone())  { 
    cout<<" Intersector -> Done = False ";
    return Standard_False ;
  }
  
#if TRACE_HATCHER
  if (myIntersector.IsEmpty()) {
    cout << "No intersection" << endl ;
    cout << "--------------------------------------------------------------------" << endl ;
  }
#endif    
  
  if (myIntersector.IsEmpty()) return Standard_True ;
  
#if TRACE_HATCHER
  cout << "Number of intersection points   : " << setw(3) << (myIntersector.NbPoints())   << endl ;
  cout << "Number of intersection segments : " << setw(3) << (myIntersector.NbSegments()) << endl ;
#endif    
  
  //-----------------------------------------------------------------------
  // Traitement des points d intersection.
  //-----------------------------------------------------------------------
  
  for (Standard_Integer IPntI = 1 ; IPntI <= myIntersector.NbPoints() ; IPntI++) {
    const IntRes2d_IntersectionPoint& PntI = myIntersector.Point (IPntI) ;
    
#if TRACE_HATCHER
    IntersectionPointDump (PntI, IPntI) ;
#endif
    
    HatchGen_PointOnElement PntE (PntI) ;
    PntE.SetIndex (IndE) ;
    
    HatchGen_PointOnHatching PntH (PntI) ;
    PntH.SetIndex (IndH) ;
    PntH.AddPoint (PntE, myConfusion2d) ;
    
    Hatching.AddPoint (PntH, myConfusion2d) ;
  }
  
  //-----------------------------------------------------------------------
  // Traitement des segments d intersection.
  //-----------------------------------------------------------------------
  
  for (Standard_Integer ISeg = 1 ; ISeg <= myIntersector.NbSegments() ; ISeg++) {
    
    const IntRes2d_IntersectionSegment& Seg = myIntersector.Segment (ISeg) ;
    
#if TRACE_HATCHER
    cout << "----- Segment # " << setw(3) << ISeg << " -------------" << endl ;
#endif
    
    Standard_Boolean FirstPoint = Seg.HasFirstPoint() ;
    Standard_Boolean LastPoint  = Seg.HasLastPoint() ;
    
    //-----------------------------------------------------------------------
    // Les deux points peuvent etre confondus.
    //-----------------------------------------------------------------------
    
    if (FirstPoint && LastPoint) {
      
      const IntRes2d_IntersectionPoint& Pnt1 = Seg.FirstPoint() ;
      const IntRes2d_IntersectionPoint& Pnt2 = Seg.LastPoint()  ;
      
      const IntRes2d_Transition& TrsPnt1H = Pnt1.TransitionOfFirst() ;
      const IntRes2d_Transition& TrsPnt1E = Pnt1.TransitionOfSecond() ;
      const IntRes2d_Transition& TrsPnt2H = Pnt2.TransitionOfFirst() ;
      const IntRes2d_Transition& TrsPnt2E = Pnt2.TransitionOfSecond() ;
      
      IntRes2d_TypeTrans TypePnt1H = TrsPnt1H.TransitionType() ;
      IntRes2d_TypeTrans TypePnt1E = TrsPnt1E.TransitionType() ;
      IntRes2d_TypeTrans TypePnt2H = TrsPnt2H.TransitionType() ;
      IntRes2d_TypeTrans TypePnt2E = TrsPnt2E.TransitionType() ;
      
      //-----------------------------------------------------------------------
      // Les deux points peuvent etre confondus au regard de la precision du
      // `hatcher'.
      //-----------------------------------------------------------------------
      
      Standard_Boolean Conf2d = Abs (Pnt1.ParamOnFirst() - Pnt2.ParamOnFirst()) <= myConfusion2d ;

      //-----------------------------------------------------------------------
      // Les deux points peuvent etre `confondus' au regard des intersections.
      //-----------------------------------------------------------------------

      Standard_Boolean Conf3d = Standard_False ;

      if (!Conf2d) {
	Conf3d = Standard_True ;
	if (Conf3d) Conf3d = TypePnt1H != IntRes2d_Touch && TypePnt1H != IntRes2d_Undecided ;
	if (Conf3d) Conf3d = TypePnt1E != IntRes2d_Touch && TypePnt1E != IntRes2d_Undecided ;
	if (Conf3d) Conf3d = TypePnt2H != IntRes2d_Touch && TypePnt2H != IntRes2d_Undecided ;
	if (Conf3d) Conf3d = TypePnt2E != IntRes2d_Touch && TypePnt2E != IntRes2d_Undecided ;
	if (Conf3d) Conf3d = TypePnt1H == TypePnt2H      && TypePnt1E == TypePnt2E ;
	if (Conf3d) Conf3d = Pnt1.Value().Distance (Pnt2.Value()) <= myConfusion3d ;
      }

      if (Conf2d || Conf3d) {
	
	HatchGen_PointOnElement PntE ;
	PntE.SetIndex (IndE) ;
	PntE.SetParameter ((Pnt1.ParamOnSecond() + Pnt2.ParamOnSecond()) / 2.) ;
	switch (TrsPnt1E.PositionOnCurve()) {
	  case IntRes2d_Head: { 
	    PntE.SetPosition(TopAbs_FORWARD) ;
	    break ;
	  }
	  case IntRes2d_Middle: {
	    switch (TrsPnt2E.PositionOnCurve()) {
	       case IntRes2d_Head: {
		 PntE.SetPosition (TopAbs_FORWARD);
		 break;
	       }
	       case IntRes2d_Middle: { 
		 PntE.SetPosition (TopAbs_INTERNAL) ;
		 break ;
	       }
	       case IntRes2d_End: {
		 PntE.SetPosition (TopAbs_REVERSED) ;
		 break ;
	       }
	       default: {
		 break;
	       }
	    }
	    break;
	  }
	  case IntRes2d_End:  { 
	    PntE.SetPosition(TopAbs_REVERSED) ;
	    break ;
	  }
          default: {
	    break;
	  }
	}
	PntE.SetIntersectionType 
	  ((PntE.Position() == TopAbs_INTERNAL) ? HatchGen_TRUE : HatchGen_TOUCH) ;
	PntE.SetStateBefore ((TypePnt1H == IntRes2d_In) ? TopAbs_OUT : TopAbs_IN) ;
	PntE.SetStateAfter  ((TypePnt2H == IntRes2d_In) ? TopAbs_OUT : TopAbs_IN) ;
	
	HatchGen_PointOnHatching PntH ;
	PntH.SetIndex (IndH) ;
	PntH.SetParameter ((Pnt1.ParamOnFirst() + Pnt2.ParamOnFirst()) / 2.) ;
	switch (TrsPnt1H.PositionOnCurve()) {
	   case IntRes2d_Head: {
	     PntH.SetPosition (TopAbs_FORWARD) ;
	     break ;
	   }
	   case IntRes2d_Middle: {
	     switch (TrsPnt2H.PositionOnCurve()) {
	        case IntRes2d_Head: {
		  PntH.SetPosition (TopAbs_FORWARD) ;
		  break ;
		}
		case IntRes2d_Middle: {
		  PntH.SetPosition (TopAbs_INTERNAL) ;
		  break ;
		}
		case IntRes2d_End: {
		  PntH.SetPosition (TopAbs_REVERSED) ;
		  break ;
		}
		default : {
		  break ;
		}
	     }
	     break ;
	   }
	   case IntRes2d_End: {
	     PntH.SetPosition (TopAbs_REVERSED) ;
	     break ;
	   }
	   default : {
	     break ;
	   }
	}

	PntH.AddPoint (PntE, myConfusion2d) ;
	Hatching.AddPoint (PntH, myConfusion2d) ;
	
#if TRACE_HATCHER
	IntersectionPointDump (Pnt1, 1) ;
	IntersectionPointDump (Pnt2, 2) ;
	cout << "THESE TWO POINTS ARE "
	     << (Conf2d ? "2D" : "3D")
	     << " CONFUSED INTO THE FOLLOWING" << endl ;
	PntH.Dump() ;
#endif
	continue ;
	
      }
      
      //-----------------------------------------------------------------------
      // Traitement du premier point du segment.
      //-----------------------------------------------------------------------
      
      if (FirstPoint) {
	
	const IntRes2d_IntersectionPoint& PntI = Seg.FirstPoint() ;
	
#if TRACE_HATCHER
	IntersectionPointDump (PntI, 1) ;
#endif
	
	HatchGen_PointOnElement PntE (PntI) ;
	PntE.SetIndex (IndE) ;
	PntE.SetSegmentBeginning (Standard_True)  ;
	PntE.SetSegmentEnd       (Standard_False) ;
	
	HatchGen_PointOnHatching PntH (PntI) ;
	PntH.SetIndex (IndH) ;
	PntH.AddPoint (PntE, myConfusion2d) ;
	
	Hatching.AddPoint (PntH, myConfusion2d) ;
	
#if TRACE_HATCHER
      } 
      else {
	cout << "----- Has no first point --------" << endl ;
	cout << "---------------------------------" << endl ;
#endif
	
      }
      
      //-----------------------------------------------------------------------
      // Traitement du deuxieme point du segment.
      //-----------------------------------------------------------------------
      
      if (LastPoint) {
	
	const IntRes2d_IntersectionPoint& PntI = Seg.LastPoint() ;
	
#if TRACE_HATCHER
	IntersectionPointDump (PntI, 2) ;
#endif
	
	HatchGen_PointOnElement PntE (PntI) ;
	PntE.SetIndex (IndE) ;
	PntE.SetSegmentBeginning (Standard_False) ;
	PntE.SetSegmentEnd       (Standard_True)  ;
	
	HatchGen_PointOnHatching PntH (PntI) ;
	PntH.SetIndex (IndH) ;
	PntH.AddPoint (PntE, myConfusion2d) ;
	
	Hatching.AddPoint (PntH, myConfusion2d) ;
	
#if TRACE_HATCHER
      } 
      else {
	cout << "----- Has no last point ---------" << endl ;
	cout << "---------------------------------" << endl ;
#endif
      }
    }
#if TRACE_HATCHER
    cout << "--------------------------------------------------------------------" << endl ;
#endif    
    
  }
  return Standard_True;
}
//=======================================================================
//=======================================================================
//  Category : Computation - Domains
//=======================================================================
//=======================================================================

//=======================================================================
// Function : GlobalTransition
// Purpose  : Returns the before and after states of the complex
//            transition of the IndP-th intersection point of the
//            IndH-th hatching.
//=======================================================================

Standard_Boolean Geom2dHatch_Hatcher::GlobalTransition (HatchGen_PointOnHatching& Point)
{
  TopAbs_State StateBefore = TopAbs_UNKNOWN ;
  TopAbs_State StateAfter  = TopAbs_UNKNOWN ;
  Standard_Boolean SegmentBegin = Standard_False ;
  Standard_Boolean SegmentEnd   = Standard_False ;

  gp_Dir2d Tangente2d, Normale2d ;
  gp_Dir   Tangente,   Normale ;
  Standard_Real Courbure ;

  const Geom2dAdaptor_Curve& CurveH = HatchingCurve (Point.Index()) ;

  myIntersector.LocalGeometry(CurveH.Curve(), Point.Parameter(), Tangente2d, Normale2d, Courbure);

  Tangente.SetCoord (Tangente2d.X(), Tangente2d.Y(), 0.0) ;
  if (Courbure < Precision::Confusion()) {
    Normale.SetCoord (-Tangente2d.Y(), Tangente2d.X(), 0.0) ;
  } else {
    Normale.SetCoord (Normale2d.X(), Normale2d.Y(), 0.0) ;
  }

  TopTrans_CurveTransition ComplexTransition ;
  ComplexTransition.Reset (Tangente, Normale, Courbure) ;

#if TRACE_HATCHER
  printf("\n ----- Global Transition Complex Transition Reset \n");
  printf("\n       P:%+10.5g  Tg2d:%+10.5g , %+10.5g  N2d:%+10.5g , %+10.5g  Crv:%+10.5g\n\n",
  Point.Parameter(),Tangente.X(),Tangente.Y(),Normale.X(),Normale.Y(),Courbure);
#endif
  for (Standard_Integer IPntE = 1 ; IPntE <= Point.NbPoints() ; IPntE++) 
  {
    const HatchGen_PointOnElement& PntE = Point.Point (IPntE) ;
    
    SegmentBegin = SegmentBegin || PntE.SegmentBeginning() ;
    SegmentEnd   = SegmentEnd   || PntE.SegmentEnd() ;
    
    const Geom2dHatch_Element& Element = myElements.Find (PntE.Index()) ;
    const Geom2dAdaptor_Curve& CurveE = Element.Curve() ;
    
    TopAbs_Orientation ElementOrientation = Element.Orientation() ;
    Standard_Boolean ToReverse = (ElementOrientation == TopAbs_REVERSED);
    Standard_Real Param ;
    switch (PntE.Position()) 
    {
      case TopAbs_FORWARD  : 
        Param = ToReverse ? CurveE.LastParameter() : CurveE.FirstParameter() ;
        break ;

      case TopAbs_INTERNAL : 
        Param = PntE.Parameter() ;
        break ;

      case TopAbs_REVERSED : 
        Param = ToReverse ? CurveE.FirstParameter() : CurveE.LastParameter() ;
        break ;

      default:
        break;
    }
    
//-- 
#if TRACE_HATCHER
    printf("\n ******** ToReverse: %d Param : %g   ANParam : %g \n",ToReverse,Param,PntE.Parameter());
#endif
    Param = PntE.Parameter();

    myIntersector.LocalGeometry(CurveE.Curve(), Param, Tangente2d, Normale2d, Courbure);

//-----------------------------------------------------------------------
// Calcul de la transition locale. On suppose les relations suivantes :
//  - Si l orientation de l element est INTERNAL ==> INTERNAL
//  - Si l orientation de l element est EXTERNAL ==> EXTERNAL
//  - Si tangence, on a IN-IN  ou OUT-OUT ==> INTERNAL/EXTERNAL
//  - Sinon,       on a IN-OUT ou OUT-IN  ==> REVERSED/FORWARD 
// Les deux dernieres conditions avec l element vu en FORWARD.    
//-----------------------------------------------------------------------
    TopAbs_Orientation LocalTransition = TopAbs_EXTERNAL;

    if (ElementOrientation == TopAbs_INTERNAL) 
      LocalTransition = TopAbs_INTERNAL ;

    else if (ElementOrientation == TopAbs_EXTERNAL) 
      LocalTransition = TopAbs_EXTERNAL ;

    else if (PntE.IntersectionType() == HatchGen_TANGENT) 
    {
      if (PntE.Position() == TopAbs_INTERNAL) 
      {
        switch (PntE.StateBefore()) 
        {
        case TopAbs_IN  : LocalTransition = ToReverse ? TopAbs_EXTERNAL : TopAbs_INTERNAL ; break ;
        case TopAbs_OUT : LocalTransition = ToReverse ? TopAbs_INTERNAL : TopAbs_EXTERNAL ; break ;
        default: break;
        }
      } 
      else 
      {
        switch (PntE.StateBefore()) 
        {
        case TopAbs_IN  : LocalTransition = ToReverse ? TopAbs_FORWARD  : TopAbs_REVERSED ; break ;
        case TopAbs_OUT : LocalTransition = ToReverse ? TopAbs_REVERSED : TopAbs_FORWARD  ; break ;
        default: break;
        }
      }
    } 
    else 
    {
      switch (PntE.StateBefore()) 
      {
      case TopAbs_IN  : LocalTransition = ToReverse ? TopAbs_FORWARD  : TopAbs_REVERSED ; break ;
      case TopAbs_OUT : LocalTransition = ToReverse ? TopAbs_REVERSED : TopAbs_FORWARD  ; break ;
      default: break;
      }
    }

//-----------------------------------------------------------------------
// Orientation de la tangente au point d interference.
//-----------------------------------------------------------------------
    TopAbs_Orientation TangenteOrientation = TopAbs_FORWARD;
    switch (PntE.Position()) 
    {
    case TopAbs_FORWARD  : TangenteOrientation = ToReverse ? TopAbs_REVERSED : TopAbs_FORWARD  ; break ;
    case TopAbs_INTERNAL : TangenteOrientation = TopAbs_INTERNAL ; break ;
    case TopAbs_REVERSED : TangenteOrientation = ToReverse ? TopAbs_FORWARD  : TopAbs_REVERSED ; break ;
    
    default: 
      break;
    }

//-----------------------------------------------------------------------
// Proprietes geometriques.
//-----------------------------------------------------------------------

    if (ToReverse) {
      Tangente.SetCoord (-Tangente2d.X(), -Tangente2d.Y(), 0.0) ;
    } else {
      Tangente.SetCoord ( Tangente2d.X(),  Tangente2d.Y(), 0.0) ;
    }
    Normale.SetCoord ( Normale2d.X(),  Normale2d.Y(), 0.0) ;

#if TRACE_HATCHER
    printf("\n \n----- Global Transition Complex Transition Compare" );
    char *str1 = " ??? ";
    char *str2 = " ??? ";
    if(LocalTransition == TopAbs_INTERNAL) str1=" INTERNAL ";
    if(LocalTransition == TopAbs_REVERSED) str1=" REVERSED ";
    if(LocalTransition == TopAbs_FORWARD)  str1=" FORWARD  ";

    if(TangenteOrientation == TopAbs_INTERNAL) str2=" INTERNAL ";
    if(TangenteOrientation == TopAbs_REVERSED) str2=" REVERSED ";
    if(TangenteOrientation == TopAbs_FORWARD)  str2=" FORWARD  ";

    printf("\n       P:%+10.5g  Tg2d:%+10.5g , %+10.5g  N2d:%+10.5g , %+10.5g  Crv:%+10.5g LocalTr:%s TangOrie:%s\n",
    Param,Tangente.X(),Tangente.Y(),Normale.X(),Normale.Y(),Courbure,str1,str2);
#endif

    ComplexTransition.Compare (Precision::Angular(),
                               Tangente, Normale, Courbure,
                               LocalTransition, TangenteOrientation) ;
  }

  switch (ComplexTransition.StateBefore()) {
      case TopAbs_IN      : StateBefore = TopAbs_IN  ; break ;
      case TopAbs_OUT     : StateBefore = TopAbs_OUT ; break ;
      case TopAbs_ON      : return Standard_False ;
      case TopAbs_UNKNOWN : return Standard_False ;
  }
  switch (ComplexTransition.StateAfter()) {
      case TopAbs_IN      : StateAfter = TopAbs_IN  ; break ;
      case TopAbs_OUT     : StateAfter = TopAbs_OUT ; break ;
      case TopAbs_ON      : return Standard_False ;
      case TopAbs_UNKNOWN : return Standard_False ;
  }


#if TRACE_HATCHER
  printf("\n");
  printf("\n --> StateBef :"); if(StateBefore==TopAbs_IN) printf(" IN "); else printf(" OUT ");
  printf("\n --> StateAft :"); if(StateAfter==TopAbs_IN) printf(" IN "); else printf(" OUT ");
  printf("\n------   Fin GlobalTransition\n");
#endif
  
  Point.SetStateBefore      (StateBefore) ;
  Point.SetStateAfter       (StateAfter) ;
  Point.SetSegmentBeginning (SegmentBegin) ;
  Point.SetSegmentEnd       (SegmentEnd) ;
  return Standard_True ;
}

//=======================================================================
// Function : ComputeDomains
// Purpose  : Computes the domains of all the hatchings.
//=======================================================================

void Geom2dHatch_Hatcher::ComputeDomains ()
{
  for (Standard_Integer IndH = 1 ; IndH <= myNbHatchings ; IndH++)
    if (myHatchings.IsBound (IndH)) ComputeDomains (IndH) ;
}

//=======================================================================
// Function : ComputeDomains
// Purpose  : Computes the domains of the IndH-th hatching.
//=======================================================================

void Geom2dHatch_Hatcher::ComputeDomains (const Standard_Integer IndH)
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_NoSuchObject_Raise_if (!myHatchings.IsBound (IndH), "") ;
#endif
  
  Geom2dHatch_Hatching& Hatching = myHatchings.ChangeFind (IndH) ;
  Hatching.ClrDomains() ;

  Hatching.IsDone (Standard_False) ;

  if (!Hatching.TrimDone())
    Trim (IndH);

  if (Hatching.Status() != HatchGen_NoProblem)
    return;
  
  Standard_Boolean Points   = myKeepPoints ;
  Standard_Boolean Segments = myKeepSegments ;
  Standard_Integer ISav = 0 ;
  Standard_Boolean SavPnt  = Standard_False ;
  Standard_Integer NbOpenedSegments = 0 ;
  Standard_Integer NbPnt = Hatching.NbPoints() ;
  Standard_Integer IPnt =1;

  if(NbPnt == 0)
  {
    //-- cout << "The hatching # " << setw(3) << IndH << " has to be classified" << endl ;
    Geom2dHatch_Classifier Classifier(myElements,Hatching.ClassificationPoint(),0.0000001); 
    if(Classifier.State() == TopAbs_IN)
    { 
      HatchGen_Domain domain ;
      Hatching.AddDomain (domain) ;
    }
    
    Hatching.IsDone (Standard_True) ;
    return ;
  }
  
//for (Standard_Integer IPnt = 1 ; IPnt <= NbPnt ; IPnt++) {
  for (IPnt = 1 ; IPnt <= NbPnt ; IPnt++)
  {
    Standard_Boolean NoDomain   = Hatching.NbDomains() == 0 ; 
    Standard_Boolean FirstPoint = IPnt ==     1 ;
    Standard_Boolean LastPoint  = IPnt == NbPnt ;

    const HatchGen_PointOnHatching& CurPnt = Hatching.Point (IPnt) ;

#if TRACE_HATCHER
    cout << "===== ComputeDomains:: Hatching # " << setw(3) << IndH << " =====" << endl ;
    CurPnt.Dump (IPnt) ;
    cout << "==========================================" << endl ;
#endif
    
    
//-----------------------------------------------------------------------
// Calcul des domaines.
//-----------------------------------------------------------------------

    TopAbs_State     StateBefore  = CurPnt.StateBefore() ;
    TopAbs_State     StateAfter   = CurPnt.StateAfter() ;
    Standard_Boolean SegmentBegin = CurPnt.SegmentBeginning() ;
    Standard_Boolean SegmentEnd   = CurPnt.SegmentEnd() ;

    HatchGen_Domain domain ;

//-----------------------------------------------------------------------
// Initialisations dues au premier point.
//-----------------------------------------------------------------------

    if (FirstPoint)
    {
      SavPnt  = Standard_False ;
      ISav = 0 ;
      NbOpenedSegments = 0 ;
      if (SegmentEnd && SegmentBegin)
      {
        if (StateAfter  == TopAbs_UNKNOWN)
          StateAfter  = TopAbs_IN ;
        if (StateBefore == TopAbs_UNKNOWN)
          StateBefore = TopAbs_IN ;

        if (Segments)
        {
          SavPnt  = Standard_True ;
          ISav = 0 ;
        }
      }
      else if (SegmentEnd)
      {
        if (StateAfter == TopAbs_UNKNOWN)
          StateAfter = TopAbs_IN ;

        if (Segments)
        {
          SavPnt  = Standard_True ;
          ISav = 0 ;
        }
      }
      else if (SegmentBegin)
      {
        if (StateBefore == TopAbs_UNKNOWN)
          StateBefore = TopAbs_IN ;
        if (StateBefore == TopAbs_IN)
        {
          SavPnt  = Standard_True ;
          ISav = 0 ;
        }
      }
      else
      {
        if (StateBefore == TopAbs_IN)
        {
          SavPnt  = Standard_True ;
          ISav = 0 ;
        }
      }
    }

//-----------------------------------------------------------------------
// Initialisations dues au dernier point.
//-----------------------------------------------------------------------

    if (LastPoint)
    {
      if (SegmentEnd && SegmentBegin)
      {
        if (StateAfter  == TopAbs_UNKNOWN)
          StateAfter  = TopAbs_IN ;
        
        if (StateBefore == TopAbs_UNKNOWN)
          StateBefore = TopAbs_IN ;
      }
      else if (SegmentEnd)
      {
        if (StateAfter  == TopAbs_UNKNOWN)
          StateAfter = TopAbs_IN ;
      }
      else if (SegmentBegin)
      {
        if (StateBefore == TopAbs_UNKNOWN)
          StateBefore = TopAbs_IN ;
      }
      else
      {
      }
    }
    
//-----------------------------------------------------------------------
// Cas general.
//-----------------------------------------------------------------------

    Standard_Boolean ToAppend = Standard_False ;

    if (SegmentEnd && SegmentBegin)
    {
      if (StateBefore != TopAbs_IN && StateAfter != TopAbs_IN)
      {
        Hatching.Status (HatchGen_IncompatibleStates) ;
        return ;
      }

      if (Points)
      {
        if (Segments)
        {
          if (!SavPnt)
          {
            if(NoDomain)
            {
              Hatching.Status (HatchGen_IncoherentParity) ;
            }
            else
            { 
              Hatching.IsDone(Standard_True);
            }
            return ;
          }

          if (ISav != 0)
            domain.SetFirstPoint (Hatching.Point(ISav)) ;

          domain.SetSecondPoint (CurPnt) ;
          ToAppend = Standard_True ;
          SavPnt = Standard_True ;
          ISav = IPnt ;
        }
        else
        {
          Standard_Boolean isININ = (StateBefore == TopAbs_IN && StateAfter == TopAbs_IN);
          if (SavPnt && !isININ)
          {
            if(NoDomain)
            { 
              Hatching.Status (HatchGen_IncoherentParity) ;
            }
            else
            {
              Hatching.IsDone(Standard_True);
            }
            
            return ;
          }

          domain.SetPoints (CurPnt, CurPnt) ;
          ToAppend = Standard_True ;
          SavPnt = Standard_False ;
          ISav = 0 ;
        }
      }
    }
    else if (SegmentEnd)
    {
      if (Segments)
      {
        if (StateAfter == TopAbs_OUT)
        {
          if (!SavPnt)
          {
            if(NoDomain)
            { 
              Hatching.Status (HatchGen_IncoherentParity) ;
            }
            else
            { 
              Hatching.IsDone(Standard_True);
            }
            return ;
          }

          if (ISav != 0)
            domain.SetFirstPoint (Hatching.Point(ISav)) ;

          domain.SetSecondPoint (CurPnt) ;
          ToAppend = Standard_True ;
        }
        else
        {
          if (Points)
          {
            if (ISav != 0)
              domain.SetFirstPoint (Hatching.Point(ISav)) ;

            domain.SetSecondPoint (CurPnt) ;
            ToAppend = Standard_True ;
            SavPnt = Standard_True ;
            ISav = IPnt ;
          }
        }
      }
      else
      {
        if (StateAfter == TopAbs_IN)
        {
          SavPnt = Standard_True ;
          ISav = IPnt ;
        }
      }

      NbOpenedSegments-- ;
      
    }
    else if (SegmentBegin)
    {
      if (Segments)
      {
        if (StateBefore == TopAbs_OUT)
        {
          SavPnt = Standard_True ;
          ISav = IPnt ;
        }
        else
        {
          if (Points)
          {
            if (!SavPnt)
            {
              if(NoDomain)
              {
                Hatching.Status (HatchGen_IncoherentParity) ;
              }
              else
              {
                Hatching.IsDone(Standard_True);
              }

              return ;
            }

            if (ISav != 0)
              domain.SetFirstPoint (Hatching.Point(ISav)) ;

            domain.SetSecondPoint (CurPnt) ;
            ToAppend = Standard_True ;
            SavPnt = Standard_True ;
            ISav = IPnt ;
          }
        }
      }
      else
      {
        if (StateBefore == TopAbs_IN)
        {
          if (!SavPnt)
          {
            if(NoDomain)
            { 
              Hatching.Status (HatchGen_IncoherentParity) ;
            }
            else
            {
              Hatching.IsDone(Standard_True);
            }

            return ;
          }

          if (ISav != 0)
            domain.SetFirstPoint (Hatching.Point(ISav)) ;

          domain.SetSecondPoint (CurPnt) ;
          ToAppend = Standard_True ;

          //Modified by Sergey KHROMOV - Fri Jan  5 12:05:30 2001
          //SavPnt = Standard_False ;
          //ISav = 0 ;

          SavPnt = Standard_True ;
          ISav = IPnt ;
          //Modified by Sergey KHROMOV - Fri Jan  5 12:05:31 2001
        }
      }

      NbOpenedSegments++ ;
    }
    else
    {
      //-- ???????????????????????????????????????????????????????????????????????????
      //-- Solution provisoire (lbr le 11 Aout 97 )
      //-- si On a 2 points dont des points OUT OUT ou IN IN qui delimitent une isos
      //-- on transforme les transitions 
      if (StateBefore == TopAbs_OUT && StateAfter == TopAbs_OUT)
      {
        if(NbPnt == 2)
        {
          if(FirstPoint)
            StateAfter  = TopAbs_IN; 
          else
            StateBefore = TopAbs_IN;
        }
      }
       //-- ???????????????????????????????????????????????????????????????????????????
      if(StateBefore == TopAbs_OUT && StateAfter == TopAbs_OUT)
      {
        if (SavPnt)
        {
          if(NoDomain)
          {
            Hatching.Status (HatchGen_IncoherentParity) ;
          }
          else
          {
            Hatching.IsDone(Standard_True);
          }

          return ;
        }

        if (Points)
        {
          domain.SetPoints (CurPnt, CurPnt) ;
          ToAppend = Standard_True ;
          SavPnt = Standard_True ;
          ISav = IPnt ;
        }
      }
      else if (StateBefore == TopAbs_OUT && StateAfter == TopAbs_IN )
      {
        SavPnt = Standard_True ;
        ISav = IPnt ;
      }
      else if (StateBefore == TopAbs_IN  && StateAfter == TopAbs_OUT)
      {
        if (!SavPnt)
        {
          if(NoDomain)
          { 
            Hatching.Status (HatchGen_IncoherentParity) ;
          }
          else
          { 
            Hatching.IsDone(Standard_True);
          }

          return ;
        }

        if (ISav != 0)
          domain.SetFirstPoint (Hatching.Point(ISav));

        domain.SetSecondPoint (CurPnt) ;
        ToAppend = Standard_True ;
        SavPnt = Standard_False ;
        ISav = 0 ;
      }
      else if (StateBefore == TopAbs_IN  && StateAfter == TopAbs_IN )
      {
        if (Points)
        {
          if (NbOpenedSegments == 0)
          {
            if (!SavPnt)
            {
              if(NoDomain)
              {
                Hatching.Status (HatchGen_IncoherentParity) ;
              }
              else
              { 
                Hatching.IsDone(Standard_True);
              }

              //return;
              continue;
            }

            if (ISav != 0)
              domain.SetFirstPoint (Hatching.Point(ISav)) ;

            domain.SetSecondPoint (CurPnt) ;
            ToAppend = Standard_True ;
            SavPnt = Standard_True ;
            ISav = IPnt ;
          }
          else
          {
            if (Segments)
            {
              if (!SavPnt)
              {
                if(NoDomain)
                {
                  Hatching.Status (HatchGen_IncoherentParity) ;
                }
                else
                {
                  Hatching.IsDone(Standard_True);
                }

                return ;
              }

              if (ISav != 0)
                domain.SetFirstPoint (Hatching.Point(ISav)) ;

              domain.SetSecondPoint (CurPnt) ;
              ToAppend = Standard_True ;
              SavPnt = Standard_True ;
              ISav = IPnt ;
            }
            else
            {
              if (SavPnt)
              {
                if(NoDomain)
                {
                  Hatching.Status (HatchGen_IncoherentParity) ;
                }
                else
                {
                  Hatching.IsDone(Standard_True);
                }

                return ;
              }

              domain.SetPoints (CurPnt, CurPnt) ;
              ToAppend = Standard_True ;
              SavPnt = Standard_False ;
              ISav = 0 ;
            }
          }
        }
      }
      else
      {
        Hatching.Status (HatchGen_IncompatibleStates) ;
        return ;
      }
    }

//-----------------------------------------------------------------------
// Ajout du domaine.
//-----------------------------------------------------------------------

    if (ToAppend)
      Hatching.AddDomain (domain) ;
    
//-----------------------------------------------------------------------
// Traitement lie au dernier point.
//-----------------------------------------------------------------------

    if (LastPoint)
    {
      domain.SetPoints () ;
      ToAppend = Standard_False ;
      
      if (SegmentEnd && SegmentBegin)
      {
        if (Segments)
        {
          if (!SavPnt)
          {
            if(NoDomain)
            {
              Hatching.Status (HatchGen_IncoherentParity) ;
            }
            else
            {
              Hatching.IsDone(Standard_True);
            }

            return ;
          }

          if(ISav != 0)
            domain.SetFirstPoint (Hatching.Point(ISav)) ;

          ToAppend = Standard_True ;
        }	
      }
      else if (SegmentEnd)
      {
        if (StateAfter == TopAbs_IN)
        {
          if (!SavPnt)
          {
            if(NoDomain)
            {
              Hatching.Status (HatchGen_IncoherentParity) ;
            }
            else
            {
              Hatching.IsDone(Standard_True);
            }

            return ;
          }

          if (ISav != 0)
            domain.SetFirstPoint (Hatching.Point(ISav)) ;

          ToAppend = Standard_True ;
        }	
      }
      else if (SegmentBegin)
      {
        if (Segments)
        {
          if (!SavPnt)
          {
            if(NoDomain)
            {
              Hatching.Status (HatchGen_IncoherentParity) ;
            }
            else
            {
              Hatching.IsDone(Standard_True);
            }

            return ;
          }

          if (ISav != 0)
            domain.SetFirstPoint (Hatching.Point(ISav)) ;
          
          ToAppend = Standard_True ;
        }
      }
      else
      {
        if (StateAfter == TopAbs_IN)
        {
          if(!SavPnt)
          {
            if(NoDomain)
            {
              Hatching.Status (HatchGen_IncoherentParity) ;
            }
            else
            {
              Hatching.IsDone(Standard_True);
            }

            return ;
          }

          if (ISav != 0)
            domain.SetFirstPoint (Hatching.Point(ISav)) ;

          ToAppend = Standard_True ;
        }
      }

      if (ToAppend)
        Hatching.AddDomain (domain) ;
    }
  }

  Hatching.IsDone(Standard_True) ;
}

//=======================================================================
//=======================================================================
//  Category : Results.
//=======================================================================
//=======================================================================


//=======================================================================
// Function : Domain
// Purpose  : Returns the IDom-th domain of the IndH-th hatching.
//=======================================================================

const HatchGen_Domain& Geom2dHatch_Hatcher::Domain (const Standard_Integer IndH,
						 const Standard_Integer IDom) const
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_NoSuchObject_Raise_if (!myHatchings.IsBound (IndH), "") ;
#endif
  const Geom2dHatch_Hatching& Hatching = myHatchings.Find (IndH) ;
  StdFail_NotDone_Raise_if (!Hatching.IsDone(), "Geom2dHatch_Hatcher::Domain") ;
#if RAISE_IF_NOSUCHOBJECT
  Standard_OutOfRange_Raise_if (IDom < 1 || IDom > Hatching.NbDomains(), "") ;
#endif
  const HatchGen_Domain& Domain = Hatching.Domain (IDom) ;
  return Domain ;
}

//=======================================================================
//=======================================================================
//  Category : Dump.
//=======================================================================
//=======================================================================

//=======================================================================
// Function : Dump
// Purpose  : Dumps the hatcher.
//=======================================================================

void Geom2dHatch_Hatcher::Dump () const
{
  cout << endl ;
  cout << "========================================================" << endl ;
  cout << "=== Dump of the hatcher ================================" << endl ;
  cout << "========================================================" << endl ;
  cout << endl ;

  cout << "The points   are "
       << (myKeepPoints   ? "    " : "not ")
       << "considered."
       << endl ;
  cout << "The segments are "
       << (myKeepSegments ? "    " : "not ")
       << "considered."
       << endl ;
  cout << "2D Confusion tolerance : " << myConfusion2d << endl ;
  cout << "3D Confusion tolerance : " << myConfusion3d << endl ;
  
  cout << myNbHatchings
       << " hatching"
       << ((myNbHatchings == 1) ? "" : "s")
       << endl ;
  cout << myNbElements
       << " element"
       << ((myNbElements  == 1) ? "" : "s")
       << endl ;
  
  cout << endl ;
  cout << "========================================================" << endl ;
  cout << "=== Hatchings ==========================================" << endl ;
  cout << "========================================================" << endl ;
  cout << endl ;
  
  for (Standard_Integer IndH = 1 ; IndH <= myNbHatchings ; IndH++) {
    cout << "Hatching # " << IndH ;
    if (!myHatchings.IsBound (IndH)) {
      cout << " is not bound" << endl ;
    } else {
      const Geom2dHatch_Hatching& Hatching = myHatchings.Find (IndH) ;
      Standard_Integer NbPnt = Hatching.NbPoints() ;
      cout << " contains " << NbPnt << " restriction points :"  << endl ;
      for (Standard_Integer IPnt = 1 ; IPnt <= NbPnt ; IPnt++) {
	const HatchGen_PointOnHatching& PntH = Hatching.Point (IPnt) ;
        PntH.Dump (IPnt) ;
      }
      cout << "----------------------------------------------" << endl ;
    }
  }

  cout << endl ;
  cout << "========================================================" << endl ;
  cout << "=== Elements ===========================================" << endl ;
  cout << "========================================================" << endl ;
  cout << endl ;
  
  for (Standard_Integer IndE = 1 ; IndE <= myNbElements ; IndE++) {
    cout << "Element # " << IndE ;
    if (!myElements.IsBound (IndE)) {
      cout << " is not bound" << endl ;
    } else {
      const Geom2dHatch_Element& Element = myElements.Find (IndE) ;
      switch (Element.Orientation()) {
        case TopAbs_FORWARD  : cout << " is FORWARD"  << endl ; break ;
        case TopAbs_REVERSED : cout << " is REVERSED" << endl ; break ;
        case TopAbs_INTERNAL : cout << " is INTERNAL" << endl ; break ;
        case TopAbs_EXTERNAL : cout << " is EXTERNAL" << endl ; break ;
      }
    }
  }

  cout << endl ;
}
