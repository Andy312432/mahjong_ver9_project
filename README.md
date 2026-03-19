# Visual Studio 2022 建置指南（Mahjong）

本專案使用 Visual Studio 2022 建置，解決方案檔為 `Mahjong.sln`。

## 1. 方案內容

使用 VS2022 開啟 `Mahjong.sln` 後，會看到三個專案（Projects）：

- `majong`（主要核心專案，**需要建置**）
- `StdinClient`（與平台透過標準輸入輸出溝通的，**需要建置**）
- `SocketClient`（目前不使用，**可忽略**）

> 平台測試通常使用 `StdinClient.exe` 。

---

## 2. 建置前置設定（Boost 路徑）

`majong` 與 `StdinClient` 兩個專案都需要設定 Boost 路徑（**兩個專案都要做一樣的設定**）。
-範例路徑 `C:\dev\MahjongVer9-TT\Libs\boost_1_83_0`
> 重點：請將路徑替換成你本機實際的 Boost 目錄位置

以下步驟請對 `majong` 與 `StdinClient` 分別重複操作：

### A. 設定 Include 目錄（Header）

1. 右鍵專案 → **屬性（Properties）**
2. **C/C++** → **一般（General）**
3. **其他 Include 目錄（Additional Include Directories）**
4. 加入：
   - `Libs\boost_1_83_0`

### B. 設定 Library 目錄（Linker）

1. 右鍵專案 → **屬性（Properties）**
2. **連結器（Linker）** → **一般（General）**
3. **其他程式庫目錄（Additional Library Directories）**
4. 加入：
   - `Libs\boost_1_83_0`


---

## 3. 建置方式

完成 Boost 路徑設定後：

- VS 上方選單：**建置（Build） → 重建方案（Rebuild Solution）**

即可完成編譯。

---

## 4. 產出與測試方式

此版本為 **TT 版本**（TT = *Transposition Table*，同形表）。

編譯完成後，可使用下列執行檔接上平台測試：

- `MahjongVer9-TT\x64\Release\StdinClient.exe`

### 與平台溝通方式

 透過 **標準輸入/標準輸出（standard streams：stdin/stdout）** 與平台進行資料交換，因此也可以在terminal中直接啟動 `StdinClient.exe` 進行手動測試。

---

## 5. 程式碼說明（版本特性）

- 此版本非最新版，程式碼相對乾淨。
- 包含先前trace 過後加上的註解，較容易閱讀與追蹤流程。

### 開發建議

開發時不一定需要把所有新功能直接放進專案中：

- 可以先在獨立檔案/中實作與測試
- 後續再整理成 API 形式，與主程式整合
