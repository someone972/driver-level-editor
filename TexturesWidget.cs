using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace DriverLevelEditor
{
    public partial class TexturesWidget : UserControl
    {
        public TexturesWidget()
        {
            InitializeComponent();

            ddTexSize.SelectedIndex = 0;
            ddTexMode.SelectedIndex = 0;
            ddTexFilter.SelectedIndex = 0;
        }
    }
}
