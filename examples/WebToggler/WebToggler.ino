/**
 * WebToggler - turn on and off 4 output channels via HTTP
 *
 * Outputs can also be toggled locally using debounced buttons.
 * Uses TimerOne library, so that buttons continue to work
 * even if there is a network failure:
 *
 * https://github.com/PaulStoffregen/TimerOne
 *
 * Outputs are connected to:
 *
 *     Output 1 - D5
 *     Output 2 - D6
 *     Output 3 - D7
 *     Output 4 - D8
 *
 * Buttons are connect to:
 *
 *     Button Input 1 - A0
 *     Button Input 2 - A1
 *     Button Input 3 - A2
 *     Button Input 4 - A3
 *
 * The following HTTP endpoints are supported:
 *
 *     /           - Displays HTML with buttons
 *     /outputs/1  - Get/set output 1
 *     /outputs/2  - Get/set output 2
 *     /outputs/3  - Get/set output 3
 *     /outputs/4  - Get/set output 4
 *
 * Any other paths will return '404 Not Found'.
 *
 * You can turn outputs on and off using a POST request with curl:
 *
 *     curl -d 'on' http://[2001:dead:beef::9cb3:19ff:fec7:1b10]/outputs/1
 *     curl -d 'off' http://[2001:dead:beef::9cb3:19ff:fec7:1b10]/outputs/1
 *
 * Or toggle using a POST with no body:
 *
 *     curl -X POST http://[2001:dead:beef::9cb3:19ff:fec7:1b10]/outputs/1
 *
 *
 * WARNING: this index page in this example is over 750 bytes. You must increase
 * the ETHERSIA_MAX_PACKET_SIZE in EtherSia.h to over 900 bytes for this example 
 * to work.
 *
 * Uses a hard-code MAC address. Get your own
 * Random Locally Administered MAC Address here:
 *
 * https://www.hellion.org.uk/cgi-bin/randmac.pl
 *
 * @file
 */

#include <Arduino.h>
#include <avr/wdt.h>

#include <EtherSia.h>
#include <TimerOne.h>

/** ENC28J60 Ethernet Interface */
EtherSia_ENC28J60 ether(10);

/** Define HTTP server */
HTTPServer http(ether);


/** The Arduino pin number for the first input button */
#define FIRST_INPUT_PIN   (A0)

/** The Arduino pin number for the first output */
#define FIRST_OUTPUT_PIN  (5)

/** The number of output channels to use */
#define CHANNEL_COUNT     (4)


const char PROGMEM label1[] = "Output 1";  ///< Label on HTML page for Output 1
const char PROGMEM label2[] = "Output 2";  ///< Label on HTML page for Output 2
const char PROGMEM label3[] = "Output 3";  ///< Label on HTML page for Output 3
const char PROGMEM label4[] = "Output 4";  ///< Label on HTML page for Output 4

/** Called once at the start */
void setup()
{
    MACAddress macAddress("d6:9c:e1:1c:0b:32");

    Serial.begin(115200);
    Serial.println(F("[EtherSia WebToggler]"));
    macAddress.println();

    for(byte i=0; i<CHANNEL_COUNT; i++) {
        pinMode(FIRST_OUTPUT_PIN+i, OUTPUT);
        pinMode(FIRST_INPUT_PIN+i, INPUT);
    }

    Timer1.initialize(50000);   // 50ms
    Timer1.attachInterrupt(checkButtons);

    // Enable the Watchdog timer
    wdt_enable(WDTO_8S);

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    Serial.print(F("Our address is: "));
    ether.globalAddress().println();

    Serial.println("Ready.");
}

/**
 * Turn an output On, when it is Off. Or Off when it is On.
 *
 * @param pin The Arduino output pin number
 */
void digitalToggle(byte pin)
{
    digitalWrite(pin, !digitalRead(pin));
}

/** Periodically called by TimerOne to check if a button has been pressed */
void checkButtons() {
    static byte ignore_count[CHANNEL_COUNT] = {0,0,0,0};
    static byte trigger_count[CHANNEL_COUNT] = {0,0,0,0};

    for(byte i=0; i<CHANNEL_COUNT; i++) {
        byte state = digitalRead(FIRST_INPUT_PIN + i);

        // Check if we are in an ignore period
        if (ignore_count[i] == 0) {

            if (state == HIGH) {
                // Increment the trigger_count every 50 ms while it is less than 3 and the BTN is HIGH
                if (trigger_count[i] < 3) {
                    trigger_count[i]++;

                } else {
                    // If the button is HIGH and if the 200 milliseconds of debounce has been met
                    digitalToggle(FIRST_OUTPUT_PIN + i);

                    trigger_count[i] = 0;  // Reset the button counter
                    ignore_count[i] = 8;   // Set a 400 millisecond Ignore counter
                }

            } else {
                // Reset counters if the button press was a bounce (state LOW)
                if (trigger_count[i] > 0)
                    trigger_count[i] = 0;
            }

        } else if (state == LOW) {
            // Count Down the Button Ignore variable once it has been released
            --ignore_count[i];
        }
    }
}

/** Send the HTML index page back to the browser */
void sendIndex()
{
    const char* const labels[CHANNEL_COUNT] = {label1, label2, label3, label4};

    http.printHeaders(http.typeHtml);
    http.print(F("<!DOCTYPE html>"));
    http.print(F("<html><head><title>WebToggler</title>"));
    http.print(F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"));
    http.print(F("<link href=\"http://njh.me/eswt.css\" rel=\"stylesheet\" />"));
    http.print(F("</head><body><h1>WebToggler</h1>"));
    http.print(F("<form method=\"POST\"><table>"));
    for (uint8_t i=0; i<CHANNEL_COUNT; i++) {
      char buffer[16];
      strcpy_P(buffer, labels[i]);
      http.print(F("<tr><th>"));
      http.print(buffer);
      http.print(F(": </th>"));
      if (digitalRead(FIRST_OUTPUT_PIN + i)) {
          http.print(F("<td class=\"on\">On</td>"));
      } else {
          http.print(F("<td class=\"off\">Off</td>"));
      }
      http.print(F("<td><button type=\"submit\" formaction=\"/outputs/"));
      http.print(i+1, DEC);
      http.print(F("\">Toggle</button></td></tr>"));
    }
    http.print(F("</table></form></body></html>"));
    http.sendReply();
}

// Ensure that the ethernet packet buffer is big enough (HTML=750 bytes, Headers=140 bytes)
// FIXME: currently disabled because it breaks the automated testing
//static_assert(ETHERSIA_MAX_PACKET_SIZE >= 900, "ETHERSIA_MAX_PACKET_SIZE should be 900 bytes or more");


/**
 * Get the output number from the path of the HTTP request
 *
 * @return the output number, or -1 if it isn't valid
 */
int8_t pathToNum()
{
    // /outputs/X
    // 0123456789
    int8_t num = http.path()[9] - '1';
    if (0 <= num && num < CHANNEL_COUNT) {
        return num;
    } else {
        http.notFound();
        return -1;
    }
}

/** the loop function runs over and over again forever */
void loop()
{
    // Check for an available packet
    ether.receivePacket();

    // GET the index page
    if (http.isGet(F("/"))) {
        sendIndex();

    // GET the state of a single output
    } else if (http.isGet(F("/outputs/?"))) {
        int8_t num = pathToNum();
        if (num != -1) {
            http.printHeaders(http.typePlain);
            if (digitalRead(FIRST_OUTPUT_PIN + num)) {
                http.print(F("on"));
            } else {
                http.print(F("off"));
            }
            http.sendReply();
        }

    // POST the state of a single output
    } else if (http.isPost(F("/outputs/?"))) {
        int8_t num = pathToNum();
        if (num != -1) {
            if (http.body() == NULL) {
                digitalToggle(FIRST_OUTPUT_PIN + num);
            } else if (http.bodyEquals("on")) {
                digitalWrite(FIRST_OUTPUT_PIN + num, HIGH);
            } else if (http.bodyEquals("off")) {
                digitalWrite(FIRST_OUTPUT_PIN + num, LOW);
            }
            http.redirect(F("/"));
        }

    // No matches - return 404 Not Found page
    } else if (http.havePacket()) {
        http.notFound();

    } else {
        // Some other packet, reply with rejection
        ether.rejectPacket();

    }

    // Reset the watchdog
    wdt_reset();
}
