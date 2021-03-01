// Import required libraries
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AccelStepper.h>

// Name of wifi Access Point (AP)
const char *ssid = "Turntable";

//String to hold the percentage for the site and other messages
String msg = "";

//define an LED output
const int output_LED = D4;
//define a pin for the BT shutter
const int output_Shutter = D0;
int Move;

//for number of PICS
String sliderValue = "0";

//for direction of rotation
String pause_btn = "";

//for the MOTOR PINS:
#define motorPin1 D2
#define motorPin2 D5
#define motorPin3 D6
#define motorPin4 D7

//Flag to start the main program
bool startPics = false;

//flag for a temporary pause
bool startPause = false;

//variabled for the turntable
const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution

//The new gear ration is 5:1
int FullRev = 5 * stepsPerRevolution; // 1 full revolution of the big gear -> Small-Big gear ratio is 7:1

int PhotoTaken = 0; // Amount of photo's that have been taken
int StepPerPhoto;   // Amount of steps per photo (calculated -> see MenuNr 0/SwMenu 2)

// Use these pins for the stepper motor
AccelStepper myStepper(4, motorPin1, motorPin3, motorPin2, motorPin4);

const char *PARAM_INPUT = "value";
const char *PAUSE_INPUT = "direction";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

IPAddress Ip(10, 10, 10, 1);
IPAddress NMask(255, 255, 255, 0);

// Create an Event Source on /events
AsyncEventSource events("/events");

//save the entire site in memory
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <title>ESP Turntable!</title>
        <style>
            html {
                font-family: Arial;
                display: inline-block;
                text-align: center;
            }
            
            h1 {
                font-size: 2rem;
                margin: 0px 0px 20px 0px;
            }
            
            p {
                font-size: 1.3rem;
                text-align: center;
                padding: 5px 5px;
                margin: 0px 0px 20px 0px;
            }
            
            body {
                font-family: "Dosis", Helvetica, Arial, sans-serif;
                background: #ecf0f1;
                color: #34495e;
                padding-top: 40px;
                text-shadow: white 1px 1px 1px;
                /* width: 50%; */
                /* height: 100vh; */
                max-width: 400px;
                margin: 0px auto;
                padding-bottom: 25px;
                display: flex;
                flex-direction: column;
                justify-content: center;
                align-items: center;
            }
            
            .value {
                border-bottom: 4px dashed #bdc3c7;
                text-align: center;
                font-weight: bold;
                font-size: 10em;
                width: 50%;
                height: 50%;
                line-height: 3em;
                margin: 3em auto;
                letter-spacing: -.07em;
                text-shadow: white 2px 2px 2px;
                transition-duration: 200ms;
            }
            
            input[type="range"] {
                display: block;
                -webkit-appearance: none;
                background-color: #bdc3c7;
                width: 300px;
                height: 5px;
                border-radius: 5px;
                margin: 0 auto;
                outline: 0;
            }
            
            .button {
                background-color: #4CAF50;
                border: none;
                border-radius: 2rem;
                color: white;
                padding: 0.6rem 1rem;
                text-decoration: none;
                font-size: 2rem;
                margin: 2px;
                cursor: pointer;
                transition: background-color 200ms ease-in-out, transform 100ms ease-in-out;
            }

            .button.pause {
                background-color: #47b47d;
            }
            
            .button:active {
                background-color: grey;
                border: none;
            }
            
            .button:hover {
                transform: scale(1.2);
            }
            
            input[type="range"]::-webkit-slider-thumb {
                -webkit-appearance: none;
                background-color: #e74c3c;
                width: 30px;
                height: 30px;
                border-radius: 50%;
                border: 2px solid white;
                cursor: pointer;
                transition: .3s ease-in-out;
            }
            
            ​ input[type="range"]::-webkit-slider-thumb:hover {
                background-color: white;
                border: 2px solid #e74c3c;
            }
            
            input[type="range"]::-webkit-slider-thumb:active {
                transform: scale(1.6);
            }
            
            .value {
                /* border-bottom: 4px dashed #4ba8e6; */
                /* text-align: center; */
                font-weight: bold;
                font-size: 8em;
                width: 300px;
                height: 100px;
                line-height: 60px;
                margin: 40px auto;
                letter-spacing: -.07em;
                text-shadow: white 2px 2px 2px;
                transition: all 200ms;
            }
            
            @media (max-width: 420px),
            (max-height: 400px) {
                html {
                    font-size: 12px;
                }
            }
        </style>

        <script>
            var sliderValue = 2;
        </script>

    </head>

    <body>
        <h1>ESP tableturner!</h2>
            <p>Press the button after setting the amount of pictures desired to start the turntable. Press again to cancel the operation!</p>
            <div id="textSliderValue" class="value">2</div>
            <p><input type="range" onchange="updateSliderPWM(this)" id="pwmSlider" min="2" max="200" value="2" step="2" class="slider" data-show-value="true"></p>

            <p>Current pic:
                <div class="value" id="pic">%COUNTER%<span>&percnt;</span></div>
            </p>

            <p>
                <input type="button" class="button" onclick="sendXHR(this)" value="Start!">
               <input type="button" class="button pause" onclick="sendPAUSE(this)" value="Pause!">

            </p>
            <script>
                if (!!window.EventSource) {
                    var source = new EventSource('/events');

                    source.addEventListener('open', function(e) {
                        console.log("Events Connected");
                    }, false);
                    source.addEventListener('error', function(e) {
                        if (e.target.readyState != EventSource.OPEN) {
                            console.log("Events Disconnected");
                        }
                    }, false);

                    source.addEventListener('message', function(e) {
                        console.log("message", e.data);
                    }, false);

                    source.addEventListener('photocount', function(e) {
                        console.log("photocount", e.data);
                        document.getElementById("pic").innerHTML = e.data;
                    }, false);

                }

                function updateSliderPWM(element) {
                    sliderValue = document.getElementById("pwmSlider").value;
                    document.getElementById("textSliderValue").innerHTML = sliderValue;
                    console.log(sliderValue);
                }

                function sendXHR(element) {
                    sliderValue = document.getElementById("pwmSlider").value;
                    var xhr = new XMLHttpRequest();
                    xhr.open("GET", "/slider?value=" + sliderValue, true);
                    xhr.send();
                    // console.log(sliderValue);

                }

                function sendPAUSE(element) {
                    var xhr = new XMLHttpRequest();
                    xhr.open("GET", "/slider?direction=" + "PAUSE", true);
                    xhr.send();
                    // console.log(sliderValue);
                }

                var elem = document.querySelector('input[type="range"]');

                var rangeValue = function() {
                    var newValue = elem.value;
                    var target = document.querySelector('.value');
                    target.innerHTML = newValue;
                }

                elem.addEventListener("input", rangeValue);
            </script>
    </body>

</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String &var)
{
    //Serial.println(var);
    if (var == "SLIDERVALUE")
    {
        return sliderValue;
    }
    return String();
}

void setup()
{
    // Serial port for debugging purposes
    Serial.begin(9600);

    myStepper.setAcceleration(300.0); //in stpes per second per second
    myStepper.setMaxSpeed(400);       //in steps per second

    pinMode(output_LED, OUTPUT);
    digitalWrite(output_LED, LOW);

    pinMode(output_Shutter, OUTPUT);

    //  Initialize Wi-Fi
    wifi_station_disconnect();
    wifi_set_opmode(0x02); // SETTING IT TO AP MODE
    WiFi.mode(WIFI_AP_STA);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);

    WiFi.softAPConfig(Ip, Ip, NMask);
    WiFi.softAP(ssid);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html, processor);
    });

    // Handle Web Server Events
    events.onConnect([](AsyncEventSourceClient *client) {
        if (client->lastId())
        {
            Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
        }
        // send event with message "hello!", id current millis
        // and set reconnect delay to 10 ms
        client->send("hello!", NULL, millis(), 100);
    });

    // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
    server.on("/slider", HTTP_GET, onWsEvent);

    server.addHandler(&events);

    // Start server
    server.begin();
}

void loop()
{

    if (PhotoTaken < sliderValue.toInt() && startPics)
    {
        // Send Events to the Web Server with the current pic count
        events.send("ping", NULL, millis());
        msg = String(PhotoTaken + 1) + "/" + String(sliderValue.toInt());
        events.send(String(msg).c_str(), "photocount", millis());

        digitalWrite(output_LED, LOW);
        Move = Move + StepPerPhoto;
        myStepper.moveTo(Move);
        myStepper.runToPosition();
        PhotoTaken = PhotoTaken + 1; //Add 1 to photos taken
        digitalWrite(output_LED, HIGH);
        Serial.print("Current pic:");
        Serial.println(PhotoTaken);

        Motor_Delay(1000); //to allow time for phone to take pic
        digitalWrite(output_LED, LOW);
        digitalWrite(output_Shutter,HIGH);
        delay(100);
        digitalWrite(output_Shutter,LOW);
        delay(100);
    }

    if (PhotoTaken == sliderValue.toInt() && startPics)
    { //If the amount of photos taken is equal to the amount of photos that have to be taken -> Program finished

        // Send Events to the Web Server with the current pic count
        events.send("ping", NULL, millis());
        msg = "Done!";
        events.send(String(msg).c_str(), "photocount", millis());

        PhotoTaken = 0;
        Serial.println("Done with pics!");
        startPics = !startPics;
        Motor_Delay(100);

        //LED blink 3 times to show that process is over
        digitalWrite(output_LED, HIGH);
        delay(500);
        digitalWrite(output_LED, LOW);
        delay(500);
        digitalWrite(output_LED, HIGH);
        delay(500);
        digitalWrite(output_LED, LOW);
        delay(500);
        digitalWrite(output_LED, HIGH);
        delay(500);
        digitalWrite(output_LED, LOW);
        delay(500);
    }

    //if the process was canceled or is finished OR PAUSE is pressed
    if (!startPics && !startPause)
    {
        PhotoTaken = 0;
        msg = "Stop";
        events.send(String(msg).c_str(), "photocount", millis());
        delay(200);

        //LED is ON to indicate that software is primed & ready!
        digitalWrite(output_LED, HIGH);
    }

    //Motor is paused and ready to resume
    if (startPause && !startPics)
    {
        //      events.send("ping", NULL, millis());
        digitalWrite(output_LED, LOW);
        msg = "&#10074; &#10074;";
        events.send(String(msg).c_str(), "photocount", millis());
        delay(100);
    }

    //to allow process time to "breathe"
    Motor_Delay(100);
}

void onWsEvent(AsyncWebServerRequest *request)
{
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT))
    {
        inputMessage = request->getParam(PARAM_INPUT)->value();
        sliderValue = inputMessage;

        startPics = !startPics;
        startPause = false;

        StepPerPhoto = FullRev / sliderValue.toInt(); //Calculate amount of steps per photo
        Serial.print("STARTPIC FLAG STATUS: ");
        Serial.println(startPics);
    }
    // GET direction value on <ESP_IP>/rotate?direction=<direction>
    else if (request->hasParam(PAUSE_INPUT))
    {
        pause_btn = request->getParam(PAUSE_INPUT)->value();
        startPics = false;
        startPause = !startPause;

        Serial.print("Pause operation: ");
        Serial.println(pause_btn);
    }
    else
    {
        inputMessage = "No message sent";
        pause_btn = "No direction sent";
    }

    Serial.println(inputMessage);
    //Uncomment for debugging reasons!
    //    Serial.print("Current steps:");
    //    Serial.println(StepPerPhoto);
    request->send(200, "text/plain", "OK");
}

void Motor_Delay(int milli)
{
    int end_time = millis() + milli;
    while (millis() < end_time)
    {
        myStepper.run();
        yield();
    }
}
