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

import jcas.Standard_CString;
import jcas.Standard_Integer;
import jcas.Standard_Character;
import jcas.Standard_Real;
import CASCADESamplesJni.TCollection_ExtendedString;
import jcas.Standard_Boolean;


public class TCollection_AsciiString extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public TCollection_AsciiString() {
   TCollection_AsciiString_Create_1();
}

private final native void TCollection_AsciiString_Create_1();

public TCollection_AsciiString(Standard_CString message) {
   TCollection_AsciiString_Create_2(message);
}

private final native void TCollection_AsciiString_Create_2(Standard_CString message);

public TCollection_AsciiString(Standard_CString message,int aLen) {
   TCollection_AsciiString_Create_3(message,aLen);
}

private final native void TCollection_AsciiString_Create_3(Standard_CString message,int aLen);

public TCollection_AsciiString(Standard_Character aChar) {
   TCollection_AsciiString_Create_4(aChar);
}

private final native void TCollection_AsciiString_Create_4(Standard_Character aChar);

public TCollection_AsciiString(int length,Standard_Character filler) {
   TCollection_AsciiString_Create_5(length,filler);
}

private final native void TCollection_AsciiString_Create_5(int length,Standard_Character filler);

public TCollection_AsciiString(int value) {
   TCollection_AsciiString_Create_6(value);
}

private final native void TCollection_AsciiString_Create_6(int value);

public TCollection_AsciiString(double value) {
   TCollection_AsciiString_Create_7(value);
}

private final native void TCollection_AsciiString_Create_7(double value);

public TCollection_AsciiString(TCollection_AsciiString astring) {
   TCollection_AsciiString_Create_8(astring);
}

private final native void TCollection_AsciiString_Create_8(TCollection_AsciiString astring);

public TCollection_AsciiString(TCollection_AsciiString astring,Standard_Character message) {
   TCollection_AsciiString_Create_9(astring,message);
}

private final native void TCollection_AsciiString_Create_9(TCollection_AsciiString astring,Standard_Character message);

public TCollection_AsciiString(TCollection_AsciiString astring,Standard_CString message) {
   TCollection_AsciiString_Create_10(astring,message);
}

private final native void TCollection_AsciiString_Create_10(TCollection_AsciiString astring,Standard_CString message);

public TCollection_AsciiString(TCollection_AsciiString astring,TCollection_AsciiString message) {
   TCollection_AsciiString_Create_11(astring,message);
}

private final native void TCollection_AsciiString_Create_11(TCollection_AsciiString astring,TCollection_AsciiString message);

public TCollection_AsciiString(TCollection_ExtendedString astring) {
   TCollection_AsciiString_Create_12(astring);
}

private final native void TCollection_AsciiString_Create_12(TCollection_ExtendedString astring);

final public void AssignCat(Standard_Character other) {
    TCollection_AsciiString_AssignCat_1(other);
}

private final native void TCollection_AsciiString_AssignCat_1(Standard_Character other);

final public void AssignCat(int other) {
    TCollection_AsciiString_AssignCat_2(other);
}

private final native void TCollection_AsciiString_AssignCat_2(int other);

final public void AssignCat(double other) {
    TCollection_AsciiString_AssignCat_3(other);
}

private final native void TCollection_AsciiString_AssignCat_3(double other);

final public void AssignCat(Standard_CString other) {
    TCollection_AsciiString_AssignCat_4(other);
}

private final native void TCollection_AsciiString_AssignCat_4(Standard_CString other);

final public void AssignCat(TCollection_AsciiString other) {
    TCollection_AsciiString_AssignCat_5(other);
}

private final native void TCollection_AsciiString_AssignCat_5(TCollection_AsciiString other);

native public final void Capitalize();
final public TCollection_AsciiString Cat(Standard_Character other) {
   return TCollection_AsciiString_Cat_2(other);
}

private final native TCollection_AsciiString TCollection_AsciiString_Cat_2(Standard_Character other);

final public TCollection_AsciiString Cat(int other) {
   return TCollection_AsciiString_Cat_3(other);
}

private final native TCollection_AsciiString TCollection_AsciiString_Cat_3(int other);

final public TCollection_AsciiString Cat(double other) {
   return TCollection_AsciiString_Cat_4(other);
}

private final native TCollection_AsciiString TCollection_AsciiString_Cat_4(double other);

final public TCollection_AsciiString Cat(Standard_CString other) {
   return TCollection_AsciiString_Cat_5(other);
}

private final native TCollection_AsciiString TCollection_AsciiString_Cat_5(Standard_CString other);

final public TCollection_AsciiString Cat(TCollection_AsciiString other) {
   return TCollection_AsciiString_Cat_7(other);
}

private final native TCollection_AsciiString TCollection_AsciiString_Cat_7(TCollection_AsciiString other);

native public final void Center(int Width,Standard_Character Filler);
native public final void ChangeAll(Standard_Character aChar,Standard_Character NewChar,boolean CaseSensitive);
native public final void Clear();
final public void Copy(Standard_CString fromwhere) {
    TCollection_AsciiString_Copy_1(fromwhere);
}

private final native void TCollection_AsciiString_Copy_1(Standard_CString fromwhere);

final public void Copy(TCollection_AsciiString fromwhere) {
    TCollection_AsciiString_Copy_2(fromwhere);
}

private final native void TCollection_AsciiString_Copy_2(TCollection_AsciiString fromwhere);

native public final void Destroy();
native public final int FirstLocationInSet(TCollection_AsciiString Set,int FromIndex,int ToIndex);
native public final int FirstLocationNotInSet(TCollection_AsciiString Set,int FromIndex,int ToIndex);
final public void Insert(int where,Standard_Character what) {
    TCollection_AsciiString_Insert_1(where,what);
}

private final native void TCollection_AsciiString_Insert_1(int where,Standard_Character what);

final public void Insert(int where,Standard_CString what) {
    TCollection_AsciiString_Insert_2(where,what);
}

private final native void TCollection_AsciiString_Insert_2(int where,Standard_CString what);

final public void Insert(int where,TCollection_AsciiString what) {
    TCollection_AsciiString_Insert_3(where,what);
}

private final native void TCollection_AsciiString_Insert_3(int where,TCollection_AsciiString what);

native public final void InsertAfter(int Index,TCollection_AsciiString other);
native public final void InsertBefore(int Index,TCollection_AsciiString other);
native public final boolean IsEmpty();
final public boolean IsEqual(Standard_CString other) {
   return TCollection_AsciiString_IsEqual_1(other);
}

private final native boolean TCollection_AsciiString_IsEqual_1(Standard_CString other);

final public boolean IsEqual(TCollection_AsciiString other) {
   return TCollection_AsciiString_IsEqual_2(other);
}

private final native boolean TCollection_AsciiString_IsEqual_2(TCollection_AsciiString other);

final public boolean IsDifferent(Standard_CString other) {
   return TCollection_AsciiString_IsDifferent_1(other);
}

private final native boolean TCollection_AsciiString_IsDifferent_1(Standard_CString other);

final public boolean IsDifferent(TCollection_AsciiString other) {
   return TCollection_AsciiString_IsDifferent_2(other);
}

private final native boolean TCollection_AsciiString_IsDifferent_2(TCollection_AsciiString other);

final public boolean IsLess(Standard_CString other) {
   return TCollection_AsciiString_IsLess_1(other);
}

private final native boolean TCollection_AsciiString_IsLess_1(Standard_CString other);

final public boolean IsLess(TCollection_AsciiString other) {
   return TCollection_AsciiString_IsLess_2(other);
}

private final native boolean TCollection_AsciiString_IsLess_2(TCollection_AsciiString other);

final public boolean IsGreater(Standard_CString other) {
   return TCollection_AsciiString_IsGreater_1(other);
}

private final native boolean TCollection_AsciiString_IsGreater_1(Standard_CString other);

final public boolean IsGreater(TCollection_AsciiString other) {
   return TCollection_AsciiString_IsGreater_2(other);
}

private final native boolean TCollection_AsciiString_IsGreater_2(TCollection_AsciiString other);

native public final int IntegerValue();
native public final boolean IsIntegerValue();
native public final boolean IsRealValue();
native public final boolean IsAscii();
native public final void LeftAdjust();
native public final void LeftJustify(int Width,Standard_Character Filler);
native public final int Length();
final public int Location(TCollection_AsciiString other,int FromIndex,int ToIndex) {
   return TCollection_AsciiString_Location_1(other,FromIndex,ToIndex);
}

private final native int TCollection_AsciiString_Location_1(TCollection_AsciiString other,int FromIndex,int ToIndex);

final public int Location(int N,Standard_Character C,int FromIndex,int ToIndex) {
   return TCollection_AsciiString_Location_2(N,C,FromIndex,ToIndex);
}

private final native int TCollection_AsciiString_Location_2(int N,Standard_Character C,int FromIndex,int ToIndex);

native public final void LowerCase();
native public final void Prepend(TCollection_AsciiString other);
native public final double RealValue();
final public void RemoveAll(Standard_Character C,boolean CaseSensitive) {
    TCollection_AsciiString_RemoveAll_1(C,CaseSensitive);
}

private final native void TCollection_AsciiString_RemoveAll_1(Standard_Character C,boolean CaseSensitive);

final public void RemoveAll(Standard_Character what) {
    TCollection_AsciiString_RemoveAll_2(what);
}

private final native void TCollection_AsciiString_RemoveAll_2(Standard_Character what);

native public final void Remove(int where,int ahowmany);
native public final void RightAdjust();
native public final void RightJustify(int Width,Standard_Character Filler);
final public int Search(Standard_CString what) {
   return TCollection_AsciiString_Search_1(what);
}

private final native int TCollection_AsciiString_Search_1(Standard_CString what);

final public int Search(TCollection_AsciiString what) {
   return TCollection_AsciiString_Search_2(what);
}

private final native int TCollection_AsciiString_Search_2(TCollection_AsciiString what);

final public int SearchFromEnd(Standard_CString what) {
   return TCollection_AsciiString_SearchFromEnd_1(what);
}

private final native int TCollection_AsciiString_SearchFromEnd_1(Standard_CString what);

final public int SearchFromEnd(TCollection_AsciiString what) {
   return TCollection_AsciiString_SearchFromEnd_2(what);
}

private final native int TCollection_AsciiString_SearchFromEnd_2(TCollection_AsciiString what);

final public void SetValue(int where,Standard_Character what) {
    TCollection_AsciiString_SetValue_1(where,what);
}

private final native void TCollection_AsciiString_SetValue_1(int where,Standard_Character what);

final public void SetValue(int where,Standard_CString what) {
    TCollection_AsciiString_SetValue_2(where,what);
}

private final native void TCollection_AsciiString_SetValue_2(int where,Standard_CString what);

final public void SetValue(int where,TCollection_AsciiString what) {
    TCollection_AsciiString_SetValue_3(where,what);
}

private final native void TCollection_AsciiString_SetValue_3(int where,TCollection_AsciiString what);

final public TCollection_AsciiString Split(int where) {
   return TCollection_AsciiString_Split_2(where);
}

private final native TCollection_AsciiString TCollection_AsciiString_Split_2(int where);

final public TCollection_AsciiString SubString(int FromIndex,int ToIndex) {
   return TCollection_AsciiString_SubString_2(FromIndex,ToIndex);
}

private final native TCollection_AsciiString TCollection_AsciiString_SubString_2(int FromIndex,int ToIndex);

native public final Standard_CString ToCString();
final public TCollection_AsciiString Token(Standard_CString separators,int whichone) {
   return TCollection_AsciiString_Token_2(separators,whichone);
}

private final native TCollection_AsciiString TCollection_AsciiString_Token_2(Standard_CString separators,int whichone);

native public final void Trunc(int ahowmany);
native public final void UpperCase();
native public final int UsefullLength();
native public final Standard_Character Value(int where);
native public static int HashCode(TCollection_AsciiString astring,int Upper);
static public boolean IsEqual(TCollection_AsciiString string1,TCollection_AsciiString string2) {
   return TCollection_AsciiString_IsEqual_3(string1,string2);
}

private static native boolean TCollection_AsciiString_IsEqual_3(TCollection_AsciiString string1,TCollection_AsciiString string2);

static public boolean IsEqual(TCollection_AsciiString string1,Standard_CString string2) {
   return TCollection_AsciiString_IsEqual_4(string1,string2);
}

private static native boolean TCollection_AsciiString_IsEqual_4(TCollection_AsciiString string1,Standard_CString string2);

native public static int HASHCODE(TCollection_AsciiString astring,int Upper);
native public static boolean ISSIMILAR(TCollection_AsciiString string1,TCollection_AsciiString string2);


public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
