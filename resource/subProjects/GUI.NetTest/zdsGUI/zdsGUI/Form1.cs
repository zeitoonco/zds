using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Newtonsoft.Json;
using System.Threading;
using System.Net.WebSockets;

namespace zdsGUI
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            string a = txt1.Text;
            //var j = JsonParser.Deserialize(a);
            //txt2.Text = j;

        }

        private void button3_Click(object sender, EventArgs e)
        {
            //var c = JsonParser.Deserialize<installInfo>(txt1.Text);
            var c = JsonConvert.DeserializeObject<DSInstallInfo>(txt1.Text);
            txt2.Text = c.commands[2].name;
           // txt2.Text = c.aa + " , " + c.bb.ToString() + " , " + c.cc.ToString();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            //var dogList = JsonParser.Deserialize<DogList>(@"{""dogs"":[{""name"":""dog0""},{""name"":""dog1""}]}");
            //var dogArray = JsonParser.Deserialize<DogArray>(@"{""dogs"":[{""name"":""dog0""},{""name"":""dog1""}]}");

        }

        private async void button5_Click(object sender, EventArgs e)
        {
            Start();
        }
        ClientWebSocket socket = new ClientWebSocket();
        CancellationTokenSource cts = new CancellationTokenSource();

        public async Task Start()
        {

            string wsUri = "ws://192.168.1.32:5455";
            var t1= socket.ConnectAsync(new Uri(wsUri), cts.Token);
          
            //cts.CancelAfter(10);
            try
            {
                await t1;
            }
            catch (TaskCanceledException tcEx)
            {
                this.Invoke(new showDataDLG(showData), "Task Cancelled: " + tcEx.Message);
                return;
            }
            catch (Exception theEx)
            {
                this.Invoke(new showDataDLG(showData), "Exception: " + theEx.Message);
                return;
            }


            label2.Text=socket.State.ToString();

            await Task.Factory.StartNew(
                async () =>
                {
                    var rcvBytes = new byte[10000];
                    var rcvBuffer = new ArraySegment<byte>(rcvBytes);
                    while (true)
                    {
                        WebSocketReceiveResult rcvResult = await socket.ReceiveAsync(rcvBuffer, cts.Token);
                        byte[] msgBytes = rcvBuffer.Skip(rcvBuffer.Offset).Take(rcvResult.Count).ToArray();
                        string rcvMsg = Encoding.UTF8.GetString(msgBytes);
                        this.Invoke(new showDataDLG(showData), rcvMsg);
                    }
                }, cts.Token, TaskCreationOptions.LongRunning, TaskScheduler.Default);
            this.Invoke(new showDataDLG(showData), "R END.");

            /*while (true)
            {
                var message = Console.ReadLine();
                if (message == "Bye")
                {
                    cts.Cancel();
                    return;
                }
                byte[] sendBytes = Encoding.UTF8.GetBytes(message);
                var sendBuffer = new ArraySegment<byte>(sendBytes);
                await socket.SendAsync(sendBuffer, WebSocketMessageType.Text, endOfMessage: true, cancellationToken: cts.Token);
            }*/
        }

        private async void button6_Click(object sender, EventArgs e)
        {
            byte[] sendBytes = Encoding.UTF8.GetBytes(textBox2.Text);
            var sendBuffer = new ArraySegment<byte>(sendBytes);
            await socket.SendAsync(sendBuffer, WebSocketMessageType.Text, endOfMessage: true, cancellationToken: cts.Token);
        }
        delegate void showDataDLG(string dt);
        public void showData(string data)
        {
            txt2.Text = data+ "\r\n"+ txt2.Text;
        }
    }

    public class testC
    {
        public string aa { get; set; }
        public int bb { get; set; }
        public bool cc { get; set; }
        public override string ToString()
        {
            return "{"+aa+","+bb.ToString()+"}";
        }
    }

    public class DogList
    {
        public List<Dog> Dogs { get; set; }
    }
    public class Dog
    {
        public string Name { get; set; }
    }
    public class DogArray
    {
        public Dog[] Dogs { get; set; }
    }
}