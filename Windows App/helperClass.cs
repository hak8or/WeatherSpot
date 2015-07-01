/*
    Author: Piotr Kapela
    Name:   WeatherSpot 
*/

using Newtonsoft.Json.Linq;
using System;

namespace WeatherSpot
{
    static class helperClass
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

            for(int i = (inputStr.Length - 1); i >= 0; i-- )
            {
                if(inputStr[i].Equals(']') == true)
                {
                    inputStr = inputStr.Remove(i);
                    break;
                }
            }
        } // end of methond

    } // end of class

} // end of namespace
