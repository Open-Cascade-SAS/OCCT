using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace IE_WPF_D3D
{
	/// <summary>
	/// Interaction logic for About.xaml
	/// </summary>
	public partial class AboutDialog : Window
	{
		public AboutDialog()
		{
			this.InitializeComponent();

            CommandBinding aBind_Ok = new CommandBinding( IECommands.AboutOk );
            aBind_Ok.Executed += OkCommand_Executed;
            aBind_Ok.CanExecute += OkCommand_CanExecute;
            CommandBindings.Add( aBind_Ok );
		}
  
        private void OkCommand_Executed( object sender, ExecutedRoutedEventArgs e )
        {
            this.Close();
        }

        private void OkCommand_CanExecute( object sender, CanExecuteRoutedEventArgs e )
        {
            e.CanExecute = true;
        }
    }
}