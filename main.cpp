#include "NetworkWrapper.h"
#include "json.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;
using namespace nlohmann;

#define USER_AGENT "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.79 Safari/537.36"

string vastr;
string unstr;
vector<string> errdic;

/// Return: -1 Failed, 0 URL broken, 1 URL ok, 2 URL unknown
int CheckURL(const string& url)
{
    string turl=url;
    string content;

    while(true)
    {
        HTTPConnection c;
        c.setUserAgent(USER_AGENT);
        c.setURL(turl);
        c.setDataOutputBuffer(nullptr,0);
        c.setSSLVerifyHost(false);
        c.setSSLVerifyPeer(false);

        //cout<<"Performing on "<<turl<<endl;

        if(c.perform()!=0)
        {
            return -1;
        }

        //cout<<"ResponseCode: "<<c.getResponseCode()<<endl;

        if(c.getResponseCode()!=200)
        {
            if(c.getResponseCode()==302)
            {
                turl=c.getRedirectURL();
                //cout<<"Redirect to "<<turl<<endl;
            }
            else return -1;
        }
        else
        {
            content=string((char*)c.getDataOutputBuffer(),c.getDataOutputBufferLength());
            break;
        }
    }

    /*
    do {
        ofstream ofs("out.txt");
        ofs<<content<<endl;
    }while(0);
    //*/

    if(content.find(vastr)!=string::npos)
    {
        return 1;
    }

    if(content.find(unstr)!=string::npos)
    {
        return 2;
    }

    for(auto& s:errdic)
    {
        if(content.find(s)!=string::npos)
        {
            return 0;
        }
    }

    /// ...
    return 3;
}

int main(int argc,char** argv)
{
    do{
        ifstream ifs("strings_utf8.json");
        string str,tmp;
        while(getline(ifs,tmp)) str.append(tmp);
        json j=json::parse(str);
        vastr=j["ValidContent"];
        unstr=j["UnknownContent"];
        for(auto& s:j["ErrorContent"])
        {
            errdic.push_back(s["content"].get<string>());
        }
    }while(0);

    if(argc==1)
    {
        while(true)
        {
            cout<<"Please input URL to check:"<<endl;
            string url;
            if(getline(cin,url))
            {
                cout<< [&](){switch(CheckURL(url)){
                    case -1:return "Internal Error";
                    case 0:return "URL Broken";
                    case 1:return "URL OK";
                    case 2:return "URL Unknown Status";
                    default:return "Wrong Status";}}() << endl;
            }
            else break;
        }
    }
    else
    {
        cout<< [&](){switch(CheckURL(argv[1])){
            case -1:return "Internal Error";
            case 0:return "URL Broken";
            case 1:return "URL OK";
            case 2:return "URL Unknown Status";
            default:return "Wrong Status";}}() << endl;
    }
}
