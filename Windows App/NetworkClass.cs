/*
    Author:     Piotr Kapela    [https://github.com/pkapela]
    Program:    WeatherSpot
    Created:    Jun/5/2015
    File:       NetworkClass.cs
    Desc:       WeatherSpot is a client type application which connects to weatherspot.us
                website to fetch and display the data on a graph. The data itself is gathered 
                by embedded devices which monitor and record into Influx Database properties 
                like temperature, humidity or pressure. In addition, the application includes 
                console feature which allows manual querries into the database. The elementary
                statistics section was added for the convinience of a user. This program 
                uses two open source libraries JSON.NET for deserialization of JSON data and 
                Dynamic Data Display for graphs plotting.  
*/

using System.IO;
using System.Net;

namespace WeatherSpot
{
    class NetworkClass
    {
        static public string serverResponse(string dbQuerry)
        {
            HelperClass.serverStringFormatter(ref dbQuerry);

            WebRequest serverResponse;
            serverResponse = WebRequest.Create(dbQuerry);
            /*
                    WebProxy myProxy = new WebProxy("myproxy", 80);
                    myProxy.BypassProxyOnLocal = true;

                    wrGETURL.Proxy = WebProxy.GetDefaultProxy();    
           */
            Stream objStream;
            string strLine = "";

            objStream = serverResponse.GetResponse().GetResponseStream();
            StreamReader objReader = new StreamReader(objStream);
        
            strLine = objReader.ReadLine();
            strLine = objReader.ReadLine();

            HelperClass.stringFormatter(ref strLine);
               
            return strLine;

        } // end of method

    } // end of class

} // end of namespace
