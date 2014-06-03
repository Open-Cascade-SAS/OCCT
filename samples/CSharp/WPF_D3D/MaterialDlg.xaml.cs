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
    public enum Material
    {
        Brass,
        Bronze,
        Copper,
        Gold,
        Pewter,
        Plaster,
        Plastic,
        Silver
    }

	/// <summary>
	/// Interaction logic for MaterialDlg.xaml
	/// </summary>
	public partial class MaterialDlg : Window
	{
		public MaterialDlg( OCCTProxyD3D theView )
		{
			this.InitializeComponent();

            if ( theView == null )
            {
                MessageBox.Show( "Fatal Error during the graphic initialisation", "Error!" );
            }

            View = theView;

            SetInitialState();
		}

        public OCCTProxyD3D View { get; private set; }

        private void PlasterBtn_Checked( object sender, RoutedEventArgs e )
        {
            View.SetMaterial( (int)Material.Plaster );
            View.UpdateCurrentViewer();
        }

        private void BrassBtn_Checked( object sender, RoutedEventArgs e )
        {
            View.SetMaterial( (int)Material.Brass );
            View.UpdateCurrentViewer();
        }

        private void BronzeBtn_Checked( object sender, RoutedEventArgs e )
        {
            View.SetMaterial( (int)Material.Bronze );
            View.UpdateCurrentViewer();
        }

        private void CopperBtn_Checked( object sender, RoutedEventArgs e )
        {
            View.SetMaterial( (int)Material.Copper );
            View.UpdateCurrentViewer();
        }

        private void GoldBtn_Checked( object sender, RoutedEventArgs e )
        {
            View.SetMaterial( (int)Material.Gold );
            View.UpdateCurrentViewer();
        }

        private void PewterBtn_Checked( object sender, RoutedEventArgs e )
        {
            View.SetMaterial( (int)Material.Pewter );
            View.UpdateCurrentViewer();
        }

        private void PlasticBtn_Checked( object sender, RoutedEventArgs e )
        {
            View.SetMaterial( (int)Material.Plastic );
            View.UpdateCurrentViewer();
        }

        private void SilverBtn_Checked( object sender, RoutedEventArgs e )
        {
            View.SetMaterial( (int)Material.Silver );
            View.UpdateCurrentViewer();
        }

        private void SetInitialState()
        {
            // TODO
        }
	}
}