
/***********************************************************************

     FONCTION :
     ----------
        Classe Visual3d_ViewManager.cxx :

	Declaration des variables specifiques aux visualiseurs

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
 * Constantes
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
#ifdef DEB
	Standard_Integer Length = MyDefinedView.Extent ();
#else
        MyDefinedView.Extent ();
#endif

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
#ifdef DEB
	Standard_Integer Length = MyDefinedView.Extent ();
#else
        MyDefinedView.Extent ();
#endif
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->ChangeDisplayPriority
			(AStructure, OldPriority, NewPriority);

		// MyIterator.Next () se positionne sur la prochaine vue
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::ReCompute (const Handle(Graphic3d_Structure)& AStructure) { 

  //Standard_Integer LengthD	= MyDisplayedStructure.Extent() ();

  // Meme si physiquement la structure ne peut pas
  // etre affichee (pb de type de visualisation)
  // elle a le statut Displayed.
 
  if (!MyDisplayedStructure.Contains(AStructure))
    return;

  //
  // Recompute structure in all activated views
  //
  Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
  while (MyIterator.More ()) {
    (MyIterator.Value ())->ReCompute (AStructure);

    // MyIterator.Next () se positionne sur la prochaine vue
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

  Standard_Integer indexD = 0;

// Meme si physiquement la structure ne peut pas
// etre affichee (pb de type de visualisation)
// elle a le statut Displayed.
  if (!MyDisplayedStructure.Contains(AStructure))
    return;
	
  //
  // Recompute structure in one activated view
  //
  Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
  while (MyIterator.More ()) {
    if ((MyIterator.Value ())->Identification () == ViewId)
      theView->ReCompute (AStructure);

    // MyIterator.Next () se positionne sur la prochaine vue
    MyIterator.Next ();
  }

}

void Visual3d_ViewManager::Clear (const Handle(Graphic3d_Structure)& AStructure, const Standard_Boolean WithDestruction) {

#ifdef DEB
	Standard_Integer Length = MyDefinedView.Extent ();
#else
        MyDefinedView.Extent ();
#endif
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Clear (AStructure, WithDestruction);

		// MyIterator.Next () se positionne sur la prochaine vue
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::Connect (const Handle(Graphic3d_Structure)& AMother, const Handle(Graphic3d_Structure)& ADaughter) {

#ifdef DEB
	Standard_Integer Length = MyDefinedView.Extent ();
#else
        MyDefinedView.Extent ();
#endif
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Connect (AMother, ADaughter);

		// MyIterator.Next () se positionne sur la prochaine vue
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::Disconnect (const Handle(Graphic3d_Structure)& AMother, const Handle(Graphic3d_Structure)& ADaughter) {

#ifdef DEB
	Standard_Integer Length = MyDefinedView.Extent ();
#else
        MyDefinedView.Extent ();
#endif
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Disconnect (AMother, ADaughter);

		// MyIterator.Next () se positionne sur la prochaine vue
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::Display (const Handle(Graphic3d_Structure)& AStructure) { 


  // Meme si physiquement la structure ne peut pas
  // etre affichee (pb de type de visualisation)
  // elle a le statut Displayed.

  MyDisplayedStructure.Add(AStructure);

	//
	// Display structure in all activated views
	//
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Display (AStructure);

		// MyIterator.Next () se positionne sur la prochaine vue
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::Erase (const Handle(Graphic3d_Structure)& AStructure) {


// Meme si physiquement la structure ne pouvait pas
// etre affichee (pb de type de visualisation)
// elle avait le statut Displayed.

 MyDisplayedStructure.Remove(AStructure);



	//
	// Erase structure in all defined views
	//
#ifdef DEB
	Standard_Integer Length = MyDefinedView.Extent ();
#else
        MyDefinedView.Extent ();
#endif
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Erase (AStructure);

		// MyIterator.Next () se positionne sur la prochaine vue
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
#ifdef DEB
	Standard_Integer Length = MyDefinedView.Extent ();
#else
        MyDefinedView.Extent ();
#endif
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->Highlight (AStructure, AMethod);

		// MyIterator.Next () se positionne sur la prochaine vue
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::SetTransform (const Handle(Graphic3d_Structure)& AStructure, const TColStd_Array2OfReal& ATrsf) {

#ifdef DEB
	Standard_Integer Length = MyDefinedView.Extent ();
#else
        MyDefinedView.Extent ();
#endif
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->SetTransform (AStructure, ATrsf);

		// MyIterator.Next () se positionne sur la prochaine vue
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
#ifdef DEB
	Standard_Integer Length = MyDefinedView.Extent ();
#else
        MyDefinedView.Extent ();
#endif
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		(MyIterator.Value ())->UnHighlight (AStructure);

		// MyIterator.Next () se positionne sur la prochaine vue
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

		// MyIterator.Next () se positionne sur la prochaine vue
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

	    // MyIterator.Next () se positionne sur la prochaine vue
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

		// MyIterator.Next () se positionne sur la prochaine vue
		MyIterator.Next ();
	}

}

Handle(Visual3d_HSetOfView) Visual3d_ViewManager::ActivatedView () const {

Handle (Visual3d_HSetOfView) SG = new Visual3d_HSetOfView ();

#ifdef DEB
	Standard_Integer Length = MyDefinedView.Extent ();
#else
        MyDefinedView.Extent ();
#endif
Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);

	while (MyIterator.More ()) {
		if ((MyIterator.Value ())->IsActive ())
			SG->Add (MyIterator.Value ());

		// MyIterator.Next () se positionne sur la prochaine vue
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

		// MyIterator.Next () se positionne
		// sur la prochaine vue
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

		// MyIterator.Next () se positionne sur la prochaine vue
		MyIterator.Next ();
	}

	return (SG);

}

void Visual3d_ViewManager::ConvertCoord (const Handle(Aspect_Window)& AWindow, const Graphic3d_Vertex& AVertex, Standard_Integer& AU, Standard_Integer& AV) const {

// On convertit que si les donnees sont correctes
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
#ifdef DEB
      Standard_Integer Length = MyDefinedView.Extent ();
#else
      MyDefinedView.Extent ();
#endif

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

	// MyIterator.Next () se positionne
	// sur la prochaine vue
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

// On convertit que si les donnees sont correctes
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

	    // Le unproject est realise par UnProjectRaster
	    if (Result) {
		Point.SetCoord
		    (Standard_Real (x), Standard_Real (y), Standard_Real (z));
	    }
	    // Le unproject n'est pas realisable par UnProjectRaster
	    // Code a virer des que les drivers Phigs et Pex seront abandonnes.
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

		    // MyIterator.Next () se positionne
		    // sur la prochaine vue
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

// On convertit que si les donnees sont correctes
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

	    // Le unproject est realise par UnProjectRaster
	    if (Result) {
		Point.SetCoord
		    (Standard_Real (x), Standard_Real (y), Standard_Real (z));
		Proj.SetCoord
		    (Standard_Real (dx), Standard_Real (dy), Standard_Real (dz));
                Proj.Normalize();
	    }
	    // Le unproject n'est pas realisable par UnProjectRaster
	    // Code a virer des que les drivers Phigs et Pex seront abandonnes.
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

		    // MyIterator.Next () se positionne
		    // sur la prochaine vue
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

Visual3d_PickDescriptor Visual3d_ViewManager::Pick (const Visual3d_ContextPick& CTX, const Handle(Aspect_Window)& AWindow, const Standard_Integer AX, const Standard_Integer AY) {

// On active le reperage que si les donnees sont correctes
Standard_Boolean DoPick = Standard_False;

CALL_DEF_PICK apick;
Standard_Integer Width, Height;

	// Parcours de la liste des vues pour rechercher
	// une vue ayant pour fenetre celle specifiee
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
	int TheWindowIdOfView;

#ifndef WNT
const Handle(Xw_Window) THEWindow = *(Handle(Xw_Window) *) &AWindow;
	int TheSpecifiedWindowId = int (THEWindow->XWindow ());
#else
const Handle(WNT_Window) THEWindow = *(Handle(WNT_Window) *) &AWindow;
	int TheSpecifiedWindowId = int (THEWindow->HWindow ());
#endif  // WNT

	while ((! DoPick) && (MyIterator.More ())) {

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
		// Comparaison sur les windows ids
		if (TheWindowIdOfView == TheSpecifiedWindowId) {
			DoPick		= Standard_True;

			// Mise a jour
			apick.WsId	=
			int ((MyIterator.Value ())->Identification ());

			apick.ViewId	=
			int ((MyIterator.Value ())->Identification ());
#ifndef WNT
			apick.DefWindow.XWindow	= TheSpecifiedWindowId;
#else
			apick.DefWindow.XWindow	= (HWND) TheSpecifiedWindowId;
#endif

			apick.x			= int (AX);
			apick.y			= int (AY);

			theWindow->Size (Width, Height);
			apick.DefWindow.dx	= float (Width);
			apick.DefWindow.dy	= float (Height);

			apick.Context.aperture	= (float) CTX.Aperture ();
			apick.Context.order	= int (CTX.Order ());
			apick.Context.depth	= int (CTX.Depth ());

		}
	   } /* if ((MyIterator.Value ())->IsDefined ()) { */

	   // MyIterator.Next () se positionne
	   // sur la prochaine vue
	   MyIterator.Next ();
	}

	if (DoPick)
		MyGraphicDriver->Pick (apick);
	else
		apick.Pick.depth	= 0;

	// Picking : le retour
Standard_Integer i, j=0;
Standard_Integer NbPick;

Visual3d_PickDescriptor PDes (CTX);
Visual3d_PickPath PPat;

	PDes.Clear ();
	NbPick	= 0;
	// For i=0 it is not a graphic structure it is a view structure
	// For i=1 it is the displayed graphic structure
	// For i=2 to apick.Pick.depth-1 it is the connected graphic structures
	if (apick.Pick.depth != 0) {
	    j = apick.Pick.listid[1];
	    if ((Graphic3d_StructureManager::Identification (j))->
						IsSelectable ()) {
		// Maj element number
		PPat.SetElementNumber (apick.Pick.listelem[1]);
		// Maj pick identifier
		PPat.SetPickIdentifier (apick.Pick.listpickid[1]);
		// Maj structure
		PPat.SetStructIdentifier
			(Graphic3d_StructureManager::Identification (j));
		// Maj PickPath
		PDes.AddPickPath (PPat);
		NbPick++;
	    }
	}

	// Pas tres efficace, a revoir (CAL 22/09/95)
	if (apick.Pick.depth > 2) {
Handle(Graphic3d_Structure) StructCur =
	Graphic3d_StructureManager::Identification (j);
Standard_Boolean found;
Graphic3d_MapOfStructure Set;

	    for (i=2; i<apick.Pick.depth; i++) {
		found = Standard_False;
		j = apick.Pick.listid[i-1];
		Set.Clear ();
		StructCur->Descendants (Set);
Graphic3d_MapIteratorOfMapOfStructure IteratorD (Set);

		j = apick.Pick.listid[i];
		while (IteratorD.More () && !found) {
		    StructCur = IteratorD.Key ();
		    if (StructCur->Identification () == j ) {
			found = Standard_True;
			// Maj element number
			PPat.SetElementNumber (apick.Pick.listelem[i]);
			// Maj pick identifier
			PPat.SetPickIdentifier (apick.Pick.listpickid[i]);
			// Maj structure
			PPat.SetStructIdentifier (StructCur);
			// Maj PickPath
			PDes.AddPickPath (PPat);
			NbPick++;
		    }
		    // IteratorD.Next () se positionne
		    // sur la prochaine structure
		    IteratorD.Next ();
		}
	    }
	}

	apick.Pick.depth	= int (NbPick);

	MyGraphicDriver->InitPick ();

	return (PDes);

}

Standard_Boolean Visual3d_ViewManager::ViewExists (const Handle(Aspect_Window)& AWindow, Graphic3d_CView& TheCView) const {

Standard_Boolean Exist = Standard_False;

	// Parcours de la liste des vues pour rechercher
	// une vue ayant pour fenetre celle specifiee
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
		// Comparaison sur les windows ids
		if (TheWindowIdOfView == TheSpecifiedWindowId) {
			Exist	= Standard_True;
			TheCView	= *(CALL_DEF_VIEW *)(MyIterator.Value ())->CView ();
		}
	   } /* if ((MyIterator.Value ())->IsDefined ()) */

	   // MyIterator.Next () se positionne
	   // sur la prochaine vue
	   MyIterator.Next ();
	}

	return (Exist);

}

void Visual3d_ViewManager::Activate () {

	//
	// Activates all deactivated views
	//
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
#ifdef DEB
	Standard_Integer Length = MyDefinedView.Extent ();
#else
        MyDefinedView.Extent ();
#endif

	while (MyIterator.More ()) {
		if (! (MyIterator.Value ())->IsActive ())
			(MyIterator.Value ())->Activate ();

		// MyIterator.Next () se positionne
		// sur la prochaine vue
		MyIterator.Next ();
	}

}

void Visual3d_ViewManager::Deactivate () {

	//
	// Deactivates all activated views
	//
#ifdef DEB
	Standard_Integer Length = MyDefinedView.Extent ();
#else
        MyDefinedView.Extent ();
#endif
	Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
 
	while (MyIterator.More ()) {
		if ((MyIterator.Value ())->IsActive ())
			(MyIterator.Value ())->Deactivate ();

		// MyIterator.Next () se positionne
		// sur la prochaine vue
		MyIterator.Next ();
	}

}

Standard_Integer Visual3d_ViewManager::MaxNumOfViews () const {

	// on retourne le nombre theorique de vues definissables pour le
	// Visual3d_ViewManager courant.
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
		// MyIterator.Next () se positionne sur la prochaine vue
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

	// si de False on passe a True :
	// pas de probleme, au prochain update de vue, celle-ci se
	// posera les bonnes questions pour reagir (SetVisualisation)
	// si de True on passe a False :
	// il faut modifier le ZBufferActivity de chaque vue pour que
	// le zbuffer ne soit actif que si le contexte le demande.
	// Dans ce cas on passe -1 pour que la vue se pose la question
	// Rappel : 0 on force la desactivation, 1 on force l'activation
	if (! AFlag) {
		Visual3d_SetIteratorOfSetOfView MyIterator(MyDefinedView);
		while (MyIterator.More ()) {
			(MyIterator.Value ())->SetZBufferActivity (-1);
			// MyIterator.Next () se positionne sur la prochaine vue
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
