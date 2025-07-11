#include "BluetoothA2DPSource.h"
#include <Arduino.h>
#include <math.h>
#include <string.h>
#include "storage/storage.h"

BluetoothA2DPSource a2dp_source;
std::vector<std::string> songs;
int currentSong = 0;


// The supported audio codec in ESP32 A2DP is SBC. SBC audio stream is encoded
// from PCM data normally formatted as 44.1kHz sampling rate, two-channel 16-bit sample data
int32_t get_data(uint8_t *data, int32_t bytes) {
    memset(data, 0, bytes);
    return bytes;
}

// Return true to connect, false will continue scanning: You can use this
// callback to build a list.
bool isValid(const char* ssid, esp_bd_addr_t address, int rssi){
   Serial.printf("Available device: %s (RSSI: %d)\n", ssid, rssi);
   return true;
}

void audio_init() {
    Serial.println("Initializing A2DP audio source");
    
    a2dp_source.set_ssid_callback(isValid);
    a2dp_source.set_auto_reconnect(false);
    a2dp_source.set_data_callback(get_data);
    a2dp_source.set_volume(30);
    a2dp_source.start();
    
    Serial.println("A2DP audio source started");
}

void audio_play() {
    if (is_paused()) {
        resume_stream();
        return;
    }
    if (songs.empty()) {
        Serial.println("No songs to play");
        return;
    }
    if (open_mp3_file(songs[currentSong])) {
        a2dp_source.set_data_callback(get_stream);
        Serial.printf("Playing: %s\n", songs[currentSong].c_str());
    }
} 
void audio_next() {
    if (!songs.empty()) {

        currentSong = (currentSong + 1) % songs.size();
        audio_play();
    }
}
void audio_prev() {
    if (!songs.empty()) {
        currentSong = (currentSong + 1) % songs.size();
        audio_play();
    }
}
void audio_pause() {
    pause_stream();
}
void audio_resume() {
    resume_stream();
}

void audio_toggle() {
    if (!is_playing()) {
        audio_play();
        return;
    }
    if (is_paused()) {
        audio_resume();
    } else {
        audio_pause();
    }
}

void audio_stop() {
    close_mp3_file();
    a2dp_source.set_data_callback(get_data); // Back to silence
    Serial.println("Audio stopped");
}