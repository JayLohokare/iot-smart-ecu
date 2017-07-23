#include <TimerOne.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <string.h>

#define ledPin 13
#define tpspin 0
const float e=2.7182818284590452353602874713527;
const float a =49.85; 
const float b =0.1357;
const float c =-474.3;  
const float d = -1.233;  
long int timer1_counter,rpm,val,q;
volatile long int counter = 0;
double timerequired,pwmwidth,tpsd,mfratio;
float vtg,off;

char ssid[] = "jay";
char password[] = "jayjayjay";
 
char topic[] = "command";
char topicrpm[] = "rpm";
char topictpsd[] = "tpsd";
char topicpulse[] = "pulse";

char server[] = "iot.eclipse.org";

char rpms[10];
char pwmwidths[10];
char tpsds[10];

char message[100];
int i, dosend = 0;
 
 
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

void setup()
{  Serial.begin(9600);


   
   pinMode(ledPin, OUTPUT);
   pinMode(tpspin,INPUT);
   Timer1.initialize(1000000);
   Timer1.attachInterrupt(timerIsr); 
   
   
/************************************************************************MQTT*************************
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
  // Generate client name based on MAC address and last 8 bits of microsecond counter
  char clientName[] = "jaylohokare";

  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);
  
  if (client.connect("jaylohokare")) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Topic is: ");
    
    if (client.publish(topic, "Started service")) {
      Serial.println("Publish ok");
      Serial.println("Subscribing to jay2");
      client.subscribe("command");
      //client.subscribe("command");
      
    }
    else {
      Serial.println("Publish failed");
    }     
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }
  

/************************************************************************MQTT*************************/
  

  
}
       
void timerIsr(void)
{  
  noInterrupts();   

      rpm=counter/3; 
      Serial.print("  rpm-  ");
      Serial.println(rpm);
      //counter=0;
     //Serial.println(counter);
      
      
      //for tps calculations 
     val=analogRead(tpspin);
     vtg=.0048*val;
     tpsd=a*pow(e,b*vtg) + c*pow(e,d*vtg)+4.16;
     mfratio=tpsd/86;
     Serial.print(" tpsd- ");
     Serial.print(tpsd);
     
     //pwmwidth
      pwmwidth=mfratio*rpm*0.000827;
      Serial.print(" pwmwidth- ");
      Serial.println(pwmwidth*50);
     //off=150-pwmwidth;
     //Serial.println(off);
      interrupts(); 
   
     
     
      
   digitalWrite(13,HIGH);
   delay(pwmwidth);
   digitalWrite(13,LOW);
   delay(1000 - pwmwidth);
   counter = 0;
  
 
   // char payload[] = "   {\"rpm\":\"";
    
    

char rpms[sizeof(rpm)];
memcpy(&rpms,&rpm,sizeof(rpm));//sprintf (rpms, "%03i", rpm);

char tpsds[20];
//sprintf (tpsds, "%03i", tpsd);
memcpy(&tpsds,&tpsd,sizeof(tpsd));//sprintf (rpms, "%03i", rpm);
  
  
char pwmwidths[20];
//sprintf (pwmwidths, "%03i", pwmwidth);
memcpy(&pwmwidths,&pwmwidth,sizeof(pwmwidth));//sprintf (rpms, "%03i", rpm);
  
  

  if(dosend == 1)
  {
      if (client.connected()){
        Serial.print("Sending payload: ");
        Serial.print("Publish MQTT to client");
        
        if (client.publish(topicrpm, rpms)) {
          Serial.println("Publish ok");
        }
        else {
          Serial.println("Publish failed");
        }
        
         if (client.publish(topictpsd, tpsds)) {
          Serial.println("Publish ok");
        }
        else {
          Serial.println("Publish failed");
        }
        
         if (client.publish(topicpulse, pwmwidths)) {
          Serial.println("Publish ok");
        }
        else {
          Serial.println("Publish failed");
        }
      }
      
  }  
   
   }
   

void loop()
{ 
   if(digitalRead(2)==HIGH)
   {
     counter++;
     digitalWrite(2,LOW);   
    
   }
   
   else
   {
     
   }
   client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
 Serial.print("Message recieved\n");
 for(i=0;i<length;i++)
 {
   message[i]=payload[i];
 }
 message[i] = '\0';
 
 String content =String(message);
 Serial.print(message);
 Serial.print("\n");
 
 
 for(i=0;i<length;i++)
 {
   message[i]=topic[i];
 }
 message[i] = '\0';
 
 String head =String(message);
 Serial.print(message);
 Serial.print("\n");

 if(head = "command")
{
  if(content == "start")
 {
  dosend = 1;
 } 
 
 if(content == "stop")
 {
  dosend = 0; 
 }
}  
}





