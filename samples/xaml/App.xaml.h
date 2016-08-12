//
// App.xaml.h
// Declaration of the App class.
//

#pragma once

#include "App.g.h"

namespace uwp
{
  /// <summary>
  /// Provides application-specific behavior to supplement the default Application class.
  /// </summary>
  ref class App sealed
  {
  protected:
    virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ theEvent) override;

  internal:
    App();

  private:
    void OnSuspending(Platform::Object^ theSender, Windows::ApplicationModel::SuspendingEventArgs^ theEvent);
    void OnNavigationFailed(Platform::Object ^theSender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^theEvent);
  };
}
