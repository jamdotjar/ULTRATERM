#include <Arduino.h>
#include "lvgl.h"
#include "system/system_init.h"
#include "ui/ui.h"
#include "input/input.h"
#include "audio/audio.h"
#include "storage/storage.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t ioTaskHandle = NULL;
TaskHandle_t audioTaskHandle = NULL;
QueueHandle_t audioCommandQueue = NULL;

void ioTask(void *parameter) {
    ui_init();
    input_set_callback(handle_input);

    while(true) {
        input_update();
        lv_timer_handler();
        ui_update_playback(is_playing());

        vTaskDelay(1);
  }
}

void audioTask(void *parameter) {

    audio_command_t cmd;

    while (true) {

        if (xQueueReceive(audioCommandQueue, &cmd, 0) == pdTRUE) {
            switch (cmd) {
                case CMD_PLAY:
                    Serial.println("PLAYING");
                    audio_play();
                    break;
                case CMD_PAUSE:
                    Serial.println("PAUSED");
                    audio_pause();
                    break;
                case CMD_RESUME:
                    Serial.println("RESUMING"); //might be able to cut
                    audio_resume();
                    break;
                case CMD_NEXT:
                    Serial.println("NEXT");
                    audio_next();
                    break;
                case CMD_STOP: 
                    Serial.println("STOPPING");
                    audio_stop();
                    break;
                case CMD_PLAY_PAUSE:
                    Serial.println("TOGGLING PLAY/PAUSE");
                    audio_toggle();
                    break;

            }
        }

        vTaskDelay(1);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting ULTRATERM core system");
    
    system_init();


    Serial.println("ULTRATERM core system initialized");

    Serial.println("Starting core tasks");

    audioCommandQueue = xQueueCreate(10, sizeof(audio_command_t));
    if (audioCommandQueue == NULL) {
        Serial.println("Error Creating cmd queue");
    }

    xTaskCreatePinnedToCore(ioTask, "Input/UI task", 4069, NULL, 1, &ioTaskHandle, 0);
    xTaskCreatePinnedToCore(audioTask, "Audio task", 4096, NULL, 1, &audioTaskHandle, 1);

    Serial.println("Core tasks started");
    Serial.println("ULTRATERM fully initialzed");
}


void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Prevent watchdog reset
}