//Всем привет! Не знаю пригодится кому или нет, но решил провести не большой ликбез по отправки почты из ардуино.
//Для отправки Email понадобится Ethernet Shield - стандартные либы уже есть, и библиотека для кодирования логина
//и пароля в формат "Base64". Ее можно скачать вот тут https://github.com/adamvr/arduino-base64 - 
//там справа внизу кнопка dowland ZIP. Качаем, распаковываем, переименовываем в Base64 и кидаем в папку с библиотеками.
//Вот, в принципе и все приготовления, думаю то, что шилд нужно подключить к маршрутизатору или роутеру, говорить не нужно((((
//Совсем немного теории.
//Вся почта из почтовых сервисов отправляется через спец. сервер smtp(smtp3). Для mail.ru это - smtp.mail.ru,
//для google.ru это - smtp.google.ru и т.д..
//Этот сервер имеет очень не большой ряд команд на которые он отвечает. Список их я приводить не буду, кому интересно гуглите.
//Так вот, отправляя поочередно команды на сервер мы заставляем его отправлять сообщения. 
//Код очень легок для понимания и расписывать по строчкам его не буду, приведу сразу листинг, 
//там есть комменты, не запутаетесь))))))
#include <Base64.h>
#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <util.h>

//структура для сообщения
struct Message
{
  String From;//адрес отправителя
  String To;// адрес получателя
  String Subject;// Тема письма
  String Body;// Текст письма
};

//mac адрес устройства (можно не менять)
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x05 };

//IP адрес адруины (можно не менять)   
IPAddress ip(192,168,0,54);

// IP smtp сервера
IPAddress server(94, 100, 177, 1);// mail.ru
// IP других сереров можно найти в инете 

// логин и пароль к почтовому сервису
char login[] = "ВашЛогин";//без @mail.ru
char password[] = "ВашПароль";

// создаем экземпляр клиента
EthernetClient client;

void setup(){
  Serial.begin(9600);// UART
  Ethernet.begin(mac,ip);// Запускаем шилд
  Serial.println("Start");
}

void loop(){
  while(Serial.available()){
    char inSerialChar = 0;
    inSerialChar = Serial.read();
    if(inSerialChar == 's'){
      SendMessage();// При отправке в дуину 's', отправляем сообщение.
    }
  }
}
// метод подключения к серверу
bool connectToServer(){
  if(client.connected()){
  }
  else{
    if(client.connect(server,25)){
      return true;
    }
    else{
      return false;
    }
  }
}

// метод отправки сообщения
void SendMessage(){
  Serial.println("Please wite...");
  LogPass();
  Message message = {
    "ваш_почтовый_ящик@mail.ru",
    "почтовый_ящик_получателя@gmail.com",
    "Проба пера",//тема
    "Огромный привет от Ардуино!!!!" };//само сообщение
  client.println("MAIL From:<"+message.From+">");
  delay(250);
  client.println("RCPT To:<"+message.To+">");
  delay(250);
  client.println("DATA");
  delay(250);
  client.println("From:"+message.From);
  client.println("To:"+message.To);
  client.println("Subject:"+message.Subject);
  delay(250);
  client.println(message.Body);
  delay(250);
  client.println(".");// по приходу точки сервер ставит сообщение в очередь
  delay(250); // на отправку
  client.println("QUIT");// как только закрыли соединение, сообщение улетает
  delay(250);
  Serial.println("Message sending...");
}

// подключение к сереверу и авторизация
void LogPass(){
  char array[64];// массив для кодирования
  if(connectToServer()){
    client.println(F("helo 1.2.3.4"));
    delay(250);
    client.println(F("AUTH LOGIN"));
    delay(250);
    client.println(strToBase64(login));
    delay(250);
    client.println(strToBase64(password));
    delay(250);

  }
  else{
    Serial.println("Server not connected...");
  }

}

// кодировка Base64
String strToBase64(char str[]){
  char charArray[64];
  base64_encode(charArray,str,strlen(str));
  String result = charArray;
  return result;
}