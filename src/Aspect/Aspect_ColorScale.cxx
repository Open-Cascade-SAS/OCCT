// File:	Aspect_ColorScale.cxx
// Created:	Tue Jun 22 17:44:25 2004
// Author:	STV
//Copyright:	Open Cascade 2004

#include <Aspect_ColorScale.ixx>

#include <Aspect_ColorMap.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_SequenceOfColor.hxx>
#include <Aspect_TypeOfColorScaleData.hxx>
#include <Aspect_TypeOfColorScalePosition.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>

#include <Precision.hxx>

#include <stdio.h>

Aspect_ColorScale::Aspect_ColorScale()
: MMgt_TShared(),
myMin( 0.0 ),
myMax( 1.0 ),
myXPos( 0 ),
myYPos( 0 ),
myWidth( 0.2 ),
myHeight( 1 ),
myTitle( "" ),
myInterval( 10 ),
myFormat( "%.4g" ),
myAtBorder( Standard_True ),
myReversed( Standard_False ),
myColorType( Aspect_TOCSD_AUTO ),
myLabelType( Aspect_TOCSD_AUTO ),
myLabelPos( Aspect_TOCSP_RIGHT ),
myTitlePos( Aspect_TOCSP_CENTER ),
myTextHeight(20)
{
}

Standard_Real Aspect_ColorScale::GetMin() const
{
  return myMin;
}

Standard_Real Aspect_ColorScale::GetMax() const
{
  return myMax;
}

void Aspect_ColorScale::GetRange( Standard_Real& aMin, Standard_Real& aMax ) const
{
  aMin = myMin;
  aMax = myMax;
}

Aspect_TypeOfColorScaleData Aspect_ColorScale::GetLabelType() const
{
  return myLabelType;
}

Aspect_TypeOfColorScaleData Aspect_ColorScale::GetColorType() const
{
  return myColorType;
}

Standard_Integer Aspect_ColorScale::GetNumberOfIntervals() const
{
  return myInterval;
}

TCollection_ExtendedString Aspect_ColorScale::GetTitle() const
{
  return myTitle;
}

TCollection_AsciiString Aspect_ColorScale::GetFormat() const
{
  return myFormat;
}

TCollection_ExtendedString Aspect_ColorScale::GetLabel( const Standard_Integer anIndex ) const
{
  TCollection_ExtendedString aLabel;
  if ( anIndex >= 0 && anIndex < myLabels.Length() )
    aLabel = myLabels.Value( anIndex + 1 );
  return aLabel;
}

Quantity_Color Aspect_ColorScale::GetColor( const Standard_Integer anIndex ) const
{
  Quantity_Color aColor;
  if ( anIndex >= 0 && anIndex < myColors.Length() )
    aColor = myColors.Value( anIndex + 1 );
  return aColor;
}

void Aspect_ColorScale::GetLabels( TColStd_SequenceOfExtendedString& aLabels ) const
{
  aLabels.Clear();
  for ( Standard_Integer i = 1; i <= myLabels.Length(); i++ )
    aLabels.Append( myLabels.Value( i ) );
}

void Aspect_ColorScale::GetColors( Aspect_SequenceOfColor& aColors ) const
{
  aColors.Clear();
  for ( Standard_Integer i = 1; i <= myColors.Length(); i++ )
    aColors.Append( myColors.Value( i ) );
}

Aspect_TypeOfColorScalePosition Aspect_ColorScale::GetLabelPosition() const
{
  return myLabelPos;
}

Aspect_TypeOfColorScalePosition Aspect_ColorScale::GetTitlePosition() const
{
  return myTitlePos;
}

Standard_Boolean Aspect_ColorScale::IsReversed() const
{
  return myReversed;
}

Standard_Boolean Aspect_ColorScale::IsLabelAtBorder() const
{
  return myAtBorder;
}

void Aspect_ColorScale::SetMin( const Standard_Real aMin )
{
  SetRange( aMin, GetMax() );
}

void Aspect_ColorScale::SetMax( const Standard_Real aMax )
{
  SetRange( GetMin(), aMax );
}

void Aspect_ColorScale::SetRange( const Standard_Real aMin, const Standard_Real aMax )
{
  if ( myMin == aMin && myMax == aMax )
    return;
 
  myMin = Min( aMin, aMax );
  myMax = Max( aMin, aMax );

  if ( GetColorType() == Aspect_TOCSD_AUTO )
    UpdateColorScale();
}

void Aspect_ColorScale::SetLabelType( const Aspect_TypeOfColorScaleData aType )
{
  if ( myLabelType == aType )
    return;

  myLabelType = aType;
  UpdateColorScale();
}

void Aspect_ColorScale::SetColorType( const Aspect_TypeOfColorScaleData aType )
{
  if ( myColorType == aType )
    return;

  myColorType = aType;
  UpdateColorScale();
}

void Aspect_ColorScale::SetNumberOfIntervals( const Standard_Integer aNum )
{
  if ( myInterval == aNum || aNum < 1 )
    return;

  myInterval = aNum;
  UpdateColorScale();
}

void Aspect_ColorScale::SetTitle( const TCollection_ExtendedString& aTitle )
{
  if ( myTitle == aTitle )
    return;

  myTitle = aTitle;
  UpdateColorScale();
}

void Aspect_ColorScale::SetFormat( const TCollection_AsciiString& aFormat )
{
  if ( myFormat == aFormat )
    return;

  myFormat = aFormat;
  if ( GetLabelType() == Aspect_TOCSD_AUTO )
    UpdateColorScale();
}

void Aspect_ColorScale::SetLabel( const TCollection_ExtendedString& aLabel, const Standard_Integer anIndex )
{
  Standard_Boolean changed = Standard_False;
  Standard_Integer i = anIndex < 1 ? myLabels.Length() + 1 : anIndex;
  if ( i <= myLabels.Length() ) {
    changed = myLabels.Value( i ) != aLabel;
    myLabels.SetValue( i, aLabel );
  }
  else {
    changed = Standard_True;
    while ( i > myLabels.Length() )
      myLabels.Append( TCollection_ExtendedString() );
    myLabels.SetValue( i, aLabel );
  }
  if ( changed )
    UpdateColorScale();
}

void Aspect_ColorScale::SetColor(const Quantity_Color& aColor, const Standard_Integer anIndex )
{
  Standard_Boolean changed = Standard_False;
  Standard_Integer i = anIndex < 1 ? myLabels.Length() + 1 : anIndex;
  if ( i <= myColors.Length() ) {
    changed = myColors.Value( i ) != aColor;
    myColors.SetValue( i, aColor );
  }
  else {
    changed = Standard_True;
    while ( i > myColors.Length() )
      myColors.Append( Quantity_Color() );
    myColors.SetValue( i, aColor );
  }
  if ( changed )
    UpdateColorScale();
}

void Aspect_ColorScale::SetLabels( const TColStd_SequenceOfExtendedString& aSeq )
{
  myLabels.Clear();
  for ( Standard_Integer i = 1; i <= aSeq.Length(); i++ )
    myLabels.Append( aSeq.Value( i ) );
}

void Aspect_ColorScale::SetColors( const Handle(Aspect_ColorMap)& aMap )
{
  myColors.Clear();
  if ( !aMap.IsNull() )
    for ( Standard_Integer i = 1; i <= aMap->Size(); i++ )
      myColors.Append( aMap->Entry( i ).Color() );
}

void Aspect_ColorScale::SetColors( const Aspect_SequenceOfColor& aSeq )
{
  myColors.Clear();
  for ( Standard_Integer i = 1; i <= aSeq.Length(); i++ )
    myColors.Append( aSeq.Value( i ) );
}

void Aspect_ColorScale::SetLabelPosition( const Aspect_TypeOfColorScalePosition aPos )
{
  if ( myLabelPos == aPos )
    return;

  myLabelPos = aPos;
  UpdateColorScale();
}

void Aspect_ColorScale::SetTitlePosition( const Aspect_TypeOfColorScalePosition aPos )
{
  if ( myTitlePos == aPos )
    return;

  myTitlePos = aPos;
  UpdateColorScale();
}

void Aspect_ColorScale::SetReversed( const Standard_Boolean aReverse )
{
  if ( myReversed == aReverse )
    return;

  myReversed = aReverse;
  UpdateColorScale();
}

void Aspect_ColorScale::SetLabelAtBorder( const Standard_Boolean anOn )
{
  if ( myAtBorder == anOn )
    return;

  myAtBorder = anOn;
  UpdateColorScale();
}

void Aspect_ColorScale::GetPosition( Standard_Real& aX, Standard_Real& aY ) const
{
  aX = myXPos;
  aY = myYPos;
}

Standard_Real Aspect_ColorScale::GetXPosition() const
{
  return myXPos;
}

Standard_Real Aspect_ColorScale::GetYPosition() const
{
  return myYPos;
}

void Aspect_ColorScale::SetPosition( const Standard_Real aX, const Standard_Real aY )
{
  if ( myXPos == aX && myYPos == aY )
    return;

  myXPos = aX;
  myYPos = aY;

  UpdateColorScale();
}

void Aspect_ColorScale::SetXPosition( const Standard_Real aX )
{
  SetPosition( aX, GetYPosition() );
}

void Aspect_ColorScale::SetYPosition( const Standard_Real aY )
{
  SetPosition( GetXPosition(), aY );
}

void Aspect_ColorScale::GetSize( Standard_Real& aWidth, Standard_Real& aHeight ) const
{
  aWidth = myWidth;
  aHeight = myHeight;
}

Standard_Real Aspect_ColorScale::GetWidth() const
{
  return myWidth;
}

Standard_Real Aspect_ColorScale::GetHeight() const
{
  return myHeight;
}

void Aspect_ColorScale::SetSize( const Standard_Real aWidth, const Standard_Real aHeight )
{
  if ( myWidth == aWidth && myHeight == aHeight )
    return;

  myWidth = aWidth;
  myHeight = aHeight;

  UpdateColorScale();
}

void Aspect_ColorScale::SetWidth( const Standard_Real aWidth )
{
  SetSize( aWidth, GetHeight() );
}

void Aspect_ColorScale::SetHeight( const Standard_Real aHeight )
{
  SetSize( GetWidth(), aHeight );
}

void Aspect_ColorScale::SizeHint( Standard_Integer& aWidth, Standard_Integer& aHeight ) const
{
  Standard_Integer num = GetNumberOfIntervals();

  Standard_Integer spacer = 5;
  Standard_Integer textWidth = 0;
  Standard_Integer textHeight = TextHeight( "" );
  Standard_Integer colorWidth = 20;

  if ( GetLabelPosition() != Aspect_TOCSP_NONE )
    for ( Standard_Integer idx = 0; idx < num; idx++ )
      textWidth = Max( textWidth, TextWidth( GetCurrentLabel( idx + 1 ) ) );

  Standard_Integer scaleWidth = 0;
  Standard_Integer scaleHeight = 0;

  Standard_Integer titleWidth = 0;
  Standard_Integer titleHeight = 0;

  if ( IsLabelAtBorder() ) {
    num++;
    if ( GetTitle().Length() )
      titleHeight += 10;
  }

  scaleWidth = colorWidth + textWidth + ( textWidth ? 3 : 2 ) * spacer;
  scaleHeight = (Standard_Integer)( 1.5 * ( num + 1 ) * textHeight );
  
  if ( GetTitle().Length() ) {
    titleHeight = TextHeight( GetTitle() ) + spacer;
    titleWidth =  TextWidth( GetTitle() ) + 10;
  }

  aWidth = Max( titleWidth, scaleWidth );
  aHeight = scaleHeight + titleHeight;
}

void Aspect_ColorScale::DrawScale( const Quantity_Color& aBgColor,
				      const Standard_Integer X, const Standard_Integer Y,
				      const Standard_Integer W, const Standard_Integer H )
{
  if ( !BeginPaint() )
    return;

  Standard_Integer num = GetNumberOfIntervals();
  Aspect_TypeOfColorScalePosition labPos = GetLabelPosition();

  Standard_Integer spacer = 5;
  Standard_Integer textWidth = 0;
  Standard_Integer textHeight = TextHeight( "" );

  Standard_Boolean drawLabel = GetLabelPosition() != Aspect_TOCSP_NONE;

  TCollection_ExtendedString aTitle = GetTitle();

  Standard_Integer titleWidth = 0;
  Standard_Integer titleHeight = 0;

  Standard_Integer aGray = (Standard_Integer)(255 * ( aBgColor.Red() * 11 + aBgColor.Green() * 16 + aBgColor.Blue() * 5 ) / 32);
  Quantity_Color aFgColor( aGray < 128 ? Quantity_NOC_WHITE : Quantity_NOC_BLACK );

  // Draw title
  if ( aTitle.Length() ) {
    titleWidth = TextWidth( aTitle );
    titleHeight = TextHeight( aTitle ) + 2 * spacer;
    PaintText( aTitle, X + spacer, Y + spacer, aFgColor );
  }

  Standard_Boolean reverse = IsReversed();

  Aspect_SequenceOfColor colors;
  TColStd_SequenceOfExtendedString labels;
  for ( int idx = 0; idx < num; idx++ ) {
    if ( reverse ) {
      colors.Append( GetCurrentColor( idx ) );
      labels.Append( GetCurrentLabel( idx ) );
    }
    else {
      colors.Prepend( GetCurrentColor( idx ) );
      labels.Prepend( GetCurrentLabel( idx ) );
    }
  }

  if ( IsLabelAtBorder() ) {
    if ( reverse )
      labels.Append( GetCurrentLabel( num ) );
    else
      labels.Prepend( GetCurrentLabel( num ) );
  }

  if ( drawLabel )
    for ( Standard_Integer i = 1; i <= labels.Length(); i++ )
      textWidth = Max( textWidth, TextWidth( labels.Value( i ) ) );

  Standard_Integer lab = labels.Length();

  Standard_Real spc = ( H - ( ( Min( lab, 2 ) + Abs( lab - num - 1 ) ) * textHeight ) - titleHeight );
  Standard_Real val = spc != 0 ? 1.0 * ( lab - Min( lab, 2 ) ) * textHeight / spc : 0;
  Standard_Real iPart;
  Standard_Real fPart = modf( val, &iPart );
  Standard_Integer filter = (Standard_Integer)iPart + ( fPart != 0 ? 1 : 0 );

  Standard_Real step = 1.0 * ( H - ( lab - num + Abs( lab - num - 1 ) ) * textHeight - titleHeight ) / num;

  Standard_Integer ascent = 0;
  Standard_Integer colorWidth = Max( 5, Min( 20, W - textWidth - 3 * spacer ) );
  if ( labPos == Aspect_TOCSP_CENTER || !drawLabel )
    colorWidth = W - 2 * spacer;

  // Draw colors
  Standard_Integer x = X + spacer;
  if ( labPos == Aspect_TOCSP_LEFT )
    x += textWidth + ( textWidth ? 1 : 0 ) * spacer;

  Standard_Real offset = 1.0 * textHeight / 2 * ( lab - num + Abs( lab - num - 1 ) ) + titleHeight;
  for ( Standard_Integer ci = 1; ci <= colors.Length() && step > 0; ci++ ) {
    Standard_Integer y = (Standard_Integer)( Y + ( ci - 1 )* step + offset );
    Standard_Integer h = (Standard_Integer)( Y + ( ci ) * step + offset ) - y;
    PaintRect( x, y, colorWidth, h, colors.Value( ci ), Standard_True );
  }

  if ( step > 0 )
    PaintRect( x - 1, (Standard_Integer)(Y + offset - 1), colorWidth + 2, (Standard_Integer)(colors.Length() * step + 2), aFgColor );

  // Draw labels
  offset = 1.0 * Abs( lab - num - 1 ) * ( step - textHeight ) / 2 + 1.0 * Abs( lab - num - 1 ) * textHeight / 2;
  offset += titleHeight;
  if ( drawLabel && labels.Length() && filter > 0 ) {
    Standard_Integer i1 = 0;
    Standard_Integer i2 = lab - 1;
    Standard_Integer last1( i1 ), last2( i2 );
    x = X + spacer;
    switch ( labPos ) {
    case Aspect_TOCSP_CENTER:
      x += ( colorWidth - textWidth ) / 2;
      break;
    case Aspect_TOCSP_RIGHT:
      x += colorWidth + spacer;
      break;
    }
    while ( i2 - i1 >= filter || ( i2 == 0 && i1 == 0 ) ) {
      Standard_Integer pos1 = i1;
      Standard_Integer pos2 = lab - 1 - i2;
      if ( filter && !( pos1 % filter ) ) {
	PaintText( labels.Value( i1 + 1 ), x, (Standard_Integer)( Y + i1 * step + ascent + offset ), aFgColor );
	last1 = i1;
      }
      if ( filter && !( pos2 % filter ) ) {
	PaintText( labels.Value( i2 + 1 ), x, (Standard_Integer)( Y + i2 * step + ascent + offset ), aFgColor );
	last2 = i2;
      }
      i1++;
      i2--;
    }
    Standard_Integer pos = i1;
    Standard_Integer i0 = -1;
    while ( pos <= i2 && i0 == -1 ) {
      if ( filter && !( pos % filter ) && Abs( pos - last1 ) >= filter && Abs( pos - last2 ) >= filter )
	i0 = pos;
      pos++;
    }

    if ( i0 != -1 )
      PaintText( labels.Value( i0 + 1 ), x, (Standard_Integer)( Y + i0 * step + ascent + offset ), aFgColor );
  }

  EndPaint();
}

Standard_Boolean Aspect_ColorScale::BeginPaint()
{
  return Standard_True;
}

Standard_Boolean Aspect_ColorScale::EndPaint()
{
  return Standard_True;
}

void Aspect_ColorScale::UpdateColorScale()
{
}

TCollection_AsciiString Aspect_ColorScale::Format() const
{
  return GetFormat();
}

TCollection_ExtendedString Aspect_ColorScale::GetCurrentLabel( const Standard_Integer anIndex ) const
{
  TCollection_ExtendedString aLabel;
  if ( GetLabelType() == Aspect_TOCSD_USER )
    aLabel = GetLabel( anIndex );
  else {
    Standard_Real val = GetNumber( anIndex );
    Standard_Character buf[1024];
    TCollection_AsciiString aFormat = Format();
    sprintf( buf, aFormat.ToCString(), val );
    aLabel = TCollection_ExtendedString( buf );
  }

  return aLabel;
}

Quantity_Color Aspect_ColorScale::GetCurrentColor( const Standard_Integer anIndex ) const
{
  Quantity_Color aColor;
  if ( GetColorType() == Aspect_TOCSD_USER )
    aColor = GetColor( anIndex );
  else
    aColor = Quantity_Color( HueFromValue( anIndex, 0, GetNumberOfIntervals() - 1 ), 1.0, 1.0, Quantity_TOC_HLS );
  return aColor;
}

Standard_Real Aspect_ColorScale::GetNumber( const Standard_Integer anIndex ) const
{
  Standard_Real aNum = 0;
  if ( GetNumberOfIntervals() > 0 )
    aNum = GetMin() + anIndex * ( Abs( GetMax() - GetMin() ) / GetNumberOfIntervals() );
  return aNum;
}

Standard_Integer Aspect_ColorScale::HueFromValue( const Standard_Integer aValue,
                                                  const Standard_Integer aMin, const Standard_Integer aMax )
{
  Standard_Integer minLimit( 0 ), maxLimit( 230 );

  Standard_Integer aHue = maxLimit;
  if ( aMin != aMax )
    aHue = (Standard_Integer)( maxLimit - ( maxLimit - minLimit ) * ( aValue - aMin ) / ( aMax - aMin ) );

  aHue = Min( Max( minLimit, aHue ), maxLimit );

  return aHue;
}

Standard_Integer  Aspect_ColorScale::GetTextHeight() const {
  return myTextHeight;
}

void Aspect_ColorScale::SetTextHeight(const Standard_Integer aHeigh) {
  myTextHeight = aHeigh;
  UpdateColorScale ();
}


Standard_Boolean Aspect_ColorScale::FindColor( const Standard_Real aValue, 
                                               Quantity_Color& aColor ) const
{
  return FindColor( aValue, myMin, myMax, myInterval, aColor );
}


Standard_Boolean Aspect_ColorScale::FindColor( const Standard_Real aValue, 
                                               const Standard_Real aMin,
                                               const Standard_Real aMax,
                                               const Standard_Integer ColorsCount,
                                               Quantity_Color& aColor )
{
  if( aValue<aMin || aValue>aMax || aMax<aMin )
    return Standard_False;

  else
  {
    Standard_Real IntervNumber = 0;
    if( aValue<aMin )
      IntervNumber = 0;
    else if( aValue>aMax )
      IntervNumber = ColorsCount-1;
    else if( Abs( aMax-aMin ) > Precision::Approximation() )
      IntervNumber = Ceiling( Standard_Real( ColorsCount ) * ( aValue - aMin ) / ( aMax - aMin ) );

    Standard_Integer Interv = Standard_Integer( IntervNumber );

    aColor = Quantity_Color( HueFromValue( Interv, 0, ColorsCount - 1 ), 1.0, 1.0, Quantity_TOC_HLS );

    return Standard_True;
  } 
}
