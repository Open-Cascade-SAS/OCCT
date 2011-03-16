//
// ARCH		//GG_240298
//		Nouvelle methode privee ResetIndex() qui sert de relais
//		a SetIndex() permettant de reinitialiser l'index
//		courant d'exploration des primitives.
//
//		//GG_200898
//		Nouvelle methode privee ComputeMinMax() qui peut etre redefinie
//		dans chaque primitive de maniere a recalculer les minmax
//		au moment ou l'on en a besoin.
//
// S3593	//GG_270298
//		Nouvelle methodes privees SetFamily() appellee par
//		Line(),Marker(),Text(),Image() et Family()
//		qui retourne la famille a laquelle appartient la primitive
//		Voir son utilisation dans Graphic2d_GraphicObject.
//		Optimisation de la methode MinMax qui retourne maintenant
//		un BOOLEAN pour signale que la primitive est vide.
//
// G002     //GG_140400
//              Add SetPickedIndex() method
//		Add DrawElement() and DrawVertex() methods
//		Add IsOn() method
//		Add Highlight(),Unhighlight() IsHighlighted() method.
//

#include <Graphic2d_Primitive.ixx>
#include <Graphic2d_View.hxx>

Graphic2d_Primitive::Graphic2d_Primitive (const Handle(Graphic2d_GraphicObject)& aGraphicObject):
	myMinX (ShortRealLast ()),
	myMinY (ShortRealLast ()),
	myMaxX (ShortRealFirst()),
	myMaxY (ShortRealFirst()),
    	myColorIndex (1),
    	myNumOfElem(0),
    	myNumOfVert(0),
    	myDisplayMode(0),
	myPickedIndex (0),
    	myTypeOfPrimitive(Graphic2d_TOP_UNKNOWN) ,
	mySeqOfHighInd( new TColStd_HSequenceOfInteger() ) {

	myGOPtr	= aGraphicObject.operator->();
	aGraphicObject->AddPrimitive (this);

}

Standard_Boolean Graphic2d_Primitive::ComputeMinMax () {
// cout << "*Graphic2d_Primitive::ComputeMinMax() must be redefined in the primitive '" << this->DynamicType()->Name() << "'" << endl;
  	return Standard_False;
}

Standard_Boolean Graphic2d_Primitive::MinMax (Standard_ShortReal & Minx, Standard_ShortReal & Maxx, Standard_ShortReal & Miny, Standard_ShortReal & Maxy) {
Standard_Boolean status = Standard_True;

	if( (myMaxX < myMinX) || (myMaxY < myMinY) ) {
	  this->ComputeMinMax();
	}

	if( (myMaxX >= myMinX) && (myMaxY >= myMinY) ) {
	  if (myGOPtr->IsTransformed () ) {
	    Standard_Real MINX,MAXX,MINY,MAXY,X,Y;
	    gp_GTrsf2d aTrsf = myGOPtr->Transform ();
	    X = myMinX; Y = myMinY;
	    aTrsf.Transforms (X, Y);
	    MINX = MAXX = X; MINY = MAXY = Y;

	    X = myMaxX; Y = myMaxY;
	    aTrsf.Transforms (X, Y);
	    MINX = Min(MINX,X); MINY = Min(MINY,Y);
	    MAXX = Max(MAXX,X); MAXY = Max(MAXY,Y);

	    X = myMinX; Y = myMaxY;
	    aTrsf.Transforms (X, Y);
	    MINX = Min(MINX,X); MINY = Min(MINY,Y);
	    MAXX = Max(MAXX,X); MAXY = Max(MAXY,Y);

	    X = myMaxX; Y = myMinY;
	    aTrsf.Transforms (X, Y);
	    Minx = Standard_ShortReal(Min(MINX,X)); 
        Miny = Standard_ShortReal(Min(MINY,Y));
	    Maxx = Standard_ShortReal(Max(MAXX,X)); 
        Maxy = Standard_ShortReal(Max(MAXY,Y));
	  } else {
	    Minx = myMinX; Miny = myMinY;
	    Maxx = myMaxX; Maxy = myMaxY;
	  }
	} else {
	  status = Standard_False;
	  Minx = myMinX; Miny = myMinY;
	  Maxx = myMaxX; Maxy = myMaxY;
	}


	return status;

}

void Graphic2d_Primitive::SetColorIndex (const Standard_Integer anIndex) {

	myColorIndex	= anIndex;
	ResetIndex();

}

Standard_Integer Graphic2d_Primitive::ColorIndex () const {

	return myColorIndex;

}

Standard_Integer Graphic2d_Primitive::PickedIndex () const {

	return myPickedIndex;

}

Standard_Integer Graphic2d_Primitive::NumOfElemIndices() const {
  return myNumOfElem;
}

Standard_Integer Graphic2d_Primitive::NumOfVertIndices() const {
 return myNumOfVert;
}

void Graphic2d_Primitive::SetPickedIndex (const Standard_Integer anIndex) {

	myPickedIndex = anIndex;
}

Standard_Boolean Graphic2d_Primitive::IsOn( const Standard_ShortReal aX1,
                                            const Standard_ShortReal aY1,
                                            const Standard_ShortReal aX2,
                                            const Standard_ShortReal aY2,
                                            const Standard_ShortReal aPrecision) {

  Standard_ShortReal DX = aX2 - aX1, DY = aY2 - aY1, 
        dd = Standard_ShortReal(Sqrt(DX*DX + DY*DY));

        return dd < aPrecision;
}

Standard_Boolean Graphic2d_Primitive::IsInMinMax (const Standard_ShortReal X,
					 const Standard_ShortReal Y,
					 const Standard_ShortReal aPrecision)
 {
Standard_Boolean Result = Standard_False;

	if( (myMaxX < myMinX) || (myMaxY < myMinY) ) {
	  this->ComputeMinMax();
	}

	if (! myGOPtr->IsTransformed ())
		Result =
			X >= myMinX - aPrecision && 
			X <= myMaxX + aPrecision && 
			Y >= myMinY - aPrecision && 
			Y <= myMaxY + aPrecision;
	else {
		Standard_ShortReal minx, miny, maxx, maxy;
		MinMax(minx,maxx,miny,maxy);
		Result =
			X >= minx - aPrecision && 
			X <= maxx + aPrecision && 
			Y >= miny - aPrecision && 
			Y <= maxy + aPrecision;
	}

	return Result;

}

Handle(Graphic2d_Drawer) Graphic2d_Primitive::Drawer() const {

        return myGOPtr->View()->Drawer();
}

Graphic2d_GOPtr Graphic2d_Primitive::PGraphicObject() const {

        return myGOPtr;
}

void Graphic2d_Primitive::ResetIndex() {

	myGOPtr->SetIndex(this);
}

void Graphic2d_Primitive::SetFamily( const Graphic2d_TypeOfPrimitive aFamily) {

	myTypeOfPrimitive = aFamily;
}

Graphic2d_TypeOfPrimitive Graphic2d_Primitive::Family() const {

	return myTypeOfPrimitive;
}

void Graphic2d_Primitive::DrawElement( const Handle(Graphic2d_Drawer)& /*aDrawer*/,
				       const Standard_Integer /*anElement*/) {
   cout << " *** Graphic2d_Primitive::DrawElement() method must be redefined ***" << endl;
}

//SAV
void Graphic2d_Primitive
::DrawPickedElements( const Handle(Graphic2d_Drawer)& /*aDrawer*/ )
{
#ifdef DEB
  cout << " *** Graphic2d_Primitive::DrawPickedElements() method must be redefined ***"
    << endl;
#endif
}

// SAV    	
void Graphic2d_Primitive
::DrawSelectedElements( const Handle(Graphic2d_Drawer)& /*aDrawer*/ )
{
#ifdef DEB
  cout << " *** Graphic2d_Primitive::DrawSelectedElements() method must be redefined ***"
    << endl;
#endif
}

// SAV
Standard_Boolean Graphic2d_Primitive::SetElementsSelected()
{
  return Standard_False;
}

// SAV
Standard_Boolean Graphic2d_Primitive::HasSelectedElements()
{
  /* if this method is not redefined we should return false
     to achieve the standard behavior of Graphic2d_GraphicObject::Draw() method.
     already redefined for SetOfMarkers
  */
  return Standard_False;
}

// SAV
void Graphic2d_Primitive::ClearSelectedElements()
{
  // parent does nothing! redefine please
#ifdef DEB
  cout << " *** Graphic2d_Primitive::ClearSelectedElements() method must be redefined ***"
    << endl;
#endif
}

// SAV
Standard_Boolean Graphic2d_Primitive
::PickByCircle( const Standard_ShortReal X,
	        const Standard_ShortReal Y,
	        const Standard_ShortReal Radius,
	        const Handle(Graphic2d_Drawer)& /*aDrawer*/,
	        const Graphic2d_PickMode aPickMode )
{
  // parent does nothing! redefine please
#ifdef SAV_DEB
  cout << " *** Graphic2d_Primitive::PickByCircle() method must be redefined ***"
    << endl;
#endif
  return Standard_False;
}


void Graphic2d_Primitive::DrawVertex( const Handle(Graphic2d_Drawer)& /*aDrawer*/,
				       const Standard_Integer /*aVertex*/) {
   cout << " *** Graphic2d_Primitive::DrawVertex() method must be redefined ***" << endl;
}

void Graphic2d_Primitive::Highlight( const Standard_Integer anIndex) {

     mySeqOfHighInd->Append(anIndex);
}

void Graphic2d_Primitive::Unhighlight () {
    if ( ! mySeqOfHighInd->IsEmpty() )
        mySeqOfHighInd->Clear();
}

Standard_Boolean Graphic2d_Primitive::IsHighlighted () const {
    return ( ! mySeqOfHighInd->IsEmpty() );
}

Handle(TColStd_HSequenceOfInteger) Graphic2d_Primitive::HighlightIndices() const {

    return mySeqOfHighInd;
}

const TColStd_MapOfInteger& Graphic2d_Primitive::PickedIndices() const
{
  return myPickedIndices;
}


void Graphic2d_Primitive::SetDisplayMode( const Standard_Integer aMode )  {

 myDisplayMode = aMode;

} 

Standard_Integer Graphic2d_Primitive::DisplayMode() const {

 return myDisplayMode;
}

Standard_Boolean Graphic2d_Primitive::Pick( const Standard_ShortReal Xmin,
                                            const Standard_ShortReal Ymin,
                                            const Standard_ShortReal Xmax,
                                            const Standard_ShortReal Ymax,
                                            const Handle(Graphic2d_Drawer)& /*aDrawer*/,
                                            const Graphic2d_PickMode aPickMode ) {

 
  Standard_ShortReal X1 = Min( Xmax, Xmin );
  Standard_ShortReal Y1 = Min( Ymax, Ymin );
  Standard_ShortReal X2 = Max( Xmax, Xmin );
  Standard_ShortReal Y2 = Max( Ymax, Ymin );

  Standard_Boolean Result = Standard_False;

  if ( ( myMaxX < myMinX ) || ( myMaxY < myMinY ) )
    ComputeMinMax();
  Standard_ShortReal minx = myMinX, miny = myMinY, 
                     maxx = myMaxX, maxy = myMaxY;
  if ( myGOPtr->IsTransformed() )
     MinMax(minx,maxx,miny,maxy);
  switch ( aPickMode )  {
  case Graphic2d_PM_INCLUDE: 

    Result =
            minx >= X1 &&
            miny >= Y1 &&
            maxx <= X2 &&
            maxy <= Y2;
    break;
  case Graphic2d_PM_EXCLUDE: 

    Result =
			maxx < X1 || 
			maxy < Y1 || 
			minx > X2 || 
			miny > Y2;
    break;
  case Graphic2d_PM_INTERSECT: {

      Standard_ShortReal a = Max( minx, X1 );
      Standard_ShortReal b = Max( miny, Y1 );
      Standard_ShortReal c = Min( maxx, X2 );
      Standard_ShortReal d = Min( maxy, Y2 );

      Result = ( a >= c || d <= b ) ? 0 : 1;

      if ( Result && 
           ( X1 >= minx &&
             Y1 >= miny  &&
             X2 <= maxx  &&
             Y2 <= maxy ) ) Result = 0;;

      
  }
  break;                      
  default: 
         Result = Standard_False;
  }
     
  return Result;

}
