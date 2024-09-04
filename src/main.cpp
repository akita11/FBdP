#include <Arduino.h>
#include <M5Unified.h>

#define SER_BUF_LEN 64
char serBuf[SER_BUF_LEN];
uint8_t p_serBuf = 0;

#define pinBz 9 // for CoreS3SE's PortB
/*
#define SAMPLE_RATE 1000
#define PCM_BUF_LEN 16
int16_t pcmBuf[PCM_BUF_LEN];
uint8_t p_pcmBuf = 0;
*/

void setup() {
	M5.begin();
	Serial2.begin(115200, SERIAL_8N1, 1, 2); // PortA RX/TX
	// ref: ATOM-S3 on DiffPres.: Serial.begin(115200, SERIAL_8N1, 2, 1); // RX/TX (Grove)
/*
	// play PCM via I2S
	// https://qiita.com/suzukiplan/items/ba86610d523a94775665

	auto config = M5.Speaker.config();
	config.sample_rate = SAMPLE_RATE;
	M5.Speaker.config(config);
	M5.Speaker.setVolume(255);
	M5.Speaker.begin();
*/
	ledcSetup(0, 15000, 8); // Ch.0, 5kHz, 8bit
  ledcAttachPin(pinBz, 0);
}

void loop() {
	int prs_i;
	float prs;
	if (Serial2.available()){
		char c = Serial2.read();
		if (c == '\n' || c == '\r'){
			serBuf[p_serBuf] = '\0';
			if (p_serBuf > 2){
				prs_i = atoi(serBuf) - 65535;
				prs = (float)prs_i / 240.0; // [Pa]
				uint8_t val;
				val = (uint8_t)(sqrt((float)(prs_i * prs_i)) * 20);
				if (val > 255) val = 255;
//				printf("%d\n", val);
/*
				pcmBuf[p_pcmBuf++] = prs + 32768;
				if (p_pcmBuf == PCM_BUF_LEN){
					while (M5.Speaker.isPlaying()) { vTaskDelay(1); }
					M5.Speaker.playRaw(pcmBuf, PCM_BUF_LEN, SAMPLE_RATE, false);
					p_pcmBuf = 0;
				}
*/
				ledcWrite(0, val);
			}
			p_serBuf = 0;
		}
		else serBuf[p_serBuf++] = c;
		if (p_serBuf == SER_BUF_LEN) p_serBuf = 0;	
	}
}
