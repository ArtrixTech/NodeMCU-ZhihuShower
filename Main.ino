#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_ssd1306syp.h>

//OLED
#define SDA_PIN 2//oled_SDA
#define SCL_PIN 0//oled_SCL
Adafruit_ssd1306syp display(SDA_PIN, SCL_PIN);

//WIFI
char ssid[] = "Artrix";  // WiFi名 SSID (name)
char pass[] = "23336666";       // WiFi密码

void initOLED() {

  display.initialize();//oled初始化

  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 24);
  display.print("iZHIHU");
  display.setTextSize(1);
  display.setCursor(60, 56);
  display.print("V1.0 ArtrixTech");
  display.update();

}

void connectWIFI() {

  display.clear();
  display.setCursor(0, 0);
  Serial.print("Connecting to ");
  display.print("Connecting to ");
  Serial.println(ssid);
  display.println(ssid);
  WiFi.begin(ssid, pass);
  display.update();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.update();
  }
  Serial.println("");
  display.println("");
  Serial.println("WiFi connected");
  display.println("WiFi connected");
  Serial.println("IP address: ");
  display.println("IP address: ");
  Serial.println(WiFi.localIP());
  display.println(WiFi.localIP());
  display.update();

}

void setup() {

  Serial.begin(115200);//串口波特率115200

  initOLED();
  delay(1500);
  connectWIFI();

}

const char* host = "www.zhihu.com";
String url = "/people/a2010115/activities";
int port = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "8D 76 F2 1B 4A F4 66 F3 10 64 7C 10 8F 03 88 FB 68 ED BC 56";

void loop() {

  WiFiClientSecure client;

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;

  }

  delay(10);

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  String postRequest = String("GET ") + url + " HTTP/1.1\r\n" +
                       "Host: " + host + "\r\n" +
                       "User-Agent: BuildFailureDetectorESP8266\r\n" +
                       "Connection: close\r\n\r\n";
  Serial.println(postRequest);
  client.print(postRequest);  // 发送HTTP请求

  //-------------Initialize---------

  Serial.println(millis());
  String line = client.readStringUntil('>');
  int matchesCount = 4;
  bool matches[matchesCount];

  for (int i = 0; i < matchesCount; i++) {
    matches[i] = false;
  }

  String prev;
  String name, vote, follower, thanked;

  //----------------Analyze-----------
  while (line.length() != 0) {

    if (line.indexOf("RichText ProfileHeader-headline") > 0)  {

      Serial.println(prev);

      int biasFront = 0;
      int biasTail = -2;
      String match = "RichText ProfileHeader-headline";

      int start = 0 + biasFront;
      int end = prev.indexOf("span")  + biasTail;

      String result = prev.substring(start, end);
      Serial.println(result);
      matches[0] = true;

      name = result;

    }

    if (line.indexOf("zhihu:voteupCount") > 0)  {

      Serial.println(line);

      int biasFront = 11;
      int biasTail = -2;
      String match = "zhihu:voteupCount";

      int start = line.indexOf(match) + match.length() + biasFront;
      int end = line.indexOf("data-reactid=")  + biasTail;

      String result = line.substring(start, end);
      Serial.println(result);
      matches[1] = true;

      vote = result;

    }

    if (line.indexOf("zhihu:followerCount") > 0)  {

      Serial.println(line);

      int biasFront = 11;
      int biasTail = -2;
      String match = "zhihu:followerCount";

      int start = line.indexOf(match) + match.length() + biasFront;
      int end = line.indexOf("data-reactid=")  + biasTail;

      String result = line.substring(start, end);
      Serial.println(result);
      matches[2] = true;

      follower = result;

    }

    if (line.indexOf("zhihu:thankedCount") > 0)  {

      Serial.println(line);

      int biasFront = 11;
      int biasTail = -2;
      String match = "zhihu:thankedCount";

      int start = line.indexOf(match) + match.length() + biasFront;
      int end = line.indexOf("data-reactid=")  + biasTail;

      String result = line.substring(start, end);
      Serial.println(result);
      matches[3] = true;

      thanked = result;

    }


    bool finished = true;
    for (int i = 0; i < matchesCount; i++) {
      if (!matches[i]) {
        finished = false;
      }
    }

    if (finished)break;

    prev = line;
    line = client.readStringUntil('>');

  }
  
  //-------------Display---------------
  
  display.clear();
  display.setCursor(0, 0);

  display.print("ZHIHU-");
  display.println(name);

  display.print("VoteUP:");
  display.println(vote);

  display.print("Follower:");
  display.println(follower);

  display.print("Thanked:");
  display.println(thanked);

  display.update();


  /*
    while (client.available()) {
     for (int i = 0; i < 30; i++)Serial.print(client.read());
     Serial.println("  ");
    } */
  client.stop();
  delay(3000);
}
