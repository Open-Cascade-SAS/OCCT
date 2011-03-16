
#define XTRACE

// File         Graphic3d_StructureManager.cxx
// Created      Mars 1992
// Author       NW,JPB,CAL

//-Copyright    MatraDatavision 1991,1992

//-Version      

//-Design       Declaration des variables specifiques aux managers

//-Warning      Un manager manipule un ensemble de structures

//-References   

//-Language     C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_StructureManager.ixx>
#include <Graphic3d_StructureManager.pxx>
static Standard_Boolean Initialisation = Standard_True;
static int StructureManager_ArrayId[StructureManager_MAX];
static Standard_Integer StructureManager_CurrentId = 0;

#include <Graphic3d_Structure.pxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>

//-Aliases

//-Global data definitions

//      -- l'identifieur du manager
//      MyId                    :       Standard_Integer;

//      -- le mode de mise a jour de l'affichage
//      MyUpdateMode            :       TypeOfUpdate;

//      -- les differents contextes de primitives
//      MyAspectLine3d          :       AspectLine3d;
//      MyAspectText3d          :       AspectText3d;
//      MyAspectMarker3d        :       AspectMarker3d;
//      MyAspectFillArea3d      :       AspectFillArea3d;

//      -- les structures affichees
//      MyDisplayedStructure    :       SequenceOfStructure;

//      -- les structures mises en evidence
//      MyHighlightedStructure  :       SequenceOfStructure;

//      -- les structures visibles
//      MyVisibleStructure      :       SequenceOfStructure;

//      -- les structures detectables
//      MyPickStructure         :       SequenceOfStructure;

//      -- le generateur d'identificateurs de structures
//      MyStructGenId           :       GenId;

//-Constructors

Graphic3d_StructureManager::Graphic3d_StructureManager (const Handle(Aspect_GraphicDevice)& aDevice):
MyDisplayedStructure (),
MyHighlightedStructure (),
MyVisibleStructure (),
MyPickStructure () {

Standard_Real Coef;
Standard_Integer i;
Standard_Boolean NotFound       = Standard_True;
Standard_Integer Limit  = Graphic3d_StructureManager::Limit ();

        /* Initialize PHIGS and start up */
        if (Initialisation) {

                Initialisation = Standard_False;
                /* tableau pour gerer les identificateurs de StructureManager */
                for (i=1; i<=Limit; i++) StructureManager_ArrayId[i]    = 0;

                StructureManager_CurrentId      = 1;
                StructureManager_ArrayId[1]     = 1;

        }
        else {
                for (i=1; i<=Limit && NotFound; i++)
                        if (StructureManager_ArrayId[i] == 0) {
                                NotFound        = Standard_False;
                                StructureManager_CurrentId      = i;
                                StructureManager_ArrayId[i]     = 1;
                        }

                if (NotFound)
                        Graphic3d_InitialisationError::Raise
                                        ("Too many ViewManagers are defined");
        }

        Coef            = (Structure_IDMIN+Structure_IDMAX)/Limit;
        Aspect_GenId theGenId(
          Standard_Integer (Structure_IDMIN+Coef*(StructureManager_CurrentId-1)),
          Standard_Integer (Structure_IDMIN+Coef*StructureManager_CurrentId-1));
        MyStructGenId   = theGenId;

        MyId                    = StructureManager_CurrentId;

        MyAspectLine3d          = new Graphic3d_AspectLine3d ();
        MyAspectText3d          = new Graphic3d_AspectText3d ();
        MyAspectMarker3d        = new Graphic3d_AspectMarker3d ();
        MyAspectFillArea3d      = new Graphic3d_AspectFillArea3d ();

        MyUpdateMode            = Aspect_TOU_WAIT;
        MyGraphicDevice         = aDevice;

}

//-Destructors

void Graphic3d_StructureManager::Destroy () {

#ifdef TRACE
        cout << "Graphic3d_StructureManager::Destroy (" << MyId << ")\n";
        cout << flush;
#endif

        MyDisplayedStructure.Clear ();
        MyHighlightedStructure.Clear ();
        MyVisibleStructure.Clear ();
        MyPickStructure.Clear ();
        StructureManager_ArrayId[MyId]  = 0;

}

//-Methods, in order

void Graphic3d_StructureManager::SetUpdateMode (const Aspect_TypeOfUpdate AType) {

        MyUpdateMode    = AType;

}

Aspect_TypeOfUpdate Graphic3d_StructureManager::UpdateMode () const {

        return (MyUpdateMode);

}

void Graphic3d_StructureManager::SetPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& CTX) {

        MyAspectLine3d          = CTX;

}

void Graphic3d_StructureManager::SetPrimitivesAspect (const Handle(Graphic3d_AspectFillArea3d)& CTX) {

        MyAspectFillArea3d      = CTX;

}

void Graphic3d_StructureManager::SetPrimitivesAspect (const Handle(Graphic3d_AspectText3d)& CTX) {

        MyAspectText3d          = CTX;

}

void Graphic3d_StructureManager::SetPrimitivesAspect (const Handle(Graphic3d_AspectMarker3d)& CTX) {

        MyAspectMarker3d        = CTX;

}

void Graphic3d_StructureManager::PrimitivesAspect (Handle(Graphic3d_AspectLine3d)& CTXL, Handle(Graphic3d_AspectText3d)& CTXT, Handle(Graphic3d_AspectMarker3d)& CTXM, Handle(Graphic3d_AspectFillArea3d)& CTXF) const {

        CTXL    = MyAspectLine3d;
        CTXT    = MyAspectText3d;
        CTXM    = MyAspectMarker3d;
        CTXF    = MyAspectFillArea3d;

}

Handle(Graphic3d_AspectLine3d) Graphic3d_StructureManager::Line3dAspect () const {

        return (MyAspectLine3d);

}

Handle(Graphic3d_AspectText3d) Graphic3d_StructureManager::Text3dAspect () const {

        return (MyAspectText3d);

}

Handle(Graphic3d_AspectMarker3d) Graphic3d_StructureManager::Marker3dAspect () const {

        return (MyAspectMarker3d);

}

Handle(Graphic3d_AspectFillArea3d) Graphic3d_StructureManager::FillArea3dAspect () const {

        return (MyAspectFillArea3d);

}

void Graphic3d_StructureManager::Remove (const Standard_Integer AnId) {

#ifdef TRACE
        cout << "Graphic3d_StructureManager::Remove " << AnId << "\n" << flush;
#endif

        MyStructGenId.Free (AnId);

}

void Graphic3d_StructureManager::Visible (const Handle(Graphic3d_Structure)& AStructure) {

  MyVisibleStructure.Add(AStructure);

}

void Graphic3d_StructureManager::Invisible (const Handle(Graphic3d_Structure)& AStructure) {

  MyVisibleStructure.Remove(AStructure);
 
}

void Graphic3d_StructureManager::Detectable (const Handle(Graphic3d_Structure)& AStructure) {

  MyPickStructure.Add(AStructure);

}

void Graphic3d_StructureManager::Undetectable (const Handle(Graphic3d_Structure)& AStructure) {

  MyPickStructure.Remove(AStructure);
 
}

void Graphic3d_StructureManager::DisplayedStructures (Graphic3d_MapOfStructure& SG) const {

  SG.Assign(MyDisplayedStructure);

  //JMBStandard_Integer Length  = MyDisplayedStructure.Length ();

  //JMBfor (Standard_Integer i=1; i<=Length; i++)
  //JMB SG.Add (MyDisplayedStructure.Value (i));

}

Standard_Integer Graphic3d_StructureManager::NumberOfDisplayedStructures () const {

Standard_Integer Length = MyDisplayedStructure.Extent ();

        return (Length);

}

//Handle(Graphic3d_Structure) Graphic3d_StructureManager::DisplayedStructure (const Standard_Integer AnIndex) const {

//return (MyDisplayedStructure.Value (AnIndex));

//}

void Graphic3d_StructureManager::HighlightedStructures (Graphic3d_MapOfStructure& SG) const {

  SG.Assign(MyHighlightedStructure);

}

void Graphic3d_StructureManager::PickStructures (Graphic3d_MapOfStructure& SG) const {

  SG.Assign(MyPickStructure);

}

void Graphic3d_StructureManager::VisibleStructures (Graphic3d_MapOfStructure& SG) const {

  SG.Assign(MyVisibleStructure);


}

void Graphic3d_StructureManager::MinMaxValues (Standard_Real& XMin, Standard_Real& YMin, Standard_Real& ZMin, Standard_Real& XMax, Standard_Real& YMax, Standard_Real& ZMax) const {

Standard_Boolean Flag = Standard_True;
Standard_Real Xm, Ym, Zm, XM, YM, ZM, RL, RF;

        RL = RealLast ();
        RF = RealFirst ();

        XMin = YMin = ZMin = RL;
        XMax = YMax = ZMax = RF;

        Graphic3d_MapIteratorOfMapOfStructure it(MyDisplayedStructure);
        for (; it.More(); it.Next()) {
          Handle(Graphic3d_Structure) SG = it.Key();
          if (! (SG->IsEmpty() || SG->IsInfinite ())) {
            SG->MinMaxValues (Xm, Ym, Zm, XM, YM, ZM);
            if (Xm < XMin) XMin = Xm;
            if (Ym < YMin) YMin = Ym;
            if (Zm < ZMin) ZMin = Zm;
            if (XM > XMax) XMax = XM;
            if (YM > YMax) YMax = YM;
            if (ZM > ZMax) ZMax = ZM;
            Flag = Standard_False;
          }
        }

        // Si toutes les structures sont vides ou infinies
        if (Flag) {
          XMin = YMin = ZMin = RF;
          XMax = YMax = ZMax = RL;
        }
 
}
 
Standard_Integer Graphic3d_StructureManager::NewIdentification () {

Standard_Integer Id     = MyStructGenId.Next ();

#ifdef TRACE
        cout << "Graphic3d_StructureManager::NewIdentification " << Id << "\n";
        cout << flush;
#endif

        return Id;

}

Handle(Graphic3d_Structure) Graphic3d_StructureManager::Identification (const Standard_Integer AId) const {

//  Standard_Integer ind=0;
  Standard_Boolean notfound     = Standard_True;

  Handle(Graphic3d_Structure) StructNull;
 
  Graphic3d_MapIteratorOfMapOfStructure it( MyDisplayedStructure);
  
  Handle(Graphic3d_Structure) SGfound;

  for (; it.More() && notfound; it.Next()) {
    Handle(Graphic3d_Structure) SG = it.Key();
    if ( SG->Identification () == AId) {
      notfound  = Standard_False;
      SGfound = SG;
    }
  }


  if (notfound)
    return (StructNull);
  else
    return (SGfound);

}

Standard_Integer Graphic3d_StructureManager::Identification () const {
 
        return (MyId);
 
}

Standard_Integer Graphic3d_StructureManager::Limit () {

        return (StructureManager_MAX);

}

Standard_Integer Graphic3d_StructureManager::CurrentId () {

        return (StructureManager_CurrentId);

}

Handle(Aspect_GraphicDevice) Graphic3d_StructureManager::GraphicDevice () const {

        return (MyGraphicDevice);

}
