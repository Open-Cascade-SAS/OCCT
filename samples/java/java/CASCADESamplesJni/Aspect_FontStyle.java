// Java Native Class from Cas.Cade
//                     Copyright (C) 1991,1999 by
//  
//                      MATRA DATAVISION, FRANCE
//  
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of the above copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any other person. No title to an ownership of the
// software is hereby transferred.
//  
// At the termination of the contract, the software and all copies of this
// software must be deleted.
//


package CASCADESamplesJni;

import CASCADESamplesJni.Aspect_TypeOfFont;
import jcas.Standard_Real;
import jcas.Standard_Boolean;
import jcas.Standard_Short;
import jcas.Standard_CString;
import jcas.Standard_Integer;


public class Aspect_FontStyle extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Aspect_FontStyle() {
   Aspect_FontStyle_Create_1();
}

private final native void Aspect_FontStyle_Create_1();

public Aspect_FontStyle(short Type,double Size,double Slant,boolean CapsHeight) {
   Aspect_FontStyle_Create_2(Type,Size,Slant,CapsHeight);
}

private final native void Aspect_FontStyle_Create_2(short Type,double Size,double Slant,boolean CapsHeight);

public Aspect_FontStyle(Standard_CString Style,double Size,double Slant,boolean CapsHeight) {
   Aspect_FontStyle_Create_3(Style,Size,Slant,CapsHeight);
}

private final native void Aspect_FontStyle_Create_3(Standard_CString Style,double Size,double Slant,boolean CapsHeight);

public Aspect_FontStyle(Standard_CString Style) {
   Aspect_FontStyle_Create_4(Style);
}

private final native void Aspect_FontStyle_Create_4(Standard_CString Style);

native public final Aspect_FontStyle Assign(Aspect_FontStyle Other);
final public void SetValues(short Type,double Size,double Slant,boolean CapsHeight) {
    Aspect_FontStyle_SetValues_1(Type,Size,Slant,CapsHeight);
}

private final native void Aspect_FontStyle_SetValues_1(short Type,double Size,double Slant,boolean CapsHeight);

final public void SetValues(Standard_CString Style,double Size,double Slant,boolean CapsHeight) {
    Aspect_FontStyle_SetValues_2(Style,Size,Slant,CapsHeight);
}

private final native void Aspect_FontStyle_SetValues_2(Standard_CString Style,double Size,double Slant,boolean CapsHeight);

final public void SetValues(Standard_CString Style) {
    Aspect_FontStyle_SetValues_3(Style);
}

private final native void Aspect_FontStyle_SetValues_3(Standard_CString Style);

native public final void SetFamily(Standard_CString aName);
native public final void SetWeight(Standard_CString aName);
native public final void SetRegistry(Standard_CString aName);
native public final void SetEncoding(Standard_CString aName);
native public final short Style();
native public final int Length();
native public final Standard_CString Value();
native public final double Size();
native public final double Slant();
native public final boolean CapsHeight();
native public final Standard_CString AliasName();
native public final Standard_CString FullName();
native public final Standard_CString Foundry();
native public final Standard_CString Family();
native public final Standard_CString Weight();
native public final Standard_CString Registry();
native public final Standard_CString Encoding();
native public final Standard_CString SSlant();
native public final Standard_CString SWidth();
native public final Standard_CString SStyle();
native public final Standard_CString SPixelSize();
native public final Standard_CString SPointSize();
native public final Standard_CString SResolutionX();
native public final Standard_CString SResolutionY();
native public final Standard_CString SSpacing();
native public final Standard_CString SAverageWidth();
native public final void Dump();
native public final boolean IsEqual(Aspect_FontStyle Other);
native public final boolean IsNotEqual(Aspect_FontStyle Other);


public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
