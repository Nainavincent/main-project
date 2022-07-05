/**********************/
// #include <WiFi.h>
#include <time.h>
#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(13, 12, 14, 27, 26, 25);
Servo servo_1;
Servo servo_2;
Servo servo_3;

/*****************************/
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
/*****************************/
#define WIFI_SSID "GNXS-3D42F0"
#define WIFI_PASSWORD "BC62D23D42F0"
#define API_KEY "AIzaSyCcLrEC1ohgOj2TQSeH55cEAVEhp-WLh14"
#define DATABASE_URL "medcine-remainder-default-rtdb.firebaseio.com"
#define USER_EMAIL "projectmedicine21@gmail.com"
#define USER_PASSWORD "promed21"
/*****************************/
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
/*****************************/
   void Wifi_connect();
   void FireBaseGet_Dat();
   int Extract_msg(String msg,char a);
   int calc_Time(int hh,int mm);
   void time_counter();
   void Time();
   void open_morning();
   void open_noon();
   void open_night();
   
/**********************/
    
    int h,m,s,d,mo,y;
    int timezone = 5.5 * 3600;//india GMT +5.30
    int dst = 0;
    int m_d=111;
    int nn_d=111;
    int n_d=111;
    int p;
   int OPEN  = 50;
   int CLOSE = 150;

   int m_pill = 0;
   int nn_pill = 0;
   int n_pill = 0;

   
/**********************/

String M_Time,M_Med;
int M_HH  = 0,M_MM  = 0;
int NN_HH = 0,NN_MM = 0;
int N_HH  = 0,N_MM  = 0;

int a_m=0,a_nn=0,a_n=0;
int b_m=0,b_nn=0,b_n=0;
int c_m=0,c_nn=0,c_n=0;


int pt,ot,dt=0;
int mrng=0,noon=0,night=0;
/**********************/
char buffer[60];
/**********************/
void setup()
{
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.print("Auto   Medcine");
    lcd.setCursor(0,1);
    lcd.print("  Dispenser");
    pinMode(23,OUTPUT);
    digitalWrite(23,LOW);
    servo_1.attach(21);
    servo_2.attach(19);
    servo_3.attach(18);
    
    servo_1.write(CLOSE);
    servo_2.write(CLOSE);
    servo_3.write(CLOSE);
    
    delay(2000);
    
    Wifi_connect();
    pt=ot=millis()/100;
}
/**********************/
void loop() 
{
  Time();
  time_counter();
  /*sprintf(buffer,"Current Date and Time%d:%d:%d   %d/%d/%d ",h,m,s,d,mo,y);
  Serial.println(buffer);*/
  delay(500);
}
/--------------------------------------------------------/
void Time()
{
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
 
                h=  p_tm->tm_hour;
                m=  p_tm->tm_min;  
                s=  p_tm->tm_sec;

                d  = p_tm->tm_mday;
                mo = p_tm->tm_mon + 1;
                y  = p_tm->tm_year + 1900;
}
               
/--------------------------------------------------------/
    void Wifi_connect()
    {
      int i=0;
          Serial.println("");
          lcd.begin(16, 2);
          lcd.print("WiFi Connecting");
          lcd.setCursor(0,1);
          lcd.print("");
          
          WiFi.disconnect();
          WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
          while ((!(WiFi.status() == WL_CONNECTED)))
          {
            i++;
            delay(300);
            Serial.print("*");
            lcd.print("*");
            if(i>15)
            {
              lcd.begin(16,2);
              lcd.print("WiFi Connecting");
              lcd.setCursor(0,1);
              i=0;
            }
          }
          lcd.begin(16,2);
          lcd.print("CONNECTED");
          Serial.println("\n\nConnected..");
          Serial.println();
          Serial.print("Connected with IP: ");
          Serial.println(WiFi.localIP());
          Serial.println();
          Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
          config.api_key = API_KEY;
          auth.user.email = USER_EMAIL;
          auth.user.password = USER_PASSWORD;
          config.database_url = DATABASE_URL;
          config.token_status_callback = tokenStatusCallback;
          #if defined(ESP8266)
          fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 /, 2048 / Tx buffer size in bytes from 512 - 16384 */);
          #endif
          Firebase.begin(&config, &auth);
          Firebase.reconnectWiFi(true);
          Firebase.setDoubleDigits(5);
          config.timeout.serverResponse = 10 * 1000;
          Serial.println("Getting Time zone info please wait.. \n");
          configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
          delay(100);
                while(!time(nullptr))
                {
                   Serial.print("*");
                   delay(100);
                }
         Serial.println("\nTime response....OK\n");
         delay(800);
    }
/**********************/
   void FireBaseGet_Dat()
   {
      
      
      M_Time = (Firebase.RTDB.getString(&fbdo, F("/Naina/Time")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
      M_Med  = (Firebase.RTDB.getString(&fbdo, F("/Naina/MED")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());

       /* Serial.print("Med time");
          Serial.println(M_Time);
    
          Serial.print("Med dosage");
          Serial.println(M_Med);    */  
        
          M_HH = Extract_msg(M_Time,'A');
          M_MM = Extract_msg(M_Time,'a');

          NN_HH = Extract_msg(M_Time,'B');
          NN_MM = Extract_msg(M_Time,'b');

          N_HH = Extract_msg(M_Time,'C');
          N_MM = Extract_msg(M_Time,'c');

          a_m = Extract_msg(M_Med,'a');
          a_nn= Extract_msg(M_Med,'b');
          a_n = Extract_msg(M_Med,'c');

          b_m = Extract_msg(M_Med,'d');
          b_nn= Extract_msg(M_Med,'e');
          b_n = Extract_msg(M_Med,'f');

          c_m = Extract_msg(M_Med,'g');
          c_nn= Extract_msg(M_Med,'h');
          c_n = Extract_msg(M_Med,'i');

          
          m_d = calc_Time(M_HH,M_MM);
          nn_d = calc_Time(NN_HH,NN_MM);
          n_d = calc_Time(N_HH,N_MM);
          
          /*Serial.print("\n\nMorning time counter =>> ");
          Serial.println(m_d);
          Serial.print("Noon time counter =>> ");
          Serial.println(nn_d);
          Serial.print("Night time counter =>> ");
          Serial.println(n_d);
          
          Serial.print("\n\n\nMorning Time -> ");
          Serial.print(M_HH);
          Serial.print(":");
          Serial.println(M_MM);

          Serial.print("Noon Time -> ");
          Serial.print(NN_HH);
          Serial.print(":");
          Serial.println(NN_MM);
        
          Serial.print("Night Time -> ");
          Serial.print(N_HH);
          Serial.print(":");
          Serial.println(N_MM);

          Serial.print("Medcine-A -> ");
          Serial.print(a_m);
          Serial.print(" , ");
          Serial.print(a_nn);
          Serial.print(" , ");
          Serial.println(a_n);

          Serial.print("Medcine-B -> ");
          Serial.print(b_m);
          Serial.print(" , ");
          Serial.print(b_nn);
          Serial.print(" , ");
          Serial.println(b_n);

          Serial.print("Medcine-C -> ");
          Serial.print(c_m);
          Serial.print(" , ");
          Serial.print(c_nn);
          Serial.print(" , ");
          Serial.println(c_n);
      if(m_d >=-5 && m_d <= 5 && mrng==0)
      {
        Serial.println("Morning pill alert");
      }
      if(nn_d >=-5 && nn_d <= 5 && noon==0)
      {
        Serial.println("Morning pill alert");
      }
        if(n_d >=-5 && n_d <= 5 && night==0)
      {
        Serial.println("Morning pill alert");
      }
      Serial.println("");*/
   }
/**********************/

int Extract_msg(String msg,char a)
{
  
  int num=0,i=0;
  for(i=0;msg[i]!='\0';i++)
  {

    if(msg[i]==a)
    {
      i++;
      while(msg[i] >=48 && msg[i] <= 57)
      {
        num = num*10 +(msg[i]-48);
        i++;
      }
      
      return num;
    }
  }
}
/**********************/
   int calc_Time(int hh,int mm)
   {
      int h_c=0,m_c=111;
  
      h_c=hh-h;
      if(h_c ==0)
      {
        m_c = mm-m;
      }
      else if(h_c == 1)
      {
        m_c=(60-m)+mm;
      }
      return m_c;
   }
/**********************/
void time_counter()
{
  pt=millis()/100;
  if(pt-ot > 15)
  {
    ot=pt;
    dt++;
    if(m_d >=-5 && m_d <= 5 && mrng==0)
    {
        lcd.begin(16, 2);       
        lcd.setCursor(0,0);
        lcd.print("Morning Pill");
        lcd.setCursor(0,1);
        lcd.print("Alert");
        m_pill=1;
        check_face();

            if(p==0)
            {
              lcd.setCursor(8,1);
              lcd.print("Waiting");
              delay(500);
            }
            else if(p==1)
            {
              lcd.setCursor(8,1);
              lcd.print("Naina");
              delay(500);
              open_morning();
            }
            else if(p==2)
            {
              lcd.setCursor(8,1);
              lcd.print("Unknown");
              delay(500);
            }
            digitalWrite(23,HIGH);
            delay(100);
            digitalWrite(23,LOW);
    }
    else if(nn_d >=-5 && nn_d <= 5 && noon  ==0 )
    {
        lcd.begin(16, 2);       
        lcd.setCursor(0,0);
        lcd.print("Noon Pill");
        lcd.setCursor(0,1);
        lcd.print("Alert");
        nn_pill=1;
        check_face();
        if(p==0)
            {
              lcd.setCursor(8,1);
              lcd.print("Waiting");
              delay(500);
            }
            else if(p==1)
            {
              lcd.setCursor(8,1);
              lcd.print("Naina");
              delay(500);
              open_noon();
            }
            else if(p==2)
            {
              lcd.setCursor(8,1);
              lcd.print("Unknown");
              delay(500);
            }
            digitalWrite(23,HIGH);
            delay(100);
            digitalWrite(23,LOW);
    }
    else if(n_d >=-5 && n_d <= 5 && night==0 )
    {
        lcd.begin(16, 2);       
        lcd.setCursor(0,0);
        lcd.print("night Pill");
        lcd.setCursor(0,1);
        lcd.print("Alert");
        n_pill=1;
        check_face();
           if(p==0)
            {
              lcd.setCursor(8,1);
              lcd.print("Waiting");
              delay(500);
            }
            else if(p==1)
            {
              lcd.setCursor(8,1);
              lcd.print("Naina");
              delay(500);
              open_night();
            }
            else if(p==2)
            {
              lcd.setCursor(8,1);
              lcd.print("Unknown");
              delay(500);
            }
            digitalWrite(23,HIGH);
            delay(100);
            digitalWrite(23,LOW);
    }
    else if(dt==1)
    {
      int y1;
        lcd.begin(16, 2);       
        lcd.setCursor(0,0);
        lcd.print(h);
        lcd.print(":");
        lcd.print(m);
        lcd.setCursor(8,0);
        if(d<10)
          lcd.print("0");
        lcd.print(d);
        lcd.print("/");
        if(mo<10)
          lcd.print("0");
        lcd.print(mo);        
        lcd.print("/");
        y1=y%100;
        lcd.print(y1);
        lcd.setCursor(0,1);
        lcd.print("Have a nice day");
    }
    else if(dt==2)
    {
        lcd.begin(16, 2);        
        lcd.print("Morning ");  
        lcd.setCursor(10,0);
        lcd.print("Noon"); 
        lcd.setCursor(0,1);
        lcd.print(M_HH); 
        lcd.print(":"); 
        lcd.print(M_MM);  
        lcd.setCursor(10,1);
        lcd.print(NN_HH); 
        lcd.print(":"); 
        lcd.print(NN_MM); 
        if(m_pill ==1)
        {
          Serial.println("Morning SKPED"); 
          String tag ="";
          char buffer[16];
          tag += "test/MSG/";
          sprintf(buffer,"%d_%d_%d_%d_%d_%d",h,m,s,d,mo,y);
          tag += buffer;
          Firebase.RTDB.setString(&fbdo, tag, "Morning_Skipped");
          delay(500); 
          m_pill =0;      
        }
        if(nn_pill ==1)
        {
          Serial.println("Noon SKPED");  
          String tag ="";
          char buffer[16];
          tag += "test/MSG/";
          sprintf(buffer,"%d_%d_%d_%d_%d_%d",h,m,s,d,mo,y);
          tag += buffer;
          Firebase.RTDB.setString(&fbdo, tag, "Noon_Skipped");
          delay(500); 
          nn_pill =0;       
        }
    }
    else if(dt==3)
    {
      int y1;
        lcd.begin(16, 2);        
        lcd.print("Night "); 
        lcd.setCursor(11,0);
        lcd.print(h);
        lcd.print(":");
        lcd.print(m); 
        lcd.setCursor(0,1);
        lcd.print(N_HH); 
        lcd.print(":"); 
        lcd.print(N_MM);  
        lcd.setCursor(8,1);
        if(d<10)
          lcd.print("0");
        lcd.print(d);
        lcd.print("/");
        if(mo<10)
          lcd.print("0");
        lcd.print(mo);        
        lcd.print("/");
        y1=y%100;
        lcd.print(y1);

        if(n_pill ==1)
        {
          Serial.println("N8 SKPED"); 
          String tag ="";
          char buffer[16];
          tag += "test/MSG/";
          sprintf(buffer,"%d_%d_%d_%d_%d_%d",h,m,s,d,mo,y);
          tag += buffer;
          Firebase.RTDB.setString(&fbdo, tag, "Night_Skipped");
          delay(500); 
          nn_pill =0;        
        }
    }
    if(dt==4)
    {
      p=5;
      lcd.begin(16, 2); 
      digitalWrite(23,LOW);
      lcd.print("Getting Info...");
      FireBaseGet_Dat();
      dt=0;
    }
  }
}
/**********************/
void check_face()
{
 char x; 
  Serial.println("a");
  while(Serial.available())
  {
    x=Serial.read();
    if(x=='p')
    {
      p=Serial.parseInt();
      Serial.print("Persion count >> ");
      Serial.println(p);      
    }
  }
}
/**********************/
void open_morning()
{
  
        lcd.begin(16, 2);        
         m_pill=0;
        if(a_m >0)
        {
          lcd.setCursor(0,0);
          lcd.print("A : ");
          lcd.print(a_m);
          servo_1.write(OPEN);
        }
        else
        {
          lcd.setCursor(0,0);
          lcd.print("A : ");
          lcd.print(a_m);
          servo_1.write(CLOSE);
        }

        if(b_m >0)
        {
          lcd.setCursor(8,0);
          lcd.print("B : ");
          lcd.print(b_m);
          servo_2.write(OPEN);
        }
        else
        {
          lcd.setCursor(8,0);
          lcd.print("B : ");
          lcd.print(b_m);
          servo_2.write(CLOSE);
        }
        if(c_m >0)
        {
          lcd.setCursor(0,1);
          lcd.print("C : ");
          lcd.print(c_m);
          servo_3.write(OPEN);
        }
        else
        {
          lcd.setCursor(0,1);
          lcd.print("C : ");
          lcd.print(c_m);
          servo_3.write(CLOSE);
        }
        Serial.print("Morning_Pill_OK");
        String tag ="";
          char buffer[16];
          tag += "test/MSG/";
          sprintf(buffer,"%d_%d_%d_%d_%d_%d",h,m,s,d,mo,y);
          tag += buffer;
          Firebase.RTDB.setString(&fbdo, tag, "Morning_OK");
          delay(500); 
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("1  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("2  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("3  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("4  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("5  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("6  ");
        
        lcd.begin(16, 2);
        lcd.print("Closing");
        delay(2000);

    servo_1.write(CLOSE);
    servo_2.write(CLOSE);
    servo_3.write(CLOSE);
    mrng=1;
    noon=0;
    night=0;        
}
/**********************/
   void open_noon()
   {
       nn_pill=0;
          lcd.begin(16, 2);        
          if(a_nn >0)
          {
            lcd.setCursor(0,0);
            lcd.print("A : ");
            lcd.print(a_nn);
            servo_1.write(OPEN);
          }
          else
          {
            lcd.setCursor(0,0);
            lcd.print("A : ");
            lcd.print(a_nn);
            servo_1.write(CLOSE);
          }
  
          if(b_nn >0)
          {
            lcd.setCursor(8,0);
            lcd.print("B : ");
            lcd.print(b_nn);
            servo_2.write(OPEN);
          }
          else
          {
            lcd.setCursor(8,0);
            lcd.print("B : ");
            lcd.print(b_nn);
            servo_2.write(CLOSE);
          }
          if(c_nn >0)
          {
            lcd.setCursor(0,1);
            lcd.print("C : ");
            lcd.print(c_nn);
            servo_3.write(OPEN);
          }
          else
          {
            lcd.setCursor(0,1);
            lcd.print("C : ");
            lcd.print(c_nn);
            servo_3.write(CLOSE);
          }
          String tag ="";
          char buffer[16];
          tag += "test/MSG/";
          sprintf(buffer,"%d_%d_%d_%d_%d_%d",h,m,s,d,mo,y);
          tag += buffer;
          Firebase.RTDB.setString(&fbdo, tag, "Noon_OK");
        Serial.print("Noon_Pill_OK");
          delay(1000);
        lcd.setCursor(14,1);
        lcd.print("1  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("2  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("3  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("4  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("5  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("6  ");          


          lcd.begin(16, 2);
          lcd.print("Closing");
          delay(2000);
  
      servo_1.write(CLOSE);
      servo_2.write(CLOSE);
      servo_3.write(CLOSE);
      
      mrng=0;
      noon=1;
      night=0;
   }
/**********************/
   void open_night()
   {
    n_pill=0;
        lcd.begin(16, 2);        
          if(a_n >0)
          {
            lcd.setCursor(0,0);
            lcd.print("A : ");
            lcd.print(a_n);
            servo_1.write(OPEN);
          }
          else
          {
            lcd.setCursor(0,0);
            lcd.print("A : ");
            lcd.print(a_n);
            servo_1.write(CLOSE);
          }
  
          if(b_n >0)
          {
            lcd.setCursor(8,0);
            lcd.print("B : ");
            lcd.print(b_n);
            servo_2.write(OPEN);
          }
          else
          {
            lcd.setCursor(8,0);
            lcd.print("B : ");
            lcd.print(b_n);
            servo_2.write(CLOSE);
          }
          if(c_n >0)
          {
            lcd.setCursor(0,1);
            lcd.print("C : ");
            lcd.print(c_n);
            servo_3.write(OPEN);
          }
          else
          {
            lcd.setCursor(0,1);
            lcd.print("C : ");
            lcd.print(c_n);
            servo_3.write(CLOSE);
          }
         Serial.print("Night_Pill_OK");
         String tag ="";
          char buffer[16];
          tag += "test/MSG/";
          sprintf(buffer,"%d_%d_%d_%d_%d_%d",h,m,s,d,mo,y);
          tag += buffer;
          Firebase.RTDB.setString(&fbdo, tag, "Night_OK");
          delay(1000);
        lcd.setCursor(14,1);
        lcd.print("1  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("2  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("3  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("4  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("5  ");
        delay(1000);
        lcd.setCursor(14,1);
        lcd.print("6  ");

        
          lcd.begin(16, 2);
          lcd.print("Closing");
          delay(2000);
  
      servo_1.write(CLOSE);
      servo_2.write(CLOSE);
      servo_3.write(CLOSE);
      
      mrng=0;
      noon=0;
      night=1;
   }
/**********************/
