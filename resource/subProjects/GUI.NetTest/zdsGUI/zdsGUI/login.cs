using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using WebSocket4Net;

namespace zdsGUI
{
    public partial class login : Form
    {
        delegate void datareceiveDLG(string node, string id, JObject data);

        WebSocketState lastState = WebSocketState.None;
        bool firstTime = true;

        public login()
        {
            InitializeComponent();
        }

        private void login_Load(object sender, EventArgs e)
        {
            Main.connection.onCallbackReceive += Connection_onReceive;
        }


        private void btnCnnct_Click(object sender, EventArgs e)
        {
            Main.connection.connect(txtIP.Text, Int32.Parse(txtPort.Text));
        }

        private void tmrConnectionChecker_Tick(object sender, EventArgs e)
        {
            if (firstTime)
            {
                firstTime = false;
                try
                {
                    Main.connection.connect(txtIP.Text, Int32.Parse(txtPort.Text));
                }
                catch (Exception)
                {
                }
            }
            WebSocketState st = Main.connection.state;
            if (st != lastState)
            {
                if (st == WebSocketState.Closed)
                {
                    btnCnnct.Enabled = true;
                    btnCnnct.Text = "Connect";
                }
                else
                {
                    btnCnnct.Enabled = btnOK.Enabled = false;
                    btnCnnct.Text = Enum.GetName(typeof(WebSocketState), st);
                }
                btnOK.Enabled = (st == WebSocketState.Open);
                lastState = st;
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            Close();
            Application.Exit();
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            if (Main.connection.state == WebSocketState.Open)
            {
                Main.connection.sendCall("userman.login", new DSLoginInfo() { username = txtUsername.Text, password = txtPass.Text });
            }
        }

        private void login_FormClosing(object sender, FormClosingEventArgs e)
        {
            zdsGUI.Properties.Settings.Default.Save();
        }

        private void Connection_onReceive(string node, string id, JObject data)
        {
            BeginInvoke(new datareceiveDLG(datareceive), node, id, data);
        }

        private void datareceive(string node, string id, JObject data)
        {
            if (node == "userman.login")
            {
                if (data["UMLoginResult"].Value<string>() == "ok")
                {
                    doLogin(data["sessionID"].Value<string>());
                }
                else
                {
                    MessageBox.Show(data["description"].Value<string>());
                }
            }
        }

        private void doLogin(string sessionID)
        {
            Main.username = txtUsername.Text;
            Main.password = txtPass.Text;
            Main.sessionId = sessionID;
            Main.wsServerIp = txtIP.Text;
            Main.wsServerPort = int.Parse(txtPort.Text);
            Main m = new Main();
            m.Show();
            tmrConnectionChecker.Stop();
            this.Hide();
            Main.connection.onCallbackReceive -= Connection_onReceive;
        }
    }
}
