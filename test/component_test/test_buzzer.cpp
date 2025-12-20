#include <Arduino.h>
// [新增] 引入 FreeRTOS 標頭檔 (PlatformIO 的 Arduino 框架通常已內含，但明確引入是好習慣)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define BUZZER_PIN 7

// 五向按鍵腳位定義
#define BTN_UP    40
#define BTN_DWN   41
#define BTN_LFT   42
#define BTN_RHT   2
#define BTN_MID   1

// ==========================================
// 1. 資料結構定義
// ==========================================

// 定義所有音效的名稱
// 最後加一個 S_COUNT，自動計算總共有幾個音效，用來做陣列大小
enum SoundType {
  S_STARTUP,
	S_SHUTDOWN,     
  S_ALARM,
  S_MSG,       // package received
  S_ERROR,
  S_BTN_UP,    // [新增] 向上音效
  S_BTN_DWN,   // [新增] 向下音效
  S_BTN_LFT,   // [新增] 向左音效
  S_BTN_RHT,   // [新增] 向右音效
  S_BTN_MID,   // [新增] 中間確認音效
  S_COUNT      // 自動更新為 音效總數
};

struct Note { // 定義一個音符結構
	uint16_t frequency; 
	uint16_t duration;
};

// 新增：定義一個結構來包裝「樂譜」跟「長度」
struct MelodyData {
	const Note* notes;
	size_t length;
};

// ==========================================
// 2. 音效資料庫 (樂譜)
// ==========================================

const Note MELODY_ALARM[] = { // continue for about 3 seconds (3000 ms)
	{1500, 200}, {0, 50}, {1900, 200}, {0, 50},
	{1500, 200}, {0, 50}, {1900, 200}, {0, 50},
	{1500, 200}, {0, 50}, {1900, 200}, {0, 50},
	{1500, 200}, {0, 50}, {1900, 200}, {0, 50},
	{1500, 200}, {0, 50}, {1900, 200}, {0, 50},
	{1500, 200}, {0, 50}, {1900, 200}, {0, 50},
};

const Note MELODY_STARTUP[] = {
	{262, 150}, {330, 150}, {392, 300}
};
// 關機：Sol-Mi-Do (高 -> 低)，與開機對應
const Note MELODY_SHUTDOWN[] = {
  {392, 150}, {330, 150}, {262, 300}
};

const Note MELODY_MSG[] = {
	{660, 100}, {0, 20}, {880, 200}
};

const Note MELODY_ERROR[] = {
	{150, 600}
};

// [新增] button音效：設計成具備「方向感」的聲音
const Note M_BTN_UP[]  = { {880, 100}, {1320, 150} };   // 頻率上升
const Note M_BTN_DWN[] = { {1320, 100}, {880, 150} };   // 頻率下降
const Note M_BTN_LFT[] = { {1000, 80}, {0, 20}, {1000, 80} }; // 雙短音
const Note M_BTN_RHT[] = { {1100, 250} };               // 單長音
const Note M_BTN_MID[] = { {2000, 100} }; // 改為短促音，方便連按

// ==========================================
// 3. 查找表 (The Lookup Table) 
// ==========================================

// 這個陣列的順序必須嚴格對應 enum SoundType 的順序！
const MelodyData SOUND_TABLE[S_COUNT] = { // {melody pointer, length}
	// [0] S_STARTUP
	{ MELODY_STARTUP, sizeof(MELODY_STARTUP) / sizeof(Note) },
	// [1] S_SHUTDOWN
	{ MELODY_SHUTDOWN, sizeof(MELODY_SHUTDOWN) / sizeof(Note) },
	// [2] S_ALARM
	{ MELODY_ALARM,   sizeof(MELODY_ALARM)   / sizeof(Note) },
	// [3] S_MSG
	{ MELODY_MSG,     sizeof(MELODY_MSG)     / sizeof(Note) },
	// [4] S_ERROR
	{ MELODY_ERROR,   sizeof(MELODY_ERROR)   / sizeof(Note) },
	// [5] S_BTN_UP
	{ M_BTN_UP,      sizeof(M_BTN_UP)       / sizeof(Note) },
	// [6] S_BTN_DWN
	{ M_BTN_DWN,     sizeof(M_BTN_DWN)      / sizeof(Note) },
	// [7] S_BTN_LFT
	{ M_BTN_LFT,     sizeof(M_BTN_LFT)      / sizeof(Note) },
	// [8] S_BTN_RHT
	{ M_BTN_RHT,     sizeof(M_BTN_RHT)      / sizeof(Note) },
	// [9] S_BTN_MID
	{ M_BTN_MID,     sizeof(M_BTN_MID)      / sizeof(Note) },
};

// ==========================================
// 4. 播放邏輯
// ==========================================

void _playRawMelody(const Note* melody, size_t length) {
  for (size_t i = 0; i < length; i++) {
    if (melody[i].frequency > 0) { // 有音符
      tone(BUZZER_PIN, melody[i].frequency);
    } else {
      noTone(BUZZER_PIN); 
      digitalWrite(BUZZER_PIN, HIGH);
    }
    
    // [修改] 將 delay() 改為 vTaskDelay()
    // 讓出 CPU 資源，單位是毫秒轉換成的 Tick
    vTaskDelay(pdMS_TO_TICKS(melody[i].duration));
    
    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, HIGH); 
    
    // [修改] 微小的間隔也改用 vTaskDelay
    vTaskDelay(pdMS_TO_TICKS(20)); 
  }
}

// 統一的音效播放介面
void sound_effect(SoundType type) {
	// 安全檢查：防止傳入無效的 type 導致當機
	if (type < 0 || type >= S_COUNT) {
		return; 
	}

	// 直接查表取得資料
	const MelodyData* data = &SOUND_TABLE[type];
	
	// 播放
	_playRawMelody(data->notes, data->length);
}

// ==========================================
// 5. 測試主程式
// ==========================================

void test_buzzer_functionality() {
	Serial.println("Start 5D-Key + Buzzer Test (Renamed BTN Version)...");

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);

  // 初始化按鍵腳位，使用內部上拉
  const uint8_t buttons[] = {BTN_UP, BTN_DWN, BTN_LFT, BTN_RHT, BTN_MID};
  for(uint8_t pin : buttons) pinMode(pin, INPUT_PULLUP);
  // --- SOS 偵測變數 ---
  uint32_t midClickTimes[3] = {0, 0, 0}; // 儲存最近三次按下的時間戳

  // 1. 測試開始：開機音效
  Serial.println("Action: Startup");
  sound_effect(S_STARTUP);
  vTaskDelay(pdMS_TO_TICKS(500));

  Serial.println("Ready for Button Testing (30s)...");
  
  // 2. 進入 30 秒的互動掃描迴圈
  unsigned long endTime = millis() + 30000;
  while (millis() < endTime) {
    if (digitalRead(BTN_UP) == LOW)      { sound_effect(S_BTN_UP);  vTaskDelay(pdMS_TO_TICKS(150)); }
    else if (digitalRead(BTN_DWN) == LOW) { sound_effect(S_BTN_DWN); vTaskDelay(pdMS_TO_TICKS(150)); }
    else if (digitalRead(BTN_LFT) == LOW) { sound_effect(S_BTN_LFT); vTaskDelay(pdMS_TO_TICKS(150)); }
    else if (digitalRead(BTN_RHT) == LOW) { sound_effect(S_BTN_RHT); vTaskDelay(pdMS_TO_TICKS(150)); }
    else if (digitalRead(BTN_MID) == LOW) { 
      uint32_t now = millis();
      
      // 1. 更新時間戳陣列 (向左擠)
      midClickTimes[0] = midClickTimes[1];
      midClickTimes[1] = midClickTimes[2];
      midClickTimes[2] = now;

      // 2. 檢查時間窗口：第三次與第一次按下是否在 5000ms 內
      if (midClickTimes[0] != 0 && (midClickTimes[2] - midClickTimes[0] <= 5000)) {
        Serial.println("!!! SOS TRIGGERED !!!");
        sound_effect(S_ALARM);
        
        // 觸發後重置陣列，避免連按第 4 下又觸發
        midClickTimes[0] = 0; midClickTimes[1] = 0; midClickTimes[2] = 0;
      } else {
        Serial.println("Middle Button Clicked");
        sound_effect(S_BTN_MID);
      }
      
      vTaskDelay(pdMS_TO_TICKS(250)); // 防彈跳並防止一次按壓被判定為多次
    }
    
    vTaskDelay(pdMS_TO_TICKS(10)); // 釋放 CPU, 讓當前這個任務進入『睡眠狀態』 5 毫秒，並在這段時間內把 CPU 的主控權讓給其他人。
  }

  // 3. 測試結束：播放關機音效
  Serial.println("Action: Shutdown (Test Loop Finished)");
  sound_effect(S_SHUTDOWN);

  digitalWrite(BUZZER_PIN, HIGH);
  Serial.println("Buzzer Test Done.");
}