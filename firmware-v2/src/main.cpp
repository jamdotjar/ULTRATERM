#include "Arduino.h"
#include "a2dp_source.h"
#include "SD.h"
#include "FS.h"
#include "SPI.h"
#include <vector>
#include <algorithm>
#include <cstring>

#define SD_CS         26
#define SPI_MOSI      18
#define SPI_MISO      19
#define SPI_SCK       22

char BT_SINK_NAME[]   = "MINIJAMBOX by Jawbone"; // sink devicename
char BT_SINK_PIN[]    = "0000";             // sink pincode
char BT_DEVICE_NAME[] = "ULTRATERM";     // source devicename


#define BTN3_PIN  14
#define BTN2_PIN  4
#define BTN1_PIN  27

// Debounce configuration and state
const unsigned long DEBOUNCE_MS = 30; // adjust if needed
static bool btn1State = HIGH, btn2State = HIGH, btn3State = HIGH, touchState = HIGH;           // debounced state
static bool btn1LastReading = HIGH, btn2LastReading = HIGH, btn3LastReading = HIGH, touchLastReading = HIGH; // last raw read
static unsigned long btn1LastChange = 0, btn2LastChange = 0, btn3LastChange = 0, touchLastChange = 0;    // last bounce time
#define TOUCH_PIN 23

File            audiofile;    // @suppress("Abstract class cannot be instantiated")
uint32_t        sampleRate;
uint32_t        bitRate;
uint8_t         channels;
uint8_t         bitsPerSample=16;
uint32_t        posDataSection;

// Playlist support
static std::vector<String> playlist; // full paths to wav files
static int currentTrack = -1;
static bool isPaused = false; // play/pause state

// Forward declarations for track navigation used in input handler
bool openNextTrack();
bool openPreviousTrack();

void input_init() {
    pinMode(BTN1_PIN, INPUT_PULLUP);
    pinMode(BTN2_PIN, INPUT_PULLUP);
    pinMode(BTN3_PIN, INPUT_PULLUP);
    pinMode(TOUCH_PIN, INPUT_PULLUP);

    btn1State = btn1LastReading = digitalRead(BTN1_PIN);
    btn2State = btn2LastReading = digitalRead(BTN2_PIN);
    btn3State = btn3LastReading = digitalRead(BTN3_PIN);
    touchState = touchLastReading = digitalRead(TOUCH_PIN);
    btn1LastChange = btn2LastChange = btn3LastChange = touchLastChange = millis();
}

void handle_input() {
    unsigned long now = millis();

    bool r1 = digitalRead(BTN1_PIN);
    if (r1 != btn1LastReading) { btn1LastReading = r1; btn1LastChange = now; }
    if ((now - btn1LastChange) > DEBOUNCE_MS) {
        if (r1 != btn1State) {
            btn1State = r1;
            if (btn1State == LOW) {
                Serial.println("Button 1 pressed -> Next track");
                openPreviousTrack();
            }
        }
    }

    bool r2 = digitalRead(BTN2_PIN);
    if (r2 != btn2LastReading) { btn2LastReading = r2; btn2LastChange = now; }
    if ((now - btn2LastChange) > DEBOUNCE_MS) {
        if (r2 != btn2State) {
            btn2State = r2;
            if (btn2State == LOW) {
                Serial.println("Button 2 pressed -> Previous track");
                openNextTrack();
            }
        }
    }

    bool r3 = digitalRead(BTN3_PIN);
    if (r3 != btn3LastReading) { btn3LastReading = r3; btn3LastChange = now; }
    if ((now - btn3LastChange) > DEBOUNCE_MS) {
        if (r3 != btn3State) {
            btn3State = r3;
            if (btn3State == LOW) {
                isPaused = !isPaused;
                Serial.println(isPaused ? "Button 3 pressed -> Paused" : "Button 3 pressed -> Playing");
            }
        }
    }

    // Optional: touch input (kept commented)
    // bool touch = (digitalRead(TOUCH_PIN));
    // if (touch != touchLastReading) { touchLastReading = touch; touchLastChange = now; }
    // if ((now - touchLastChange) > DEBOUNCE_MS) {
    //     if (touch != touchState) {
    //         touchState = touch;
    //         if (touchState == LOW) {
    //             Serial.println("Touch detected");
    //         }
    //     }
    // }
}

bool parseWAV(fs::FS &fs, String path){
    char chbuf[256];
    audiofile=fs.open(path.c_str());
    String afn = (String)audiofile.name();  //audioFileName

    if(afn.endsWith(".wav")) {
        audiofile.readBytes(chbuf, 4); // read RIFF tag
        if ((chbuf[0] != 'R') || (chbuf[1] != 'I') || (chbuf[2] != 'F') || (chbuf[3] != 'F')){
            Serial.println("file has no RIFF tag");
            audiofile.seek(0);
            return false;
        }

        audiofile.readBytes(chbuf, 4); // read chunkSize (datalen)
        uint32_t cs = (uint32_t)(chbuf[0] + (chbuf[1] <<8) + (chbuf[2] <<16) + (chbuf[3] <<24) - 8);

        audiofile.readBytes(chbuf, 4); /* read wav-format */ chbuf[5] = 0;
        if ((chbuf[0] != 'W') || (chbuf[1] != 'A') || (chbuf[2] != 'V') || (chbuf[3] != 'E')){
            Serial.println("format tag is not WAVE");
            audiofile.seek(0);
            return false;
        }

        while(true){ // skip wave chunks, seek for fmt element
            audiofile.readBytes(chbuf, 4); /* read wav-format */
            if ((chbuf[0] == 'f') && (chbuf[1] == 'm') && (chbuf[2] == 't')){
                //if(audio_info) audio_info("format tag found");
                break;
            }
        }

        audiofile.readBytes(chbuf, 4); // fmt chunksize
        cs = (uint32_t) (chbuf[0] + (chbuf[1] <<8));
        if(cs>40) return false; //something is wrong
        uint8_t bts=cs-16; // bytes to skip if fmt chunk is >16
        audiofile.readBytes(chbuf, 16);
        uint16_t fc  = (uint16_t)(chbuf[0]  + (chbuf[1] <<8));  // Format code
        uint16_t nic = (uint16_t)(chbuf[2]  + (chbuf[3] <<8));  // Number of interleaved channels
        uint32_t sr  = (uint32_t)(chbuf[4]  + (chbuf[5] <<8) + (chbuf[6]  <<16) + (chbuf[7]  <<24)); // Smpling rate
        uint32_t dr  = (uint32_t)(chbuf[8]  + (chbuf[9] <<8) + (chbuf[10] <<16) + (chbuf[11] <<24)); // Data rate
        uint16_t dbs = (uint16_t)(chbuf[12] + (chbuf[13] <<8));  // Data block size
        uint16_t bps = (uint16_t)(chbuf[14] + (chbuf[15] <<8));  // Bits per sample
        Serial.printf("FormatCode=%u\n", fc);
        Serial.printf("Channel=%u\n", nic);
        Serial.printf("SampleRate=%u\n", sr);
        Serial.printf("DataRate=%u\n", dr);
        Serial.printf("DataBlockSize=%u\n", dbs);
        Serial.printf("BitsPerSample=%u\n", bps);


        if(fc != 1){
            Serial.println("format code is not 1 (PCM)");
            return false;
        }

        if(nic != 1 && nic != 2){
            Serial.print("number of channels must be 1 or 2");
            return false;
        }

        if(bps != 8 && bps !=16){
            Serial.println("bits per sample must be 8 or 16");
            return false;
        }
        bitsPerSample=bps;
        channels = nic;
        sampleRate = sr;
        bitRate = nic * sr * bps;
        Serial.printf("BitRate=%u\n", bitRate);

        audiofile.readBytes(chbuf, bts); // skip to data
        uint32_t s = audiofile.position();
        //here can be extra info, seek for data;
        while(true){
            audiofile.seek(s);
            audiofile.readBytes(chbuf, 4); /* read header signature */
            if ((chbuf[0] == 'd') && (chbuf[1] == 'a') && (chbuf[2] == 't') && (chbuf[3] == 'a')) break;
            s++;
        }

        audiofile.readBytes(chbuf, 4); // read chunkSize (datalen)
        cs = chbuf[0] + (chbuf[1] <<8) + (chbuf[2] <<16) + (chbuf[3] <<24) - 44;
        sprintf(chbuf, "DataLength=%u\n", cs);
        Serial.print(chbuf);
        posDataSection = audiofile.position();
        // Ensure file position is at the start of data
        audiofile.seek(posDataSection);
        return true;
    }
    return false;
}

void listWavFiles(fs::FS &fs, const char* dirname){
    Serial.printf("Top-level WAV files in %s\n", dirname);
    File root = fs.open(dirname);
    if(!root || !root.isDirectory()){
        Serial.println("Failed to open directory");
        return;
    }
    File entry = root.openNextFile();
    while(entry){
        if(!entry.isDirectory()){
            String name = String(entry.name());
            if((name.endsWith(".wav") || name.endsWith(".WAV")) && !name.startsWith("._") && !name.startsWith(".")){
                String path = String(dirname);
                if(!path.endsWith("/")) path += "/";
                path += name;
                Serial.printf("- %s (%u bytes)\n", path.c_str(), (unsigned int)entry.size());
            }
        }
        entry = root.openNextFile();
    }
}

// Build playlist of WAV files (top-level) and sort alphabetically
void buildPlaylist(fs::FS &fs, const char* dirname){
    playlist.clear();
    File root = fs.open(dirname);
    if(!root || !root.isDirectory()){
        Serial.println("Failed to open directory for playlist");
        return;
    }
    File entry = root.openNextFile();
    while(entry){
        if(!entry.isDirectory()){
            String name = String(entry.name());
            if((name.endsWith(".wav") || name.endsWith(".WAV")) && !name.startsWith("._") && !name.startsWith(".")){
                String path = String(dirname);
                if(!path.endsWith("/")) path += "/";
                path += name;
                playlist.push_back(path);
            }
        }
        entry = root.openNextFile();
    }
    std::sort(playlist.begin(), playlist.end(), [](const String& a, const String& b){
        return a.compareTo(b) < 0;
    });
    Serial.printf("Playlist contains %u tracks\n", (unsigned int)playlist.size());
    for (size_t i = 0; i < playlist.size(); ++i){
        Serial.printf("  %2u: %s\n", (unsigned int)i, playlist[i].c_str());
    }
}

bool openTrackByIndex(int idx){
    if(idx < 0 || idx >= (int)playlist.size()) return false;
    if(audiofile) audiofile.close();
    const String& path = playlist[idx];
    Serial.printf("Opening track %d: %s\n", idx, path.c_str());
    bool ok = parseWAV(SD, path);
    if(ok){
        // Ensure we are at data start
        audiofile.seek(posDataSection);
    }
    return ok;
}

bool openNextTrack(){
    if(playlist.empty()) return false;
    currentTrack = (currentTrack + 1) % (int)playlist.size();
    return openTrackByIndex(currentTrack);
}

// New: open previous track (wrap-around)
bool openPreviousTrack(){
    if(playlist.empty()) return false;
    currentTrack = (currentTrack - 1 + (int)playlist.size()) % (int)playlist.size();
    return openTrackByIndex(currentTrack);
}

void setup(){
    Serial.begin(115200);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    SD.begin(SD_CS);
    listWavFiles(SD, "/");
    buildPlaylist(SD, "/");
    if(!playlist.empty()){
        currentTrack = 0;
        if(!openTrackByIndex(currentTrack)){
            Serial.println("Failed to open first track");
        }
    } else {
        Serial.println("No WAV files found on SD");
    }
    input_init();
    a2dp_source_init(BT_SINK_NAME, BT_SINK_PIN);
}


void loop(){
    bt_loop();
    handle_input();
}

int32_t bt_data(uint8_t *data, int32_t len, uint32_t* sr){
    *sr = sampleRate;
    if (len < 0 || data == NULL) {
        return 0;
    }

    // When paused, send silence to keep the stream alive
    if (isPaused) {
        memset(data, 0, (size_t)len);
        return len;
    }

    int32_t readLen = audiofile.read(data, len);
    if(readLen == 0){
        // End of current track, advance to next and try again
        if(openNextTrack()){
            readLen = audiofile.read(data, len);
        } else {
            return 0;
        }
    }
    return readLen;
}
void bt_info(const char* info){
    Serial.printf("bt_info: %s\n", info);
}