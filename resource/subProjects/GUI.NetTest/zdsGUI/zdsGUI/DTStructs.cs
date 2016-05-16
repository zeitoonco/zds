using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace zdsGUI
{
    class DSLoginInfo
    {
        public string username { get; set; }
        public string password { get; set; }
    }

    public class DSInstallInfo
    {
        public class DSCommandDetail
        {
            public string name { get; set; }
            public string inputDatatype { get; set; }
            public int inputDatatypeVersion { get; set; }
            public string outputDatatype { get; set; }
            public int outputDatatypeVersion { get; set; }
            public string permissionRequired { get; set; }

        }
        public class DSEventDetail
        {
            public string name { get; set; }
            public string datatype { get; set; }
            public int datatypeVersion { get; set; }

        }
        public class DSHookDetail
        {
            public string name { get; set; }
            public string datatype { get; set; }
            public int datatypeVersion { get; set; }

        }
        public class DSInstallInfoCommandCallDetail
        {
            public string name { get; set; }
            public string inputDatatype { get; set; }
            public int inputDatatypeVersion { get; set; }
            public string outputDatatype { get; set; }
            public int outputDatatypeVersion { get; set; }

        }
        public class DSInstallInfoRequirementDetail
        {
            public string name { get; set; }
            public int version { get; set; }

        }
        public class DSInstallInfoDatatypesDetail
        {
            public string name { get; set; }
            public int version { get; set; }
        }
        public string name { get; set; }
        public string name2 { get; set; }
        public int version { get; set; }
        public int minSupportedVersion { get; set; }
        public string serviceType { get; set; }
        public List<DSCommandDetail> commands = new List<DSCommandDetail>();
        public List<DSEventDetail> events = new List<DSEventDetail>();
        public List<DSHookDetail> hooks = new List<DSHookDetail>();
        public List<DSInstallInfoCommandCallDetail> calls = new List<DSInstallInfoCommandCallDetail>();
        public List<DSInstallInfoRequirementDetail> installRequirements = new List<DSInstallInfoRequirementDetail>();
        public List<DSInstallInfoRequirementDetail> enableRequirements = new List<DSInstallInfoRequirementDetail>();
        public List<DSInstallInfoDatatypesDetail> datatypes = new List<DSInstallInfoDatatypesDetail>();
    }


    public class DSPermission
    {
        public string permissionID { get; set; }
        public string name { get; set; }
        public string title { get; set; }
        public string description { get; set; }
        public string parentID { get; set; }
    }
}
