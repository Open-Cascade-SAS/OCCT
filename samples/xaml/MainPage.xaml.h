//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include <TopoDS_Shape.hxx>
#include <STEPControl_StepModelType.hxx>

#include "MainPage.g.h"

namespace uwp
{
  /// <summary>
  /// An empty page that can be used on its own or navigated to within a Frame.
  /// </summary>
  public ref class MainPage sealed
  {
  public:
    MainPage();

    void OnClickOffset(Platform::Object^  theSender,
      Windows::UI::Xaml::Input::PointerRoutedEventArgs^ theEvent);

    void OnClickMesh(Platform::Object^  theSender,
      Windows::UI::Xaml::Input::PointerRoutedEventArgs^ theEvent);

    void OnClickBoolean(Platform::Object^  theSender,
      Windows::UI::Xaml::Input::PointerRoutedEventArgs^ theEvent);

    void OnClickDataExchange(Platform::Object^  theSender,
        Windows::UI::Xaml::Input::PointerRoutedEventArgs^ theEvent);

    void OnClickBuildTemporary(Platform::Object^  theSender,
      Windows::UI::Xaml::Input::PointerRoutedEventArgs^ theEvent);

  private:
    // test data exchange export functionality
    Standard_Boolean SaveBREP(const wchar_t* theFilePath, const TopoDS_Shape& theShape);
    Standard_Boolean SaveIGES(const wchar_t* theFilePath, const TopoDS_Shape& theShape);
    Standard_Boolean SaveSTEP(const wchar_t* theFilePath, const TopoDS_Shape& theShape, const STEPControl_StepModelType theValue);
    Standard_Boolean SaveSTL (const wchar_t* theFilePath, const TopoDS_Shape& theShape);
    Standard_Boolean SaveVRML(const wchar_t* theFilePath, const TopoDS_Shape& theShape);

    // test data exchange import functionality
    Standard_Boolean ReadBREP(const wchar_t* theFilePath, TopoDS_Shape& theShape);
    Standard_Boolean ReadIGES(const wchar_t* theFilePath, TopoDS_Shape& theShape);
    Standard_Boolean ReadSTEP(const wchar_t* theFilePath, TopoDS_Shape& theShape);
  };
}
