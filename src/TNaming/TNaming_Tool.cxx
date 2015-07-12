// Created on: 1997-01-06
// Created by: Yves FRICAUD
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


#include <BRep_Builder.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TDF_AttributeMap.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelList.hxx>
#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TDF_Tool.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_ListIteratorOfListOfNamedShape.hxx>
#include <TNaming_ListOfNamedShape.hxx>
#include <TNaming_Localizer.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_RefShape.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_UsedShapes.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : LastModif 
//purpose  : 
//=======================================================================
static void LastModif(      TNaming_NewShapeIterator& it,
		      const TopoDS_Shape&             S,
		            TopTools_IndexedMapOfShape& MS,
		      const TDF_LabelMap&             Updated,
		            TDF_LabelList&            Deleted)
{ 
  Standard_Boolean YaModif = Standard_False;
  for (; it.More(); it.Next()) {
    const TDF_Label&    Lab = it.Label();
    if (!Updated.Contains(Lab)) continue;
    
    if (it.IsModification()) {
      YaModif = Standard_True;
      TNaming_NewShapeIterator it2(it);
      if (!it2.More()) {
	const TopoDS_Shape& S1   = it.Shape();
	if (S1.IsNull()) {
	  Deleted.Append(Lab);
	}
	else {
	  MS.Add (S1);  // Modified
	}
      }
      else
	LastModif(it2,it.Shape(),MS,Updated,Deleted);
    }
  }
  if (!YaModif) 
    MS.Add(S);  
}

//=======================================================================
//function : LastModif 
//purpose  : 
//=======================================================================

static void LastModif(TNaming_NewShapeIterator& it,
		      TopTools_IndexedMapOfShape& MS,
		      const TopoDS_Shape&       S,
		      TDF_LabelList&            Deleted)
{
  Standard_Boolean YaModif = Standard_False;
  for (; it.More(); it.Next()) {
    const TDF_Label&    Lab = it.Label();
    if (it.IsModification()) {
      YaModif = Standard_True;
      TNaming_NewShapeIterator it2(it);
      if (!it2.More()) {
	const TopoDS_Shape& S1   = it.Shape();
	if (S1.IsNull()) {
	  Deleted.Append(Lab);
	}
	else {
	  MS.Add (S1);  // Modified
	}
      }
      else 
	LastModif(it2,MS,it.Shape(), Deleted);
    }
  }
  if (!YaModif) 
    MS.Add(S);
}

//=======================================================================
//function : MakeShape
//purpose  : 
//=======================================================================

static TopoDS_Shape MakeShape (const TopTools_IndexedMapOfShape& MS) 
{  
  if (!MS.IsEmpty ()) {
    if (MS.Extent() == 1) {
      return MS (1);
    }
    else {
      TopoDS_Compound C;
      BRep_Builder B;
      B.MakeCompound(C);
      for (Standard_Integer anItMS = 1; anItMS <= MS.Extent(); ++anItMS) {
        B.Add (C, MS (anItMS));
      }
      return C;
    }
  }
  return TopoDS_Shape();  
}

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================
TopoDS_Shape TNaming_Tool::GetShape(const Handle(TNaming_NamedShape)& NS)
{  
  TNaming_Iterator itL (NS);
  TopTools_IndexedMapOfShape MS;
  if(NS->Evolution() == TNaming_SELECTED) {
    for (; itL.More(); itL.Next()) {
      if(!itL.NewShape().IsNull()) {
		if(itL.NewShape().ShapeType() != TopAbs_VERTEX ) { //OR-N
	      Handle (TNaming_Naming)  aNaming;
	      NS->Label().FindAttribute(TNaming_Naming::GetID(), aNaming);
	      if(!aNaming.IsNull()) {
		    if(aNaming->GetName().Orientation() == TopAbs_FORWARD ||
				aNaming->GetName().Orientation() == TopAbs_REVERSED) {
		      TopoDS_Shape aS = itL.NewShape();
			  if(aNaming->GetName().Type() == TNaming_ORIENTATION) {
			    aS.Orientation(aNaming->GetName().Orientation());
			  } else {
				Handle (TNaming_Naming)  aNaming2;
				TDF_ChildIterator it(aNaming->Label());
				for(;it.More();it.Next()) {
				  const TDF_Label& aLabel = it.Value();
				  aLabel.FindAttribute(TNaming_Naming::GetID(), aNaming2);
	              if(!aNaming2.IsNull()) {
				    if(aNaming2->GetName().Type() == TNaming_ORIENTATION) {
					  aS.Orientation(aNaming2->GetName().Orientation());
					  break;
					}
				  }
				}
			  }
		      MS.Add(aS);
			} else  
			    MS.Add(itL.NewShape());
		  } else
	          MS.Add(itL.NewShape());		
		} //
		else
	        MS.Add(itL.NewShape());
	  }
	}  
  } else 
    for (; itL.More(); itL.Next()) {
      if (!itL.NewShape().IsNull()) MS.Add(itL.NewShape());      
    }
  return MakeShape(MS);
}

//=======================================================================
//function : InitialShape
//purpose  : 
//=======================================================================

TopoDS_Shape TNaming_Tool::OriginalShape (const Handle(TNaming_NamedShape)& NS)
{
  TNaming_Iterator itL (NS);
  TopTools_IndexedMapOfShape MS;
  for (; itL.More(); itL.Next()) {
    MS.Add(itL.OldShape());
  }
  return MakeShape(MS);
}  

//=======================================================================
static void ApplyOrientation (TopTools_IndexedMapOfShape& MS, 
			      const TopAbs_Orientation OrientationToApply)
{
  for (Standard_Integer anItMS = 1; anItMS <= MS.Extent(); ++anItMS)
  {
    MS.Substitute (anItMS, MS (anItMS).Oriented (OrientationToApply));
  }
}

//=======================================================================
//function : CurrentShape
//purpose  : 
//=======================================================================

TopoDS_Shape TNaming_Tool::CurrentShape(const Handle(TNaming_NamedShape)& Att)
{ 
  TopTools_IndexedMapOfShape MS;
  TDF_LabelList Deleted;

  TNaming_Iterator itL (Att);
  for (; itL.More(); itL.Next()) {
    const TopoDS_Shape& S = itL.NewShape();
    if (S.IsNull()) continue;
//OR-N
    Standard_Boolean YaOrientationToApply(Standard_False);
    TopAbs_Orientation OrientationToApply(TopAbs_FORWARD);
    if(Att->Evolution() == TNaming_SELECTED) {
      if (itL.More() && itL.NewShape().ShapeType() != TopAbs_VERTEX) {		 	
		Handle (TNaming_Naming)  aNaming;
		Att->Label().FindAttribute(TNaming_Naming::GetID(), aNaming);
		if(!aNaming.IsNull()) {
		  if(aNaming->GetName().Type() == TNaming_ORIENTATION) {
			 OrientationToApply = aNaming->GetName().Orientation();
		  } else {
	        Handle (TNaming_Naming)  aNaming2;
			TDF_ChildIterator it(aNaming->Label());
			for(;it.More();it.Next()) {
			  const TDF_Label& aLabel = it.Value();
			  aLabel.FindAttribute(TNaming_Naming::GetID(), aNaming2);
	          if(!aNaming2.IsNull()) {
			    if(aNaming2->GetName().Type() == TNaming_ORIENTATION) {
				  OrientationToApply = aNaming2->GetName().Orientation();
				  break;
				}
			  }
			}
		  }			
		  if(OrientationToApply == TopAbs_FORWARD || OrientationToApply == TopAbs_REVERSED)
			YaOrientationToApply = Standard_True;
		}
	  }
	} //
    TNaming_NewShapeIterator it(itL);
    if (!it.More()) {
	MS.Add(S);
    }
    else {
//      LastModif(it, MS, S, Deleted);
      TopTools_IndexedMapOfShape MS2;  // to be optimized later
      LastModif(it, MS2, S, Deleted);
      if (YaOrientationToApply) ApplyOrientation (MS2, OrientationToApply);
      for (Standard_Integer anItMS2 = 1; anItMS2 <= MS2.Extent(); ++anItMS2)
        MS.Add (MS2 (anItMS2));
    }
  }
  return MakeShape (MS);
}

//=======================================================================
//function : CurrentShape
//purpose  : 
//=======================================================================
TopoDS_Shape TNaming_Tool::CurrentShape(const Handle(TNaming_NamedShape)& Att,
					const TDF_LabelMap&           Updated)
{  
  TDF_Label Lab = Att->Label();

  TopTools_IndexedMapOfShape MS;
  TDF_LabelList Deleted;
  
  if (!Updated.Contains(Lab)) {
    return TopoDS_Shape();
  }

  TNaming_Iterator itL (Att);
  for (; itL.More(); itL.Next()) {
    const TopoDS_Shape& S = itL.NewShape();
    if (S.IsNull()) continue;
//OR-N
    Standard_Boolean YaOrientationToApply(Standard_False);
    TopAbs_Orientation OrientationToApply(TopAbs_FORWARD);
    if(Att->Evolution() == TNaming_SELECTED) {
      if (itL.More() && itL.NewShape().ShapeType() != TopAbs_VERTEX) {
		Handle (TNaming_Naming)  aNaming;
		Att->Label().FindAttribute(TNaming_Naming::GetID(), aNaming);
		if(!aNaming.IsNull()) {
		  if(aNaming->GetName().Type() == TNaming_ORIENTATION) {
			 OrientationToApply = aNaming->GetName().Orientation();
		  } else {
	        Handle (TNaming_Naming)  aNaming2;
			TDF_ChildIterator it(aNaming->Label());
			for(;it.More();it.Next()) {
			  const TDF_Label& aLabel = it.Value();
			  aLabel.FindAttribute(TNaming_Naming::GetID(), aNaming2);
	          if(!aNaming2.IsNull()) {
			    if(aNaming2->GetName().Type() == TNaming_ORIENTATION) {
				  OrientationToApply = aNaming2->GetName().Orientation();
				  break;
				}
			  }
			}
		  }
		  if(OrientationToApply == TopAbs_FORWARD || OrientationToApply == TopAbs_REVERSED)
			YaOrientationToApply = Standard_True;	    	
		}
      }
    } //
    TNaming_NewShapeIterator it(itL);
    if (!it.More()) {
      MS.Add(S);
    }
    else {
//      LastModif(it, S, MS, Updated, Deleted);
      TopTools_IndexedMapOfShape MS2; // to be optimized later
      LastModif(it, S, MS2, Updated, Deleted);
      if (YaOrientationToApply) ApplyOrientation (MS2, OrientationToApply);
      for (Standard_Integer anItMS2 = 1; anItMS2 <= MS2.Extent(); ++anItMS2)
        MS.Add(MS2 (anItMS2));
    }
  }
  return MakeShape (MS);
}

//=======================================================================
//function : CurrentNamedShape
//purpose  : 
//=======================================================================
Handle(TNaming_NamedShape) TNaming_Tool::CurrentNamedShape(const Handle(TNaming_NamedShape)& Att,
							   const TDF_LabelMap&           Updated)
{
  TopoDS_Shape CS = CurrentShape(Att,Updated);
  if (CS.IsNull()) {
    Handle(TNaming_NamedShape) NS;
    return NS;
  }
  return NamedShape(CS,Att->Label());
}

//=======================================================================
//function : CurrentNamedShape
//purpose  : 
//=======================================================================
Handle(TNaming_NamedShape) TNaming_Tool::CurrentNamedShape(const Handle(TNaming_NamedShape)& Att)

{
  TopoDS_Shape CS = CurrentShape(Att);
  if (CS.IsNull()) {
    Handle(TNaming_NamedShape) NS;
    return NS;
  }
  return NamedShape(CS,Att->Label());
}

//=======================================================================
//function : FindModifUntil
//purpose  : 
//=======================================================================

static void FindModifUntil (TNaming_NewShapeIterator&         it,
			    TopTools_IndexedMapOfShape& MS,
			    const Handle(TNaming_NamedShape)& Context)
{ 
  for (; it.More(); it.Next()) {
    if (!it.Shape().IsNull()) {
      if (it.NamedShape() == Context) {
	MS.Add(it.Shape());
      }
      else {
	TNaming_NewShapeIterator it2(it);
	FindModifUntil (it2,MS,Context);
      }
    }
  }
}

//=======================================================================
//function : GeneratedShape
//purpose  : 
//=======================================================================
TopoDS_Shape TNaming_Tool::GeneratedShape(const TopoDS_Shape&               S,
					  const Handle(TNaming_NamedShape)& Generation)

{  
  TopTools_IndexedMapOfShape MS;
  Handle(TNaming_UsedShapes) US;
  Generation->Label().Root().FindAttribute(TNaming_UsedShapes::GetID(),US);
  
  for (TNaming_NewShapeIterator it(S,US);  it.More(); it.Next()) {
    if (!it.Shape().IsNull() && it.NamedShape() == Generation) {
      MS.Add(it.Shape());
    }
  }
  if (MS.IsEmpty()) {
    TNaming_NewShapeIterator it2(S,US);
    FindModifUntil (it2,MS,Generation);
  }
  return MakeShape (MS);
}

//=======================================================================
//function : FirstOlds
//purpose  : 
//=======================================================================
void TNaming_Tool::FirstOlds (const Handle(TNaming_UsedShapes)& US,
			      const TopoDS_Shape&         S,
			      TNaming_OldShapeIterator&  it,
			      TopTools_IndexedMapOfShape& MS,
			      TDF_LabelList&          Labels) 
{  
  Standard_Integer TransDef;
  Standard_Boolean YaModif = 0;

  for (; it.More(); it.Next()) {
    if (it.IsModification()) {
      YaModif = 1;
      TNaming_OldShapeIterator it2(it);
      if (!it2.More()) { 
	Labels.Append(TNaming_Tool::Label(US,it.Shape(),TransDef));
	MS.Add (it.Shape()); 
      }
      else {
	FirstOlds(US,it.Shape(),it2,MS,Labels);
      }
    }
  }
  if (!YaModif) MS.Add(S);
}

//=======================================================================
//function : InitialShape
//purpose  : 
//=======================================================================

TopoDS_Shape TNaming_Tool::InitialShape(const TopoDS_Shape&  S,
					const TDF_Label&     Acces,
					TDF_LabelList&       Labels) 
{
  Handle(TNaming_UsedShapes) US;
  Acces.Root().FindAttribute(TNaming_UsedShapes::GetID(),US);
  TopoDS_Shape Res;

  if (!TNaming_Tool::HasLabel(US,S)) return Res;

  Standard_Integer Transdef;
  Label(US,S,Transdef); 
  TopTools_IndexedMapOfShape MS;
  TNaming_OldShapeIterator it(S,Transdef,US);
  if (!it.More()) {
    return S;
  }
  else {
    FirstOlds(US,S,it,MS,Labels);
  }
  return MakeShape(MS);
}



//=======================================================================
//function : Back
//purpose  : 
//=======================================================================

static void Back (const Handle(TNaming_NamedShape)& NS,
		        TNaming_MapOfNamedShape&    MNS)
{  
  for (TNaming_Iterator it(NS); it.More(); it.Next()) {
    if (it.NewShape().IsNull()) continue;
    for (TNaming_OldShapeIterator Oldit(it); Oldit.More(); Oldit.Next()) {
      const TopoDS_Shape& OS = Oldit.Shape();
      if (!OS.IsNull()) {
	Handle(TNaming_NamedShape) NOS = TNaming_Tool::NamedShape(OS,NS->Label());
	// Continue de remonter
	if (!NOS.IsNull()) { 
	  if (MNS.Add(NOS)) Back (NOS,MNS);
	}
      }
    }
  }
}

//=======================================================================
//function : Collect
//purpose  : 
//=======================================================================

void TNaming_Tool::Collect(const Handle(TNaming_NamedShape)& NS,
			         TNaming_MapOfNamedShape&    MNS,
			   const Standard_Boolean            OnlyModif)
{
  MNS.Add(NS);
  Back   (NS,MNS);

  for (TNaming_Iterator it(NS); it.More(); it.Next()) {
    if  (it.NewShape().IsNull()) continue;
    for (TNaming_NewShapeIterator NewIt(it); NewIt.More(); NewIt.Next()) {
      if (!OnlyModif || NewIt.IsModification()) {
	// Continue la descente
	Collect(NewIt.NamedShape(),MNS,OnlyModif);
      }
    }
  }
}


// Pour DEBUGGER 
#ifdef OCCT_DEBUG

//=======================================================================
//function : TNamingTool_Label
//purpose  : 
//=======================================================================
void TNamingTool_DumpLabel (const TopoDS_Shape& S,
			    const TDF_Label&    Acces)
{
  Handle(TNaming_NamedShape) NS = TNaming_Tool::NamedShape(S,Acces);
  NS->Label().EntryDump(cout);
  cout<<endl;
}

#include <BRepTools.hxx>
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void TNamingTool_Write (const TopoDS_Shape& S,
			const Standard_CString File)
{
  BRepTools::Write (S,File);  
}

#endif


//=======================================================================
//function : FindShape
//purpose  : ANaming
//=======================================================================

void TNaming_Tool::FindShape(const TDF_LabelMap&               Valid,
			     const TDF_LabelMap&               /*Forbiden*/,
			     const Handle(TNaming_NamedShape)& Arg,
			     TopoDS_Shape&                     S)
{
  if (!Valid.IsEmpty() && !Valid.Contains(Arg->Label())) return;
  if (Arg.IsNull() || Arg->IsEmpty()) return;
  
  // Which type of shape is being expected?
  Handle(TNaming_Naming) aNaming;
  if (!Arg->FindAttribute(TNaming_Naming::GetID(), aNaming)) {
#ifdef OCCT_DEBUG
//    cout<<"TNaming_Tool::FindShape(): Naming attribute hasn't been found attached at the Argument label"<<endl;
#endif
    return;
  }

  // Looking for sub shapes of the result shape
  TopTools_MapOfShape subShapes;
  TopExp_Explorer anExpl(Arg->Get(), (TopAbs_ShapeEnum)((int)(aNaming->GetName().ShapeType()) + 1));
  for (; anExpl.More(); anExpl.Next()) subShapes.Add(anExpl.Current());
#ifdef OCCT_DEBUG
//  cout<<"TNaming_Tool::FindShape(): Nb of sub shapes = "<<subShapes.Extent()<<endl;
#endif

  // Looking for external arguments:
  TNaming_ListOfNamedShape extArgs;
  TDF_AttributeMap outRefs;
  TDF_Tool::OutReferences(Arg->Label(), outRefs);
  if (outRefs.IsEmpty()) {
#ifdef OCCT_DEBUG
//    cout<<"TNaming_Tool::FindShape(): No out references have been found"<<endl;
#endif
    return;
  }
  for (TDF_MapIteratorOfAttributeMap itr(outRefs); itr.More(); itr.Next()) {
    if (itr.Key()->DynamicType() == STANDARD_TYPE(TNaming_NamedShape)) {
#ifdef OCCT_DEBUG
//      Standard_Integer nbExtArgs = extArgs.Extent();
#endif
      Handle(TNaming_NamedShape) anExtArg (Handle(TNaming_NamedShape)::DownCast(itr.Key()));
      const Handle(TNaming_NamedShape)& aCurrentExtArg = TNaming_Tool::CurrentNamedShape(anExtArg);
      if (!aCurrentExtArg.IsNull() && !aCurrentExtArg->IsEmpty())
	extArgs.Append(aCurrentExtArg);
#ifdef OCCT_DEBUG
//      if (extArgs.Extent() - 1 == nbExtArgs) {
//	cout<<"TNaming_Tool::FindShape(): An external reference has been found at ";
//	itr.Key()->Label().EntryDump(cout); cout<<endl;
//      }
#endif
    }
  }

  // The iterator on external arguments:
  TNaming_ListIteratorOfListOfNamedShape extArgsIterator (extArgs);
  for(; extArgsIterator.More(); extArgsIterator.Next()) {
    Handle(TNaming_NamedShape) anExtArg = extArgsIterator.Value();

    // Looking for context:
    Handle(TNaming_NamedShape) aContextNS;
    if (anExtArg->Label().Father().IsNull() ||
	!anExtArg->Label().Father().FindAttribute(TNaming_NamedShape::GetID(), aContextNS)) {
      aContextNS = anExtArg;
// #ifdef OCCT_DEBUG
//       cout<<"TNaming_Tool::FindShape(): A context shape hasn't been found at the father label of the external argument"<<endl;
// #endif
//       continue;
    }

#ifdef OCCT_DEBUG
//    cout<<"TNaming_Tool::FindShape(): Searching in the external reference ";
//    aContextNS->Label().EntryDump(cout); cout<<"  ";
#endif
    
    // Lets find the sub shape of the context which coincides with our sub shapes (subShapes map):
    TopExp_Explorer explC(aContextNS->Get(), aNaming->GetName().ShapeType()), explSubC;
    for (; explC.More(); explC.Next()) {
      Standard_Integer  DoesCoincide = 0;
      const TopoDS_Shape& possibleResult = explC.Current();
      TopTools_MapOfShape subShapesOfResult;
      for (explSubC.Init(possibleResult, (TopAbs_ShapeEnum)((int)(aNaming->GetName().ShapeType()) + 1)); 
	   explSubC.More(); explSubC.Next()) {
	subShapesOfResult.Add(explSubC.Current());
      }
      if (subShapesOfResult.Extent() != subShapes.Extent()) continue;
      for (TopTools_MapIteratorOfMapOfShape itrR(subShapesOfResult); itrR.More(); itrR.Next()) {
	for (TopTools_MapIteratorOfMapOfShape itr1(subShapes); itr1.More(); itr1.Next()) {
	  if (itrR.Key().IsSame(itr1.Key())) {
	    DoesCoincide++; //cout<<".";
	    break;
	  }
	}
      }
      if (DoesCoincide == subShapes.Extent()) {
#ifdef OCCT_DEBUG
//	cout<<"TNaming_Tool::FindShape(): Found! ";
#endif
	S = possibleResult;
	break;
      }
    }

    if (!S.IsNull()) break;
#ifdef OCCT_DEBUG
//    cout<<endl;
#endif

  }

#ifdef OCCT_DEBUG
  if (S.IsNull()) {
    cout<<"TNaming_Tool::FindShape(): There hasn't been found a sub shape of the context shape coinciding with the sub shapes of naming"<<endl;
  }
#endif
      
}
