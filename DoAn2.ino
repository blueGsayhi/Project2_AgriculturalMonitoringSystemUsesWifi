#include <FirebaseESP8266.h>
#include <DHT.h>
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "https://doan2-4ab5c-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "Gf6lsAwjebjTmZi9X08Wc2keJe7tVOiL7ZF4OfaW"
#define WIFI_SSID "Binh" // Thay đổi tên wifi của bạn
#define WIFI_PASSWORD "11102002@@" // Thay đổi password wifi của bạn

const int DHTPIN = D6;
const int DHTTYPE = DHT11;
int relaydat = D1;
int cbdat = A0;
int doc_cb = 0;
int TBcb = 0;
int relaykk = D2;
DHT dht(DHTPIN, DHTTYPE);
FirebaseData fbdo;
FirebaseData firebaseData;

String OFF = "0";
String ON = "1";

String mode = "0";
String relaydatState = "0";
String relaykkState = "0";

void setup() {
  Serial.begin(9600);
  delay(1000);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Dang ket noi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  pinMode(cbdat, INPUT);
  pinMode(relaydat, OUTPUT);
  pinMode(relaykk, OUTPUT);
  digitalWrite(relaydat, LOW);
  digitalWrite(relaykk, LOW);
  dht.begin();
  Serial.println ("");
  Serial.println ("Da ket noi WiFi!");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {

  if (Firebase.getString(firebaseData, "mode")) {
    mode = firebaseData.stringData();
  }

  if (Firebase.getString(firebaseData, "relaydat")) {
    relaydatState = firebaseData.stringData();
  }

  if (Firebase.getString(firebaseData, "relaykk")) {
    relaykkState = firebaseData.stringData();
  }
  

  doc_cb = 0;
  for (int i = 0; i <= 9; i++) {
    doc_cb += analogRead(cbdat);
  }
  TBcb = doc_cb / 10;
  int phantramao = map(TBcb, 0, 1023, 0, 100);
  int phantramthuc = 100 - phantramao;

  int t1 = dht.readTemperature();

  if (mode == "0") {
    Firebase.setString(fbdo, "relaydatauto", OFF);
    Firebase.setString(fbdo, "relaykkauto", OFF);

    if (relaydatState == "1") {
      digitalWrite(relaydat, HIGH);
    } else if (relaydatState == "0") {
      digitalWrite(relaydat, LOW);
    }

    if (relaykkState == "1") {
      digitalWrite(relaykk, HIGH);
    } else if (relaykkState == "0") {
      digitalWrite(relaykk, LOW);
    }
  }

  if (mode == "1") {
    if (phantramthuc >= 30) {
      digitalWrite(relaydat, LOW);
      Firebase.setString(fbdo, "relaydatauto", OFF);
    } else {
      digitalWrite(relaydat, HIGH);
      Firebase.setString(fbdo, "relaydatauto", ON);
    }

    if (t1 < 35) {
      digitalWrite(relaykk, LOW);
      Firebase.setString(fbdo, "relaykkauto", OFF);
    } else {
      digitalWrite(relaykk, HIGH);
      Firebase.setString(fbdo, "relaykkauto", ON);
    }
  }

  // Gửi dữ liệu nhiệt độ và độ ẩm lên Firebase
  Firebase.setFloat(fbdo, "Nhiet do", t1);
  Firebase.setFloat(fbdo, "Do am", phantramthuc);
  Serial.println("Het 1 vong");
  delay(500);
}
