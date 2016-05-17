using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Diagnostics;
using WebSocket4Net;

namespace zdsGUI
{
    public class wsConnection
    {
        WebSocket connection;

        public delegate void DLGonReceive(string data);
        public delegate void DLGonCallbackReceive(string name, string id, JObject data);
        public event DLGonReceive onReceive;
        public event DLGonCallbackReceive onCallbackReceive;

        public void connect(string ip, int port)
        {
            if (connection != null)
                disconnect();
            string wsUri = "ws://" + ip + ":" + port.ToString();
            connection = new WebSocket(wsUri);
            //connection.Opened += Connection_Opened;
            //connection.Closed += Connection_Closed;
           // connection.Error += Connection_Error;
            connection.MessageReceived += Connection_MessageReceived;
            connection.Open();
        }

        private void Connection_MessageReceived(object sender, MessageReceivedEventArgs e)
        {
            try
            {
                if (onReceive != null)
                    onReceive(e.Message);
                if (onCallbackReceive != null)
                    if (e.Message[0] == '{')
                    {
                        JObject obj = JObject.Parse(e.Message);
                        if (obj["type"].Value<string>() == "callback")
                        {
                            onCallbackReceive(obj["node"].Value<string>(), obj["id"].Value<string>(), obj["data"].Value<JObject>());
                        }
                    }

            }
            catch (Exception ex)
            {
                Debug.Print("EXCP:" + ex.ToString());
            }

        }

        private void Connection_Error(object sender, SuperSocket.ClientEngine.ErrorEventArgs e)
        {
        }

        private void Connection_Closed(object sender, EventArgs e)
        {
        }

        private void Connection_Opened(object sender, EventArgs e)
        {
        }

        public void disconnect()
        {
            if (state == WebSocketState.Open)
            {
                connection.Close();
            }
        }

        public void sendCall(string inode, object strct = null)
        {
            Random r = new Random();
            string ss = "";
            if (strct == null)
            {
                ss = JsonConvert.SerializeObject(new { type = "call", node = inode, id = r.Next(10000, 1000000000).ToString() });
            }
            else
            {
                ss = JsonConvert.SerializeObject(new { type = "call", node = inode, id = r.Next(10000, 1000000000).ToString(), data = strct });

            }
            send(ss);
        }
        public void send(string data)
        {
            if (state == WebSocketState.Open)
                connection.Send(data);
            else
                throw new Exception("connection is closed");
        }



        public WebSocketState state
        {
            get
            {
                return (connection == null ? WebSocketState.Closed : connection.State);
            }
        }

    }
}
