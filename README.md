
##說明BY GEMINI

這是一個基於 Arduino Nano 開發的「觸感與視覺優化」節拍器。針對物理結構（Foam Optimized）進行了音效與視覺的細微調校。

## 📊 核心技術數據 (Specs)
- **控制器**: Arduino Nano (ATmega328P)
- **顯示器**: I2C LCD 1602 (Address: 0x27)
- **音效控制**: 透過手動脈衝控制 (Manual Pulse Control) 模擬「悶音」敲擊感
- **BPM 範圍**: 60 - 180 BPM (透過電位器動態映射)
- **特殊功能**: 
  - **三段式系統開關**: 透過感測器數值實現「休眠、待機、運作」邏輯。
  - **雙模式切換**: 支援 CLA (古典) 與 GUI (吉他風格) 節奏模式。
  - **自定義字型**: 16x2 螢幕上的像素點陣圖示，強化拍點視覺反饋。

## 🛠️ 硬體接線清單 (Pin Mapping)
| 元件 | 引腳 (Pin) | 功能說明 |
| :--- | :--- | :--- |
| **Buzzer** | D8 | 音效輸出 (手動控制強度) |
| **Button** | D2 | 模式切換 (使用硬體中斷 Interrupt) |
| **Potentiometer** | A0 | 速度控制與系統開關邏輯 |
| **LCD (SDA)** | A4 | I2C 數據線 |
| **LCD (SCL)** | A5 | I2C 時鐘線 |

## 💡 開發亮點：悶音演算法 (Muffled Click)
為了避免便宜蜂鳴器產生刺耳的電子音，本專案使用了 `muffledClick` 函數：
- 透過 `delayMicroseconds` 精確控制 `HIGH` 狀態的時間。
- 模擬物理撞擊的「重音」與「輕音」差異，讓節拍更有層次感。
