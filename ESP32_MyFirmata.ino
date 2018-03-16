/* 
Arduino IDE + NodeMCU (ESP32)

Author : ChungYi Fu (Taiwan)  2018-3-17 01:00

Command Format :  ?cmd=str1;str2;str3;str4;str5;str6;str7;str8;str9

Default APIP： 192.168.4.1
http://192.168.4.1/?ip
http://192.168.4.1/?mac
http://192.168.4.1/?resetwifi=id;pwd
http://192.168.4.1/?inputpullup=13
http://192.168.4.1/?pinmode=13;1
http://192.168.4.1/?digitalwrite=13;1
http://192.168.4.1/?analogwrite=13;200
http://192.168.4.1/?digitalread=13
http://192.168.4.1/?analogread=13

STAIP：
Query： http://192.168.4.1/?ip
Link：http://192.168.4.1/?resetwifi=id;pwd

Control Page
https://github.com/fustyles/webduino/blob/master/ESP8266_MyFirmata.html
*/

#include <WiFi.h>

const char* ssid     = "";   //your network SSID
const char* password = "";   //your network password

const char* apssid = "ESP32";
const char* appassword = "12345678";

WiFiServer server(80);

String Feedback="", Command="",cmd="",str1="",str2="",str3="",str4="",str5="",str6="",str7="",str8="",str9="";

void ExecuteCommand()
{
  Serial.println("");
  //Serial.println("Command: "+Command);
  Serial.println("cmd= "+cmd+" ,str1= "+str1+" ,str2= "+str2+" ,str3= "+str3+" ,str4= "+str4+" ,str5= "+str5+" ,str6= "+str6+" ,str7= "+str7+" ,str8= "+str8+" ,str9= "+str9);
  Serial.println("");
  
  if (cmd=="your command")
  {
    // You can do anything
    // Feedback="<font color=red>Hello World</font>";
  }
  else if (cmd=="ip")
  {
    Feedback="AP IP: "+WiFi.softAPIP().toString();    
    Feedback+="<br>";
    Feedback+="STA IP: "+WiFi.localIP().toString();
  }  
  else if (cmd=="mac")
  {
    Feedback+="STA MAC: "+WiFi.macAddress();
  }  
  else if (cmd=="resetwifi")
  {
    WiFi.begin(str1.c_str(), str2.c_str());
    Serial.print("Connecting to ");
    Serial.println(str1);
    long int StartTime=millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if ((StartTime+5000) < millis()) break;
    } 
    Serial.println("");
    Serial.println(WiFi.localIP());
    Feedback=WiFi.localIP().toString();
  }  
  else if (cmd=="inputpullup")
  {
    pinMode(str1.toInt(), INPUT_PULLUP);
    Feedback=Command;
  }  
  else if (cmd=="pinmode")
  {
    pinMode(str1.toInt(), str2.toInt());
    Feedback=Command;
  }        
  else if (cmd=="digitalwrite")
  {
    pinMode(str1.toInt(), OUTPUT);
    digitalWrite(str1.toInt(),str2.toInt());
    Feedback=Command;
  }   
  else if (cmd=="digitalread")
  {
    Feedback=String(digitalRead(str1.toInt()));
  }
  else if (cmd=="analogwrite")
  {
    ledcAttachPin(str1.toInt(), 1);
    ledcSetup(1, 12000, 8);
    ledcWrite(1,str2.toInt());
    Feedback=Command;
  }       
  else if (cmd=="analogread")
  {
    Feedback=String(analogRead(str1.toInt()));
  }
  else 
  {
    Feedback="Command is not defined";
  }             
}

void setup()
{
    Serial.begin(115200);
    delay(10);
    
    WiFi.mode(WIFI_AP_STA);
    
    WiFi.softAP(apssid, appassword);
  
    //WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  
    Serial.println("");
    Serial.println("APIP address: ");
    Serial.println(WiFi.softAPIP());  
  
    //WiFi.config(IPAddress(192, 168, 201, 100), IPAddress(192, 168, 201, 2), IPAddress(255, 255, 255, 0));

    WiFi.begin(ssid, password);

    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    long int StartTime=millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if ((StartTime+5000) < millis()) break;
    } 

    Serial.println("");
    Serial.println("STAIP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
}

void loop(){
 WiFiClient client = server.available();

  if (client) { 
    //Serial.println("New Client.");
    String currentLine = "";

    Command="";cmd="";str1="";str2="";str3="";str4="";str5="";str6="";str7="";str8="";str9="";
    byte ReceiveState=0,cmdState=1,strState=1,questionstate=0,equalstate=0,semicolonstate=0;

    while (client.connected()) {
      
      if (client.available()) {
        char c = client.read();             
        
        if (c=='?') ReceiveState=1;
        if ((c==' ')||(c=='\r')||(c=='\n')) ReceiveState=0;

        if (ReceiveState==1)
        {
          Command=Command+String(c);
          
          if (c=='=') cmdState=0;
          if (c==';') strState++;
        
          if ((cmdState==1)&&((c!='?')||(questionstate==1))) cmd=cmd+String(c);
          if ((cmdState==0)&&(strState==1)&&((c!='=')||(equalstate==1))) str1=str1+String(c);
          if ((cmdState==0)&&(strState==2)&&(c!=';')) str2=str2+String(c);
          if ((cmdState==0)&&(strState==3)&&(c!=';')) str3=str3+String(c);
          if ((cmdState==0)&&(strState==4)&&(c!=';')) str4=str4+String(c);
          if ((cmdState==0)&&(strState==5)&&(c!=';')) str5=str5+String(c);
          if ((cmdState==0)&&(strState==6)&&(c!=';')) str6=str6+String(c);
          if ((cmdState==0)&&(strState==7)&&(c!=';')) str7=str7+String(c);
          if ((cmdState==0)&&(strState==8)&&(c!=';')) str8=str8+String(c);
          if ((cmdState==0)&&(strState>=9)&&((c!=';')||(semicolonstate==1))) str9=str9+String(c);
          
          if (c=='?') questionstate=1;
          if (c=='=') equalstate=1;
          if ((strState>=9)&&(c==';')) semicolonstate=1;
        }
                
        if (c == '\n') {
          if (currentLine.length() == 0) {
            if (Feedback=="") Feedback="Hello World";
              
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(Feedback);
            client.println();
              
            Feedback="";
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if ((currentLine.indexOf("?")!=-1)&&(currentLine.indexOf(" HTTP")!=-1))
        {
          currentLine="";
          Feedback="";
          ExecuteCommand();
        }
      }
    }
    client.stop();
    //Serial.println("Client Disconnected.");
  }
}