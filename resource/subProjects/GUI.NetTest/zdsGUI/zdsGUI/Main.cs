using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using WebSocket4Net;
using Newtonsoft.Json.Linq;

namespace zdsGUI
{
    public partial class Main : Form
    {
        //GLOBAl
        public static wsConnection connection = new wsConnection();
        public static string username;
        public static string password;
        public static string sessionId;
        public static string wsServerIp;
        public static int wsServerPort;

        //types
        class commandTimes
        {
            public DateTime lastSend;
            public DateTime lastReceive;
        }
        class lstSrvItem
        {
            public lstSrvItem(string id, string name)
            {
                ID = id;
                Name = name;
                valid = true;
            }
            public string ID { get; set; }
            public string Name { get; set; }
            public bool valid { get; set; }
            public override string ToString()
            {
                return Name;
            }
        }
        delegate void datareceiveDLG(string node, string id, JObject data);

        //vars
        WebSocketState lastState = WebSocketState.None;
        commandTimes extListLoad = new commandTimes();
        JObject srvList;
        Dictionary<string, Panel> psrvList = new Dictionary<string, Panel>();

        #region Form

        public Main()
        {
            InitializeComponent();
        }

        private void Main_Load(object sender, EventArgs e)
        {
            connection.onCallbackReceive += Connection_onReceive;
            foreach (Panel c in splitContainerMain.Panel2.Controls)
            {
                c.Visible = false;
                psrvList.Add(c.Name, c);
            }
        }

        private void Main_FormClosing(object sender, FormClosingEventArgs e)
        {
            Application.Exit();
        }

        private void ssReconnect_Click(object sender, EventArgs e)
        {
            if (connection.state == WebSocketState.Closed)
                connection.connect(wsServerIp, wsServerPort);
        }

        private void tmrConnectionCheck_Tick(object sender, EventArgs e)
        {
            WebSocketState st = connection.state;
            if (st != lastState)
            {
                if (st == WebSocketState.Closed)
                {
                    ssReconnect.Visible = true;
                    ssConnectionStatus.Image = zdsGUI.Properties.Resources.bullet_red;
                }
                else if (st == WebSocketState.Open)
                {
                    ssReconnect.Visible = false;
                    ssConnectionStatus.Image = zdsGUI.Properties.Resources.bullet_green;
                }
                else
                {
                    ssReconnect.Visible = false;
                    ssConnectionStatus.Image = zdsGUI.Properties.Resources.bullet_yellow;
                }
                lastState = st;
            }
            if (connection.state == WebSocketState.Open)
            {
                if (extListLoad.lastSend == null ||
                    (((DateTime.Now - extListLoad.lastSend).TotalSeconds > 5) &&
                    (DateTime.Now - extListLoad.lastReceive).TotalSeconds > 120))
                {
                    connection.sendCall("_core.getListOfServices");
                    extListLoad.lastSend = DateTime.Now;
                }
            }
        }

        private void lstSrv_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lstSrv.SelectedIndex < 0)
                return;
            lstSrvItem seli = lstSrv.SelectedItem as lstSrvItem;
            foreach (Panel c in splitContainerMain.Panel2.Controls)
            {
                if (c.Name == seli.ID)
                    c.Visible = true;
                else
                    c.Visible = false;
            }
        }


        #endregion

        #region netReceive
        private void Connection_onReceive(string node, string id, JObject data)
        {
            BeginInvoke(new datareceiveDLG(datareceive), node, id, data);
        }
        private void datareceive(string node, string id, JObject data)
        {
            switch (node)
            {
                case "_core.getListOfServices":
                    DRgetListOfServices(data);
                    break;
                case "_core.getServiceInfo":
                    DRgetServiceInfo(data);
                    break;
                case "userman.listPermissions":
                    DRlistPermissions(data);
                    break;
                default:
                    break;
            }

        }

        private void DRgetListOfServices(JObject data)
        {
            extListLoad.lastReceive = DateTime.Now;
            lstCoreServices.Items.Clear();
            //un-validate all items in list
            foreach (lstSrvItem item in lstSrv.Items)
            {
                item.valid = false;
            }
            //update list
            var sl = data["extensions"].Children().ToList();
            foreach (var s in sl)
            {
                string sname = s["name"].Value<string>();
                if (psrvList.ContainsKey(sname))
                {
                    bool found = false;
                    foreach (lstSrvItem item in lstSrv.Items)
                    {
                        if (item.ID == sname)
                        {
                            item.valid = true;
                            found = true;
                        }
                    }
                    if (!found)
                        lstSrv.Items.Add(new lstSrvItem(sname, psrvList[sname].Tag.ToString()));
                }
                //add service data to list in core tab
                lstCoreServices.Items.Add(s["name"].Value<string>(), s["name"].Value<string>(), 0).SubItems.AddRange(
                    new string[] { s["version"].Value<string>(), s["state"].Value<string>() });
            }
            //remove items that r not valid anymore
            for (int i = lstSrv.Items.Count - 1; i >= 0; i--)
            {
                if (!(lstSrv.Items[i] as lstSrvItem).valid)
                    lstSrv.Items.RemoveAt(i);
            }
        }

        private void DRgetServiceInfo(JObject data)
        {
            trvInstallInfo.Nodes.Clear();
            //DSInstallInfo ii = data.Value<DSInstallInfo>();
            DSInstallInfo ii = Newtonsoft.Json.JsonConvert.DeserializeObject<DSInstallInfo>(data.ToString());
            TreeNode t1 = trvInstallInfo.Nodes.Add("ServiceInfo");
            t1.Nodes.Add("Name: " + ii.name);
            t1.Nodes.Add("Name 2: " + ii.name2);
            t1.Nodes.Add("serviceType: " + ii.serviceType);
            t1.Nodes.Add("version: " + ii.version);
            t1.Nodes.Add("minSupportedVersion: " + ii.minSupportedVersion);
            TreeNode t2 = t1.Nodes.Add("Commands");
            foreach (var item in ii.commands)
                t2.Nodes.Add(item.name);
            t2 = t1.Nodes.Add("Events");
            foreach (var item in ii.events)
                t2.Nodes.Add(item.name);
            t2 = t1.Nodes.Add("Hooks");
            foreach (var item in ii.hooks)
                t2.Nodes.Add(item.name);
            t2 = t1.Nodes.Add("Calls");
            foreach (var item in ii.calls)
                t2.Nodes.Add(item.name);
            t2 = t1.Nodes.Add("InstallReguirements");
            foreach (var item in ii.installRequirements)
                t2.Nodes.Add(item.name + "[" + item.version + "]");
            t2 = t1.Nodes.Add("EnableReguirements");
            foreach (var item in ii.enableRequirements)
                t2.Nodes.Add(item.name + "[" + item.version + "]");
            t2 = t1.Nodes.Add("Datatypes");
            foreach (var item in ii.datatypes)
                t2.Nodes.Add(item.name + "[" + item.version + "]");
        }

        private void DRlistPermissions(JObject data)
        {
            List<DSPermission> pl = new List<DSPermission>();
            var dplist = data["listPermissions"].ToList();
            foreach (var p in dplist)
                pl.Add(Newtonsoft.Json.JsonConvert.DeserializeObject<DSPermission>(p.ToString()));

            trvPermissions.Nodes.Clear();
            DSPermission p1 = pl.Where(p => (int.Parse(p.parentID) == -1)).ToArray()[0];
            TreeNode t1 = trvPermissions.Nodes.Add(p1.title);
            listPermissionsAddNodes(t1, int.Parse(p1.permissionID), pl);
        }
        private void listPermissionsAddNodes(TreeNode parentTN, int parentID, List<DSPermission> pl)
        {
            DSPermission[] childs = pl.Where(p => (int.Parse(p.parentID) == parentID)).ToArray();
            if (childs.Length == 0)
                return;
            foreach (var child in childs)
            {
                TreeNode ch = parentTN.Nodes.Add(child.title);
                listPermissionsAddNodes(ch, int.Parse(child.permissionID), pl);
            }
        }

        #endregion


        private void btnCoreGetII_Click(object sender, EventArgs e)
        {
            if (lstCoreServices.SelectedItems.Count == 0)
                return;
            connection.sendCall("_core.getServiceInfo", new { name = lstCoreServices.SelectedItems[0].Text });
        }

        private void btnUMplUpdate_Click(object sender, EventArgs e)
        {
            connection.sendCall("userman.listPermissions");
        }
    }
}
