#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <dht.h>
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>

#define sensor 9 
#define button1 A0
#define button1 A1

const int chipSelect = 10;
const int rs = 3, en = 4, d4 = 5, d5 = 6, d6 = 7, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
dht DHT;
tmElements_t tm;

int flag, flag2, flag3, temperature, humidity;
String dataString, monthString;

///////////////////////////////Display/////////////////////////////
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    lcd.print("0");
    //Serial.write('0');
  }
  //Serial.print(number);
  lcd.print(number);
}

void displayLCD_time(){
  lcd.setCursor(2,0);  
  print2digits(tm.Hour); 
  lcd.print(":"); 
  print2digits(tm.Minute); 
  lcd.print(":");
  print2digits(tm.Second); 
  lcd.print(" WIB");
}

void displayLCD_date(){
  convertToMonth(tm.Month);
  lcd.setCursor(2,0);  
  print2digits(tm.Day); 
  lcd.print(" ");  
  lcd.print(monthString);
  lcd.print(" ");  
  lcd.print(tmYearToCalendar(tm.Year));
}

void displayLCD_dht11(){
  lcd.setCursor(0,1);
  lcd.print("SUHU=");
  lcd.print(temperature);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(10,1);
  lcd.print("RH=");
  lcd.print(humidity);
  lcd.print("%");  
}

void convertToMonth(int monthValue){
  if(monthValue==1)monthString="Jan";
  else if(monthValue==2)monthString="Feb";
  else if(monthValue==3)monthString="Mar";
  else if(monthValue==4)monthString="Apr";
  else if(monthValue==5)monthString="Mei";
  else if(monthValue==6)monthString="Jun";
  else if(monthValue==7)monthString="Jul";
  else if(monthValue==8)monthString="Agu";
  else if(monthValue==9)monthString="Sep";
  else if(monthValue==10)monthString="Okt";
  else if(monthValue==11)monthString="Nop";
  else if(monthValue==12)monthString="Des";
}

///////////////////////////////RTC/////////////////////////////
void rtc() {
  if (RTC.read(tm)) {
    /*Serial.print("Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));*/
    dataString = "Time= ";
    dataString += tm.Hour;
    dataString += ":";
    dataString += tm.Minute;
    dataString += ":";
    dataString += tm.Second;

    dataString += " , Date(D/M/Y)= ";
    dataString += tm.Day;
    dataString += ":";
    dataString += tm.Month;
    dataString += ":";
    dataString += tmYearToCalendar(tm.Year);

    dataString += " , Temperature(C)= ";
    dataString += temperature;
    dataString += " , Humidity(RH)= ";
    dataString += humidity;
    //Serial.println(dataString);
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    //delay(9000);
  }
  //delay(1000);
}

///////////////////////////////Sensor/////////////////////////////
void readDHT11(){
  DHT.read11(sensor);
  temperature = (DHT.temperature);
  humidity = (DHT.humidity);
  Serial.print("Kelembaban udara = ");
  Serial.print(DHT.humidity,2);
  Serial.print("% ");
  Serial.print("Suhu = ");
  Serial.print(DHT.temperature,2); 
  Serial.print("C");
  Serial.println("");
}

///////////////////////////////SD Card/////////////////////////////
void write_SD_CARD(){
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    //Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    Serial.println(dataString);
  }  
}


void setup() {
  readDHT11();
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.setCursor(3,0);
  lcd.print("Bismillah");
  Serial.begin(9600);
  Serial.println("Humidity and Temperature Sensor");
  Serial.println("-------------------------------");
  
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  delay(2000);
}

void loop1(){
  if(digitalRead(A0)==0){
    delay(50);
    displayLCD_dht11(); 
  }
  if(digitalRead(A1)==0==0){
    delay(50);
    lcd.clear();
  }
}

void loop(){
  lcd.clear();
  
  //update data real time clock every second
  rtc();
  
  //update humidity and temperature sensor every 2 second
  if(flag>=2){ 
    readDHT11(); 
    flag=0; 
  }

  //Writing rtc and dht11 data to sd card
  if(flag2>=2){ 
    write_SD_CARD();
    flag2=0;
  }

  //display any data on LCD 16x2
  if(flag3>=30&&flag3<=40){
    displayLCD_date();
    displayLCD_dht11();
  }
  else if(flag3>60){
    flag3=0;  
  }
  else{
    displayLCD_time();
    displayLCD_dht11();  
  }
  
  flag++; flag2++; flag3++;
  delay(1000);
}
