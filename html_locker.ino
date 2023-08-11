#include <EtherCard.h>

#define STATIC 0 // set to 1 to disable DHCP (adjust myip/gwip values below)

// mac address
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
// ethernet interface ip address
static byte myip[] = { 192,168,0,5 };
// gateway ip address
static byte gwip[] = { 192,168,0,1 };

// LED to control output

byte Ethernet::buffer[700];
const int ledPin10 = 3;

boolean ledStatus;



char* on = "OPEN";

char* off = "CLOSE";

char* statusLabel;

char* buttonLabel;

void setup () {
pinMode(ledPin10, OUTPUT);

Serial.begin(9600);
Serial.println("Trying to get an IP...");

Serial.print("MAC: ");
for (byte i = 0; i < 6; ++i) {
Serial.print(mymac[i], HEX);
if (i < 5)
Serial.print(':');
}
Serial.println();

if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
{
Serial.println( "Failed to access Ethernet controller");
}
else
{
Serial.println("Ethernet controller access: OK");
}
;

#if STATIC
Serial.println( "Getting static IP.");
if (!ether.staticSetup(myip, gwip)){
Serial.println( "could not get a static IP");
blinkLed(); // blink forever to indicate a problem
}
#else

Serial.println("Setting up DHCP");
if (!ether.dhcpSetup()){
Serial.println( "DHCP failed");
blinkLed(); // blink forever to indicate a problem
}
#endif

ether.printIp("My IP: ", ether.myip);
ether.printIp("Netmask: ", ether.netmask);
ether.printIp("GW IP: ", ether.gwip);
ether.printIp("DNS IP: ", ether.dnsip);
}
void loop () {

  word len = ether.packetReceive();

  word pos = ether.packetLoop(len);

  

  if(pos) {

    

    if(strstr((char *)Ethernet::buffer + pos, "GET /?status=OPEN") != 0) {

      Serial.println("Received ON command");

      ledStatus = true;

    }



    if(strstr((char *)Ethernet::buffer + pos, "GET /?status=CLOSE") != 0) {

      Serial.println("Received OFF command");

      ledStatus = false;

    }

    

    if(ledStatus) {

      digitalWrite(ledPin10, HIGH);
      delay(1000);

      statusLabel = on;

      buttonLabel = off;
      digitalWrite(ledPin10, LOW);
      delay(1000);      

    } else {

      digitalWrite(ledPin10, LOW);
      delay(1000);

      statusLabel = off;

      buttonLabel = on;
     
    }

      

    BufferFiller bfill = ether.tcpOffset();

    bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\n"

      "Content-Type: text/html\r\nPragma: no-cache\r\n\r\n"

      "<html><head><title>WebLed</title></head>"

      "<body>DOOR Status: $S "

      "<a href=\"/?status=$S\"><input type=\"button\" value=\"$S\"></a>"

      "</body></html>"      

      ), statusLabel, buttonLabel, buttonLabel);

    ether.httpServerReply(bfill.position());

  }

}

void blinkLed(){
while (true){
digitalWrite(ledPin10, HIGH);
delay(500);
digitalWrite(ledPin10, LOW);
delay(500);
}
while(1);
}
