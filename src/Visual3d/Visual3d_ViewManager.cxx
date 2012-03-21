// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


/***********************************************************************

     FONCTION :
     ----------
        Classe Visual3d_ViewManager.cxx :

	Declaration of variables specific to visualisers

     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      Mars 1992 : NW,JPB,CAL ; Creation.
      19-06-96  : FMN ; Suppression variables inutiles
      04-02-97  : FMN ; Suppression de PSOutput, XWDOutput ...
      06-05-97  : CAL ; Ajout du Clear sur les TOS_COMPUTED.
      19-09-97  : CAL ; Remplacement de Window->Position par Window->Size;
      24-10-97  : CAL ; Retrait de DownCast.
      20-11-97  : CAL ; Disparition de la dependance avec math
      01-12-97  : CAL ; Retrait du test IsActive sur l'Update et le Redraw
      31-12-97  : CAL ; Disparition de MathGra 
      16-01-98  : CAL ; Ajout du SetTransform sur une TOS_COMPUTED
      11-03-98  : CAL ; Visual3d_ViewManager::Remove ()
      20-05-98  : CAL ; Perfs. Connection entre structures COMPUTED.
      10-06-98  : CAL ; Modification des signatures de xxProjectRaster.
      10-06-98  : CAL ; Modification de la signature de ViewExists.
      01-12-98  : CAL ; S4062. Ajout des layers.
      02-12-98  : CAL ; Remove () ne detruit plus les vues.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
 * Constants
 */

#define NO_DOWNCAST
#define NO_DESTROY

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

// for the class
#include <Visual3d_ViewManager.ixx>
#include <Visual3d_ViewManager.pxx>

#include <Standard_ErrorHandler.hxx>

#include <Aspect.hxx>
#include <Aspect_IdentDefinitionError.hxx>

#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>

#include <Visual3d_PickPath.hxx>
#include <Visual3d_SetIteratorOfSetOfView.hxx>

#ifndef WNT
# include <Xw_Window.hxx>
#else
# include <WNT_Window.hxx>
#endif  // WNT

//-Aliases

//-Global data definitions

//	-- les vues definies
//	MyDefinedView		:	SetOfView;

//	-- le generateur d'identificateurs de vues
//	MyViewGenId		: 	GenId;

//-Constructors

Visual3d_ViewManager::Visual3d_ViewManager (const Handle(Aspect_GraphicDevice)& aDevice):
Graphic3d_StructureManager (aDevice),
MyDefinedView (),
MyViewGenId (View_IDMIN+((View_IDMIN+View_IDMAX)/(Visual3d_ViewManager::Limit ()))*(Visual3d_ViewManager::CurrentId ()-1),View_IDMIN+((View_IDMIN+View_IDMAX)/(Visual3d_ViewManager::Limit ()))*Visual3d_ViewManager::CurrentId ()-1),
MyZBufferAuto (Standard_False),
MyTransparency (Standard_False)
{
  // default layer is always presented in display layer sequence
  // it can not be removed
  myLayerIds.Add (0);
  myLayerSeq.Append (0);

  Handle(Aspect_GraphicDriver) agd = aDevice->GraphicDriver ();

  MyGraphicDriver = *(Handle(Graphic3d_GraphicDriver) *) &agd;
}

//-Destructors

void Visual3d_ViewManager::Destroy () {

#ifdef DESTROY
	cout << "Visual3d_ViewManager::Destroy (" << MyId << ")\n" << flush;
#endif

	Remove ();
}

//-Methods, in order

void Visual3d_ViewManager::Remove () {

#ifdef DESTROY
	cout << "Visual3d_ViewManager::Remove (" << MyId << ")\n" << flush;
#endif

	//
	// Destroy all defined views
	//

#ifdef DESTROY
	cout << "The Manager " << MyId << " have " << Length << " defined views\n";
	cout << flush;
#endif
 
	MyDefinedView.Clear ();

}

void Visual3d_ViewManager::ChangeDisplayPriority (const Handle(Graphic3d_Structure)& AStructure, const Standard_Integer OldPriority, const Standard_Integer NewPriority) {

#ifdef TRACE
	cout << "Visual3d_ViewManager::ChangeDisplayPriority ("
	     << AStructure->Identification ()
	     << ", " << OldPriority << ", " << NewPriority << ")\n";
	cout << flush;
#endif

	//
	// Change structure priority in all defined views
	//
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->ChangeDisplayPriority
			(AStructure, OldPriority, NewPriority);

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::ReCompute (const Handle(Graphic3d_Structure)& AStructure) { 

  //Standard_Integer LengthD	= MyDisplayedStructure.Extent() ();

  // Even if physically the structure cannot
  // be displayed (pb of visualisation type)
  // it has status Displayed.
 
  if (!MyDisplayedStructure.Contains(AStructure))
    return;

  //
  // Recompute structure in all activated views
  //
  Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
  while (MyIterator.More ()) {
    (MyIterator.Value ())->ReCompute (AStructure);

    // MyIterator.Next () is located on the next view
    MyIterator.Next ();
  }
  
}

void Visual3d_ViewManager::ReCompute (const Handle(Graphic3d_Structure)& AStructure, 
				      const Handle(Graphic3d_DataStructureManager)& AProjector) 
{ 

  if (! AProjector->IsKind (STANDARD_TYPE (Visual3d_View))) return;

#ifdef DOWNCAST
  Handle(Visual3d_View) theView = Handle(Visual3d_View)::DownCast (AProjector);
#else
  Handle(Visual3d_View) theView = *(Handle(Visual3d_View) *) &AProjector;
#endif
  Standard_Integer ViewId = theView->Identification ();

  // Even if physically the structure cannot
  // be displayed (pb of visualisation type)
  // it has status Displayed.
  if (!MyDisplayedStructure.Contains(AStructure))
    return;
	
  //
  // Recompute structure in all activated views
  //
  Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
  while (MyIterator.More ()) {
    if ((MyIterator.Value ())->Identification () == ViewId)
      theView->ReCompute (AStructure);

    // MyIterator.Next () is located on the next view
    MyIterator.Next ();
  }

}

void Visual3d_ViewManager::Clear (const Handle(Graphic3d_Structure)& AStructure, const Standard_Boolean WithDestruction) {

	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Clear (AStructure, WithDestruction);

		// MyIterator.Next ()  is located on the next view
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::Connect (const Handle(Graphic3d_Structure)& AMother, const Handle(Graphic3d_Structure)& ADaughter) {

	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Connect (AMother, ADaughter);

		// MyIterator.Next ()  is located on the next view
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::Disconnect (const Handle(Graphic3d_Structure)& AMother, const Handle(Graphic3d_Structure)& ADaughter) {

	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Disconnect (AMother, ADaughter);

		// MyIterator.Next ()  is located on the next view
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::Display (const Handle(Graphic3d_Structure)& AStructure) { 


 // Even if physically the structure cannot
  // be displayed (pb of visualisation type)
  // it has status Displayed.

  MyDisplayedStructure.Add(AStructure);

	//
	// Display structure in all activated views
	//
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Display (AStructure);

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::Erase (const Handle(Graphic3d_Structure)& AStructure) {


// Even if physically the structure cannot
  // be displayed (pb of visualisation type)
  // it has status Displayed.

 MyDisplayedStructure.Remove(AStructure);



	//
	// Erase structure in all defined views
	//
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Erase (AStructure);

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

	MyHighlightedStructure.Remove (AStructure);
	MyVisibleStructure.Remove (AStructure);
	MyPickStructure.Remove (AStructure);

}

void Visual3d_ViewManager::Erase () {

 Graphic3d_MapIteratorOfMapOfStructure it( MyDisplayedStructure);
 
 for (; it.More(); it.Next()) {
   Handle(Graphic3d_Structure) SG = it.Key();
   SG->Erase();
 }

}

void Visual3d_ViewManager::Highlight (const Handle(Graphic3d_Structure)& AStructure, const Aspect_TypeOfHighlightMethod AMethod) {

  MyHighlightedStructure.Add(AStructure);

	//
	// Highlight in all activated views
	//
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Highlight (AStructure, AMethod);

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::SetTransform (const Handle(Graphic3d_Structure)& AStructure, const TColStd_Array2OfReal& ATrsf) {

	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->SetTransform (AStructure, ATrsf);

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::UnHighlight () {

  Graphic3d_MapIteratorOfMapOfStructure it(MyHighlightedStructure);
  
  for (; it.More(); it.Next()) {
    Handle(Graphic3d_Structure) SG = it.Key();
    SG->UnHighlight ();
  }


}

void Visual3d_ViewManager::UnHighlight (const Handle(Graphic3d_Structure)& AStructure) {

  MyHighlightedStructure.Remove(AStructure);


	//
	// UnHighlight in all activated views
	//
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->UnHighlight (AStructure);

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::Redraw () const {

Standard_Integer MaxDx, MaxDy;
Standard_Integer Dx, Dy;
	MaxDx = MaxDy = IntegerFirst ();

	//
	// Redraw all activated views
	//
	Standard_Integer j = MyDefinedView.Extent ();
	if (j == 0) return;
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	if (! MyUnderLayer.IsNull () || ! MyOverLayer.IsNull ()) {
	    while (MyIterator.More ()) {
		(MyIterator.Value ())->Window ()->Size (Dx, Dy);
		if (Dx > MaxDx) MaxDx = Dx;
		if (Dy > MaxDy) MaxDy = Dy;

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	    }
	    if (! MyUnderLayer.IsNull ())
		MyUnderLayer->SetViewport (MaxDx, MaxDy);
	    if (! MyOverLayer.IsNull ())
		MyOverLayer->SetViewport (MaxDx, MaxDy);
	}
 
	if (! MyUnderLayer.IsNull () || ! MyOverLayer.IsNull ())
	    MyIterator.Initialize (MyDefinedView);
	while (MyIterator.More ()) {
	    (MyIterator.Value ())->Redraw (MyUnderLayer, MyOverLayer);

	    // MyIterator.Next () is located on the next view
	    MyIterator.Next ();
	}

}

void Visual3d_ViewManager::Update () const {

	//
	// Update all activated views
	//
	Standard_Integer j = MyDefinedView.Extent ();
	if (j == 0) return;
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Update (MyUnderLayer, MyOverLayer);

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

}

Handle(Visual3d_HSetOfView) Visual3d_ViewManager::ActivatedView () const {

Handle (Visual3d_HSetOfView) SG = new Visual3d_HSetOfView ();

Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);

	while (MyIterator.More ()) {
		if ((MyIterator.Value ())->IsActive ())
			SG->Add (MyIterator.Value ());

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

	return (SG);

}

#ifdef IMPLEMENTED
Standard_Boolean Visual3d_ViewManager::ContainsComputedStructure () const {

Standard_Boolean Result = Standard_False;

	//
	// Check all activated views
	//
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);

	Standard_Integer i	= MyDefinedView.Extent ();

	while ((! Result) && (MyIterator.More ())) {
		if ((MyIterator.Value ())->IsActive ())
			Result	=
			(MyIterator.Value ())->ContainsComputedStructure ();

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

	return Result;
}
#endif

Handle(Visual3d_HSetOfView) Visual3d_ViewManager::DefinedView () const {

Handle (Visual3d_HSetOfView) SG = new Visual3d_HSetOfView ();

Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);

	while (MyIterator.More ()) {
		SG->Add (MyIterator.Value ());

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

	return (SG);

}

void Visual3d_ViewManager::ConvertCoord (const Handle(Aspect_Window)& AWindow, const Graphic3d_Vertex& AVertex, Standard_Integer& AU, Standard_Integer& AV) const {

// Convert only if the data is correct
Standard_Boolean Exist;
Graphic3d_CView  TheCView;
//Graphic3d_Vertex Point;

TColStd_Array2OfReal Ori_Matrix (0,3,0,3);
TColStd_Array2OfReal Map_Matrix (0,3,0,3);

Standard_Integer Width, Height;
Standard_Real AX, AY, AZ;
Standard_Real Dx, Dy, Ratio;

	Exist	= ViewExists (AWindow, TheCView);

	if (! Exist) {
		AU = AV = IntegerLast ();
	}
	else {
    // NKV - 11.02.08 - Use graphic driver functions
    Standard_Boolean Result;

    AVertex.Coord (AX, AY, AZ);

    Result = MyGraphicDriver->ProjectRaster (TheCView, 
      Standard_ShortReal (AX), Standard_ShortReal (AY), Standard_ShortReal (AZ),
      AU, AV);

    // the old code
    if (!Result) {

      Standard_Real PtX, PtY, PtZ, PtT;
      Standard_Real APX, APY, APZ;
      Standard_Real APT;

      Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);

      Standard_Integer stop = 0;

      while ((! stop) && (MyIterator.More ())) {
	if (TheCView.ViewId ==
	    (MyIterator.Value ())->Identification ()) {
	  Ori_Matrix	=
	    (MyIterator.Value ())->MatrixOfOrientation ();
	  Map_Matrix	=
	    (MyIterator.Value ())->MatrixOfMapping ();
	  stop	= 1;
	}

	// MyIterator.Next () is located on the next view
	MyIterator.Next ();
      }


      // WCS -> View Reference Coordinate Space
      PtX	= Ori_Matrix (0, 0) * AX
	        + Ori_Matrix (0, 1) * AY
	        + Ori_Matrix (0, 2) * AZ
	        + Ori_Matrix (0, 3);
      PtY	= Ori_Matrix (1, 0) * AX
	        + Ori_Matrix (1, 1) * AY
	        + Ori_Matrix (1, 2) * AZ
	        + Ori_Matrix (1, 3);
      PtZ	= Ori_Matrix (2, 0) * AX
	        + Ori_Matrix (2, 1) * AY
	        + Ori_Matrix (2, 2) * AZ
	        + Ori_Matrix (2, 3);
      PtT	= Ori_Matrix (3, 0) * AX
	        + Ori_Matrix (3, 1) * AY
	        + Ori_Matrix (3, 2) * AZ
	        + Ori_Matrix (3, 3);

      // VRCS -> Normalized Projection Coordinate Space
      APX	= Map_Matrix (0, 0) * PtX
	        + Map_Matrix (0, 1) * PtY
	        + Map_Matrix (0, 2) * PtZ
	        + Map_Matrix (0, 3) * PtT;
      APY	= Map_Matrix (1, 0) * PtX
	        + Map_Matrix (1, 1) * PtY
	        + Map_Matrix (1, 2) * PtZ
	        + Map_Matrix (1, 3) * PtT;
      APZ	= Map_Matrix (2, 0) * PtX
	        + Map_Matrix (2, 1) * PtY
	        + Map_Matrix (2, 2) * PtZ
	        + Map_Matrix (2, 3) * PtT;
      APT	= Map_Matrix (3, 0) * PtX
	        + Map_Matrix (3, 1) * PtY
	        + Map_Matrix (3, 2) * PtZ
	        + Map_Matrix (3, 3) * PtT;

      if (APT == 0. || stop == 0) {
	AU = AV = IntegerLast ();
      }
      else {
	APX /= APT;
	APY /= APT;
	APZ /= APT;

	// NPCS -> Device Coordinate Space
	AWindow->Size (Width, Height);
	Dx	= Standard_Real (Width);
	Dy	= Standard_Real (Height);
	Ratio	= Dx / Dy;
	if (Ratio >= 1.) {
	  AU = Standard_Integer (APX * Dx);
	  AV = Standard_Integer (Dy - APY * Dy * Ratio);
	}
	else {
	  AU = Standard_Integer (APX * Dx / Ratio);
	  AV = Standard_Integer (Dy - APY * Dy);
	}
      }
    }
  }

}

Graphic3d_Vertex Visual3d_ViewManager::ConvertCoord (const Handle(Aspect_Window)& AWindow, const Standard_Integer AU, const Standard_Integer AV) const {

// Convert only if the data is correct
Graphic3d_CView TheCView;
Graphic3d_Vertex Point;

	if (! ViewExists (AWindow, TheCView))
	    Point.SetCoord (RealLast (), RealLast (), RealLast ());
	else {
Standard_Integer Width, Height;
Standard_ShortReal x, y, z;
Standard_Boolean Result;

	    AWindow->Size (Width, Height);

	    Result	= MyGraphicDriver->UnProjectRaster (TheCView,
				0, 0, Width, Height,
				AU, AV, x, y, z);

	    // unproject is done by UnProjectRaster
	    if (Result) {
		Point.SetCoord
		    (Standard_Real (x), Standard_Real (y), Standard_Real (z));
	    }
	    // unproject cannot be done by UnProjectRaster
	    // Code suspended since drivers Phigs and Pex are abandoned.
	    else {

Standard_Real NPCX, NPCY, NPCZ;
Standard_Real VRCX, VRCY, VRCZ, VRCT;
Standard_Real WCX, WCY, WCZ, WCT;

TColStd_Array2OfReal TOri_Matrix (0,3,0,3);
TColStd_Array2OfReal TMap_Matrix (0,3,0,3);
TColStd_Array2OfReal TOri_Matrix_Inv (0,3,0,3);
TColStd_Array2OfReal TMap_Matrix_Inv (0,3,0,3);

Standard_Real Dx, Dy, Ratio;
Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
Standard_Integer j;

Standard_Integer stop = 0;

Standard_Boolean BResult;

		j = MyDefinedView.Extent ();

		while ((! stop) && (MyIterator.More ())) {
		    if (TheCView.ViewId ==
			(MyIterator.Value ())->Identification ()) {
			TOri_Matrix	=
				(MyIterator.Value ())->MatrixOfOrientation ();
			TMap_Matrix	=
				(MyIterator.Value ())->MatrixOfMapping ();
			stop	= 1;
		    }

		    // MyIterator.Next () is located on the next view
		    MyIterator.Next ();
		}

		// View Mapping Transformation and View Clip, inversion
		BResult = Aspect::Inverse (TMap_Matrix, TMap_Matrix_Inv);

		// View Orientation Transformation, inversion
		BResult = Aspect::Inverse (TOri_Matrix, TOri_Matrix_Inv);

		// (AU, AV) : Device Coordinate Space
		// DCS -> NPCS Normalized Projection Coordinate Space
		Dx	= Standard_Real (Width);
		Dy	= Standard_Real (Height);
		Ratio	= Dx / Dy;

		if (Ratio >= 1.) {
			NPCX	= Standard_Real (AU) / Dx;
			NPCY	= (Dy - Standard_Real (AV)) / Dx;
		}
		else {
			NPCX	= Standard_Real (AU) / Dy;
			NPCY	= (Dy - Standard_Real (AV)) / Dy;
		}
		NPCZ	= 0.0;

		// NPCS -> VRCS View Reference Coordinate Space
		// PtVRC = Map_Matrix_Inv.Multiplied (PtNPC);

		VRCX	= TMap_Matrix_Inv (0, 0) * NPCX
			+ TMap_Matrix_Inv (0, 1) * NPCY
			+ TMap_Matrix_Inv (0, 2) * NPCZ
			+ TMap_Matrix_Inv (0, 3);
		VRCY	= TMap_Matrix_Inv (1, 0) * NPCX
			+ TMap_Matrix_Inv (1, 1) * NPCY
			+ TMap_Matrix_Inv (1, 2) * NPCZ
			+ TMap_Matrix_Inv (1, 3);
		VRCZ	= TMap_Matrix_Inv (2, 0) * NPCX
			+ TMap_Matrix_Inv (2, 1) * NPCY
			+ TMap_Matrix_Inv (2, 2) * NPCZ
			+ TMap_Matrix_Inv (2, 3);
		VRCT	= TMap_Matrix_Inv (3, 0) * NPCX
			+ TMap_Matrix_Inv (3, 1) * NPCY
			+ TMap_Matrix_Inv (3, 2) * NPCZ
			+ TMap_Matrix_Inv (3, 3);

		// VRCS -> WCS World Coordinate Space
		// PtWC = Ori_Matrix_Inv.Multiplied (PtVRC);

		WCX	= TOri_Matrix_Inv (0, 0) * VRCX
			+ TOri_Matrix_Inv (0, 1) * VRCY
			+ TOri_Matrix_Inv (0, 2) * VRCZ
			+ TOri_Matrix_Inv (0, 3) * VRCT;
		WCY	= TOri_Matrix_Inv (1, 0) * VRCX
			+ TOri_Matrix_Inv (1, 1) * VRCY
			+ TOri_Matrix_Inv (1, 2) * VRCZ
			+ TOri_Matrix_Inv (1, 3) * VRCT;
		WCZ	= TOri_Matrix_Inv (2, 0) * VRCX
			+ TOri_Matrix_Inv (2, 1) * VRCY
			+ TOri_Matrix_Inv (2, 2) * VRCZ
			+ TOri_Matrix_Inv (2, 3) * VRCT;
		WCT	= TOri_Matrix_Inv (3, 0) * VRCX
			+ TOri_Matrix_Inv (3, 1) * VRCY
			+ TOri_Matrix_Inv (3, 2) * VRCZ
			+ TOri_Matrix_Inv (3, 3) * VRCT;

		if (WCT != 0.)
		    Point.SetCoord (WCX/WCT, WCY/WCT, WCZ/WCT);
		else
		    Point.SetCoord (RealLast (), RealLast (), RealLast ());
	    }
	}

	return (Point);

}

void Visual3d_ViewManager::ConvertCoordWithProj (const Handle(Aspect_Window)& AWindow, const Standard_Integer AU, const Standard_Integer AV, Graphic3d_Vertex& Point, Graphic3d_Vector& Proj) const {

// Conversion only if the data is correct
Graphic3d_CView TheCView;

        if (! ViewExists (AWindow, TheCView)) {
	    Point.SetCoord (RealLast (), RealLast (), RealLast ());
	    Proj.SetCoord (0., 0., 0.);
        }
	else {
Standard_Integer Width, Height;
Standard_ShortReal x, y, z;
Standard_ShortReal dx, dy, dz;
Standard_Boolean Result;

	    AWindow->Size (Width, Height);

	    Result	= MyGraphicDriver->UnProjectRasterWithRay (TheCView,
				0, 0, Width, Height,
				AU, AV, x, y, z, dx, dy, dz);

	    // unproject is done by UnProjectRaster
	    if (Result) {
		Point.SetCoord
		    (Standard_Real (x), Standard_Real (y), Standard_Real (z));
		Proj.SetCoord
		    (Standard_Real (dx), Standard_Real (dy), Standard_Real (dz));
                Proj.Normalize();
	    }
	    // unproject cannot be done by UnProjectRaster
	    // Code is suspended since drivers Phigs are Pex abandoned.
	    else {

Standard_Real NPCX, NPCY, NPCZ;
Standard_Real VRCX, VRCY, VRCZ, VRCT;
Standard_Real WCX, WCY, WCZ, WCT;

TColStd_Array2OfReal TOri_Matrix (0,3,0,3);
TColStd_Array2OfReal TMap_Matrix (0,3,0,3);
TColStd_Array2OfReal TOri_Matrix_Inv (0,3,0,3);
TColStd_Array2OfReal TMap_Matrix_Inv (0,3,0,3);

Standard_Real Dx, Dy, Ratio;
Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
Standard_Integer j;

Standard_Integer stop = 0;

Standard_Boolean BResult;

		j = MyDefinedView.Extent ();

		while ((! stop) && (MyIterator.More ())) {
		    if (TheCView.ViewId ==
			(MyIterator.Value ())->Identification ()) {
			TOri_Matrix	=
				(MyIterator.Value ())->MatrixOfOrientation ();
			TMap_Matrix	=
				(MyIterator.Value ())->MatrixOfMapping ();
			stop	= 1;
		    }

		    // MyIterator.Next () is located on the next view
		    MyIterator.Next ();
		}

		// View Mapping Transformation and View Clip, inversion
		BResult = Aspect::Inverse (TMap_Matrix, TMap_Matrix_Inv);

		// View Orientation Transformation, inversion
		BResult = Aspect::Inverse (TOri_Matrix, TOri_Matrix_Inv);

		// (AU, AV) : Device Coordinate Space
		// DCS -> NPCS Normalized Projection Coordinate Space
		Dx	= Standard_Real (Width);
		Dy	= Standard_Real (Height);
		Ratio	= Dx / Dy;

		if (Ratio >= 1.) {
			NPCX	= Standard_Real (AU) / Dx;
			NPCY	= (Dy - Standard_Real (AV)) / Dx;
		}
		else {
			NPCX	= Standard_Real (AU) / Dy;
			NPCY	= (Dy - Standard_Real (AV)) / Dy;
		}
		NPCZ	= 0.0;

		// NPCS -> VRCS View Reference Coordinate Space
		// PtVRC = Map_Matrix_Inv.Multiplied (PtNPC);

		VRCX	= TMap_Matrix_Inv (0, 0) * NPCX
			+ TMap_Matrix_Inv (0, 1) * NPCY
			+ TMap_Matrix_Inv (0, 2) * NPCZ
			+ TMap_Matrix_Inv (0, 3);
		VRCY	= TMap_Matrix_Inv (1, 0) * NPCX
			+ TMap_Matrix_Inv (1, 1) * NPCY
			+ TMap_Matrix_Inv (1, 2) * NPCZ
			+ TMap_Matrix_Inv (1, 3);
		VRCZ	= TMap_Matrix_Inv (2, 0) * NPCX
			+ TMap_Matrix_Inv (2, 1) * NPCY
			+ TMap_Matrix_Inv (2, 2) * NPCZ
			+ TMap_Matrix_Inv (2, 3);
		VRCT	= TMap_Matrix_Inv (3, 0) * NPCX
			+ TMap_Matrix_Inv (3, 1) * NPCY
			+ TMap_Matrix_Inv (3, 2) * NPCZ
			+ TMap_Matrix_Inv (3, 3);

		// VRCS -> WCS World Coordinate Space
		// PtWC = Ori_Matrix_Inv.Multiplied (PtVRC);

		WCX	= TOri_Matrix_Inv (0, 0) * VRCX
			+ TOri_Matrix_Inv (0, 1) * VRCY
			+ TOri_Matrix_Inv (0, 2) * VRCZ
			+ TOri_Matrix_Inv (0, 3) * VRCT;
		WCY	= TOri_Matrix_Inv (1, 0) * VRCX
			+ TOri_Matrix_Inv (1, 1) * VRCY
			+ TOri_Matrix_Inv (1, 2) * VRCZ
			+ TOri_Matrix_Inv (1, 3) * VRCT;
		WCZ	= TOri_Matrix_Inv (2, 0) * VRCX
			+ TOri_Matrix_Inv (2, 1) * VRCY
			+ TOri_Matrix_Inv (2, 2) * VRCZ
			+ TOri_Matrix_Inv (2, 3) * VRCT;
		WCT	= TOri_Matrix_Inv (3, 0) * VRCX
			+ TOri_Matrix_Inv (3, 1) * VRCY
			+ TOri_Matrix_Inv (3, 2) * VRCZ
			+ TOri_Matrix_Inv (3, 3) * VRCT;

		if (WCT != 0.)
		    Point.SetCoord (WCX/WCT, WCY/WCT, WCZ/WCT);
		else
		    Point.SetCoord (RealLast (), RealLast (), RealLast ());

                // Define projection ray
		NPCZ	= 10.0;

		// NPCS -> VRCS View Reference Coordinate Space
		// PtVRC = Map_Matrix_Inv.Multiplied (PtNPC);

		VRCX	= TMap_Matrix_Inv (0, 0) * NPCX
			+ TMap_Matrix_Inv (0, 1) * NPCY
			+ TMap_Matrix_Inv (0, 2) * NPCZ
			+ TMap_Matrix_Inv (0, 3);
		VRCY	= TMap_Matrix_Inv (1, 0) * NPCX
			+ TMap_Matrix_Inv (1, 1) * NPCY
			+ TMap_Matrix_Inv (1, 2) * NPCZ
			+ TMap_Matrix_Inv (1, 3);
		VRCZ	= TMap_Matrix_Inv (2, 0) * NPCX
			+ TMap_Matrix_Inv (2, 1) * NPCY
			+ TMap_Matrix_Inv (2, 2) * NPCZ
			+ TMap_Matrix_Inv (2, 3);
		VRCT	= TMap_Matrix_Inv (3, 0) * NPCX
			+ TMap_Matrix_Inv (3, 1) * NPCY
			+ TMap_Matrix_Inv (3, 2) * NPCZ
			+ TMap_Matrix_Inv (3, 3);

		// VRCS -> WCS World Coordinate Space
		// PtWC = Ori_Matrix_Inv.Multiplied (PtVRC);

		WCX	= TOri_Matrix_Inv (0, 0) * VRCX
			+ TOri_Matrix_Inv (0, 1) * VRCY
			+ TOri_Matrix_Inv (0, 2) * VRCZ
			+ TOri_Matrix_Inv (0, 3) * VRCT;
		WCY	= TOri_Matrix_Inv (1, 0) * VRCX
			+ TOri_Matrix_Inv (1, 1) * VRCY
			+ TOri_Matrix_Inv (1, 2) * VRCZ
			+ TOri_Matrix_Inv (1, 3) * VRCT;
		WCZ	= TOri_Matrix_Inv (2, 0) * VRCX
			+ TOri_Matrix_Inv (2, 1) * VRCY
			+ TOri_Matrix_Inv (2, 2) * VRCZ
			+ TOri_Matrix_Inv (2, 3) * VRCT;
		WCT	= TOri_Matrix_Inv (3, 0) * VRCX
			+ TOri_Matrix_Inv (3, 1) * VRCY
			+ TOri_Matrix_Inv (3, 2) * VRCZ
			+ TOri_Matrix_Inv (3, 3) * VRCT;

                if (WCT != 0.) {
		    Proj.SetCoord (WCX/WCT, WCY/WCT, WCZ/WCT);
                    Proj.Normalize();
                }
		else
		    Proj.SetCoord (0., 0., 0.);
	    }
	}

}


Standard_Boolean Visual3d_ViewManager::ViewExists (const Handle(Aspect_Window)& AWindow, Graphic3d_CView& TheCView) const {

Standard_Boolean Exist = Standard_False;

	// Parse the list of views to find
	// a view with the specified window
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
	int TheWindowIdOfView;

#ifndef WNT
const Handle(Xw_Window) THEWindow = *(Handle(Xw_Window) *) &AWindow;
	int TheSpecifiedWindowId = int (THEWindow->XWindow ());
#else
const Handle(WNT_Window) THEWindow = *(Handle(WNT_Window) *) &AWindow;
	int TheSpecifiedWindowId = int (THEWindow->HWindow ());
#endif  // WNT

	while ((! Exist) && (MyIterator.More ())) {

	   if ( ((MyIterator.Value ())->IsDefined ()) &&
		((MyIterator.Value ())->IsActive ()) ) {

const Handle(Aspect_Window) AspectWindow = (MyIterator.Value ())->Window ();
#ifndef WNT
const Handle(Xw_Window) theWindow = *(Handle(Xw_Window) *) &AspectWindow;
	TheWindowIdOfView = int (theWindow->XWindow ());
#else
const Handle(WNT_Window) theWindow = *(Handle(WNT_Window) *) &AspectWindow;
	TheWindowIdOfView = int (theWindow->HWindow ());
#endif  // WNT
		// Comparaison on window IDs
		if (TheWindowIdOfView == TheSpecifiedWindowId) {
			Exist	= Standard_True;
			TheCView	= *(CALL_DEF_VIEW *)(MyIterator.Value ())->CView ();
		}
	   } /* if ((MyIterator.Value ())->IsDefined ()) */

	   // MyIterator.Next () is located on the next view
	   MyIterator.Next ();
	}

	return (Exist);

}

void Visual3d_ViewManager::Activate () {

	//
	// Activates all deactivated views
	//
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		if (! (MyIterator.Value ())->IsActive ())
			(MyIterator.Value ())->Activate ();

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::Deactivate () {

	//
	// Deactivates all activated views
	//
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		if ((MyIterator.Value ())->IsActive ())
			(MyIterator.Value ())->Deactivate ();

		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

}

Standard_Integer Visual3d_ViewManager::MaxNumOfViews () const {

	// Retourne the planned of definable views for the current
	// Visual3d_ViewManager.
	return
(Standard_Integer ((View_IDMAX-View_IDMIN+1)/Visual3d_ViewManager::Limit ()));

}

Handle(Graphic3d_Structure) Visual3d_ViewManager::Identification (const Standard_Integer AId) const {

	return (Graphic3d_StructureManager::Identification (AId));

}

Standard_Integer Visual3d_ViewManager::Identification () const {
 
	return (Graphic3d_StructureManager::Identification ());

}

Standard_Integer Visual3d_ViewManager::Identification (const Handle(Visual3d_View)& AView) {

	MyDefinedView.Add (AView);
	return (MyViewGenId.Next ());

}

void Visual3d_ViewManager::UnIdentification (const Standard_Integer aViewId)
{
  MyViewGenId.Free(aViewId);
}

void Visual3d_ViewManager::SetTransparency (const Standard_Boolean AFlag) {

	if (MyTransparency && AFlag) return;
	if (! MyTransparency && ! AFlag) return;

	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
	while (MyIterator.More ()) {
		(MyIterator.Value ())->SetTransparency (AFlag);
		// MyIterator.Next () is located on the next view
		MyIterator.Next ();
	}

	MyTransparency	= AFlag;

}

Standard_Boolean Visual3d_ViewManager::Transparency () const {

	return (MyTransparency);

}

void Visual3d_ViewManager::SetZBufferAuto (const Standard_Boolean AFlag) {

	if (MyZBufferAuto && AFlag) return;
	if (! MyZBufferAuto && ! AFlag) return;

	// if pass from False to True :
	// no problem, at the next view update, it 
	// will properly ask questions to answer (SetVisualisation)
	// if pass from True to False :
	// it is necessary to modify ZBufferActivity at each view so that
	// zbuffer could be active only if required by context.
	// In this case -1 is passed so that the view ask itself the question
	// Note : 0 forces the desactivation, 1 forces the activation
	if (! AFlag) {
		Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
		while (MyIterator.More ()) {
			(MyIterator.Value ())->SetZBufferActivity (-1);
			// MyIterator.Next () is located on the next view
			MyIterator.Next ();
		}
	}
	MyZBufferAuto	= AFlag;

}

Standard_Boolean Visual3d_ViewManager::ZBufferAuto () const {

	return (MyZBufferAuto);

}

void Visual3d_ViewManager::SetLayer (const Handle(Visual3d_Layer)& ALayer) {

#ifdef TRACE_LAYER
	cout << "Visual3d_ViewManager::SetLayer\n" << flush;
#endif

	if (ALayer->Type () == Aspect_TOL_OVERLAY) {
#ifdef TRACE_LAYER
		if (MyOverLayer.IsNull ())
			cout << "MyOverLayer is defined" << endl;
		else
			cout << "MyOverLayer is redefined" << endl;
#endif
		MyOverLayer = ALayer;
	}
	else {
#ifdef TRACE_LAYER
		if (MyUnderLayer.IsNull ())
			cout << "MyUnderLayer is defined" << endl;
		else
			cout << "MyUnderLayer is redefined" << endl;
#endif
		MyUnderLayer = ALayer;
	}

}

const Handle(Visual3d_Layer)& Visual3d_ViewManager::UnderLayer () const {

	return (MyUnderLayer);

}

const Handle(Visual3d_Layer)& Visual3d_ViewManager::OverLayer () const {

	return (MyOverLayer);

}

//=======================================================================
//function : ChangeZLayer
//purpose  : 
//=======================================================================

void Visual3d_ViewManager::ChangeZLayer (const Handle(Graphic3d_Structure)& theStructure,
                                         const Standard_Integer theLayerId)
{
  if (!myLayerIds.Contains (theLayerId))
    return;
  
  // change display layer for structure in all views
  if (MyDisplayedStructure.Contains (theStructure))
  {
    Visual3d_SetIteratorOfSetOfView aViewIt(MyDefinedView);
    for ( ; aViewIt.More (); aViewIt.Next ())
      (aViewIt.Value ())->ChangeZLayer (theStructure, theLayerId);
  }
  
  // tell graphic driver to update the structure's display layer
  MyGraphicDriver->ChangeZLayer (
    (*(Graphic3d_CStructure*)theStructure->CStructure ()), theLayerId);
}

//=======================================================================
//function : GetZLayer
//purpose  :
//=======================================================================

Standard_Integer Visual3d_ViewManager::GetZLayer (const Handle(Graphic3d_Structure)& theStructure) const
{
  Graphic3d_CStructure& aStructure =
    (*(Graphic3d_CStructure*)theStructure->CStructure ());

  return MyGraphicDriver->GetZLayer (aStructure);
}

//=======================================================================
//function : AddZLayer
//purpose  :
//=======================================================================

Standard_Boolean Visual3d_ViewManager::AddZLayer (Standard_Integer& theLayerId)
{
  try
  {
    OCC_CATCH_SIGNALS
    theLayerId = getZLayerGenId ().Next ();
    myLayerIds.Add (theLayerId);
    myLayerSeq.Append (theLayerId);
  }
  catch (Aspect_IdentDefinitionError)
  {
    // new index can't be generated
    return Standard_False;
  }

  // tell all managed views to remove display layers
  Visual3d_SetIteratorOfSetOfView aViewIt(MyDefinedView);
  for ( ; aViewIt.More (); aViewIt.Next ())
    (aViewIt.Value ())->AddZLayer (theLayerId);

  return Standard_True;
}

//=======================================================================
//function : RemoveZLayer
//purpose  : 
//=======================================================================

Standard_Boolean Visual3d_ViewManager::RemoveZLayer (const Standard_Integer theLayerId)
{
  if (!myLayerIds.Contains (theLayerId) || theLayerId == 0)
    return Standard_False;

  // tell all managed views to remove display layers
  Visual3d_SetIteratorOfSetOfView aViewIt(MyDefinedView);
  for ( ; aViewIt.More (); aViewIt.Next ())
    (aViewIt.Value ())->RemoveZLayer (theLayerId);

  MyGraphicDriver->UnsetZLayer (theLayerId);

  // remove index
  for (int aIdx = 1; aIdx <= myLayerSeq.Length (); aIdx++)
    if (myLayerSeq(aIdx) == theLayerId)
    {
      myLayerSeq.Remove (aIdx);
      break;
    }

  myLayerIds.Remove (theLayerId);
  getZLayerGenId ().Free (theLayerId);

  return Standard_True;
}

//=======================================================================
//function : GetAllZLayers
//purpose  :
//=======================================================================

void Visual3d_ViewManager::GetAllZLayers (TColStd_SequenceOfInteger& theLayerSeq) const
{
  theLayerSeq.Assign (myLayerSeq);
}

//=======================================================================
//function : getZLayerGenId
//purpose  :
//=======================================================================

Aspect_GenId& Visual3d_ViewManager::getZLayerGenId ()
{
  static Aspect_GenId aGenId (1, IntegerLast());
  return aGenId;
}

//=======================================================================
//function : InstallZLayers
//purpose  :
//=======================================================================

void Visual3d_ViewManager::InstallZLayers(const Handle(Visual3d_View)& theView) const
{
  if (!MyDefinedView.Contains (theView))
    return;
  
  // erase and insert layers iteratively to provide the same layer order as
  // in the view manager's sequence. This approach bases on the layer insertion
  // order: the new layers are always appended to the end of the list
  // inside of view, while layer remove operation doesn't affect the order.
  // Starting from second layer : no need to change the default z layer.
  for (Standard_Integer aSeqIdx = 2; aSeqIdx <= myLayerSeq.Length (); aSeqIdx++)
  {
    Standard_Integer aLayerID = myLayerSeq.Value (aSeqIdx);
    theView->RemoveZLayer (aLayerID);
    theView->AddZLayer (aLayerID);
  }
}
