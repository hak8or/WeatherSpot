/*
    Author:     Piotr Kapela    [https://github.com/pkapela]
    Name:       WeatherSpot
    Created:    Jun/5/2015
    Desc:       WeatherSpot is a client type application which connects to weatherspot.us
                website to fetch and display the data on a graph. The data itself is gathered 
                by embedded devices which monitor and record into Influx Database properties 
                like temperature, humidity or pressure. In addition, the application includes 
                console feature which allows manual querries into the database. The elementary
                statistics section was added for the convinience of a user. This program 
                uses two open source libraries JSON.NET for deserialization of JSON data and 
                Dynamic Data Display for graphs plotting.  
*/

using Newtonsoft.Json.Linq;
using System;

namespace WeatherSpot
{
    static class HelperClass
    {
        static public void stringFormatter(ref string inputStr)
        {
            for(int i = 0; i < inputStr.Length; i++)
            {
                if (inputStr[i].Equals('[') == true)
                {
                    inputStr = inputStr.Remove(i, (i + 1));
                    break;
                }
            }

            for(int i = (inputStr.Length - 1); i >= 0; i--)
            {
                if(inputStr[i].Equals(']') == true)
                {
                    inputStr = inputStr.Remove(i);
                    break;
                }
            }
        } // end of method

    } // end of class

} // end of namespace
