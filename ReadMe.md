## はじめに

このプロジェクトは、**iOS** と **Android** 上で動作する、イラストロジック解析アプリです。  

![ss_50x50](https://user-images.githubusercontent.com/13220051/89090909-d51ffd80-d3e0-11ea-9c7b-0fff8ad0e68e.gif)  

ヒントの数字と盤面の状況から埋められるマスを逐次解析していきます。  
また、埋められるマスがない場合は、仮置きをして解析を進めます。  

----
## iOS で動作確認をする場合  
**Xcode** から **/Prj_iOS/AppEnv.xcodeproj** を開いてコンパイルしてください。  

### 実装環境  
・macOS Catalina 10.15.6  
・Xcode 11.6(11E708)  

----
## Android で動作確認をする場合  
**AndroidStudio** から **/Prj_Android** を開いてコンパイルしてください。  

### 実装環境  
・Windows 10  
・Android Studio 3.1.2  
・Java 1.8.0_152  
・NDK 17.0.4754217  

----
## アプリの操作に関して  

### 解析画面  
![ss_ui_00](https://user-images.githubusercontent.com/13220051/89090891-bd487980-d3e0-11ea-9aac-444a1414823b.png)  

解析を行う基本画面です。  
各種操作の内容は下記となります。  

**＊画面左上**  
・**サンプルメニュー** アプリに組み込んであるサンプルロジックデータを選択して読み込みます  

**＊画面左下**  
・**RESET** 盤面の状況をリセットします  
・**AUTO(STOP)** 解析を開始（解析中は停止）します  
・**ASM BLK** 仮置きの際に「黒マス」を置くのか「白マス」を置くのかを切り替えます（赤色で表示されている場合は黒マスを、白で表示されている場合は白マスを仮置きします）  

**＊画面右上**  
・**NEW** 新規ロジックの作成を行います（盤面サイズ選択画面へ遷移します）  
・**LOAD** 保存済みのロジックデータを読み込みます（ロジックデータの保存はエディット画面で行います／読み込んだロジックデータに問題がある場合はエラーを表示します）  
・**EDIT** 現在のロジックデータの内容をエディットします（エディット画面へ遷移します）  

### 新規ロジックのサイズ選択画面  
![ss_ui_01](https://user-images.githubusercontent.com/13220051/89090894-bf123d00-d3e0-11ea-93ed-2a475a47c235.png)  

作成する新規ロジックのサイズを選択する画面です。  
最小サイズは１０、最大サイズは５０で、５マス区切りで指定できます。  
各種操作の内容は下記となります。  

**＊画面左上**
・**サイズメニュー** サイズを選択します（選択済み項目を再タップすることでそのサイズに決定され、エディット画面へ遷移します）

**＊画面右上**  
・**CANCEL** 新規ロジック作成をキャンセルします（解析画面へ戻ります）  

### エディット画面  
![ss_ui_02](https://user-images.githubusercontent.com/13220051/89090897-c20d2d80-d3e0-11ea-8ceb-f8ff7cd124e0.png)　　

ロジックデータの編集を行う画面です。  
入力したヒント領域をタップすることで、その領域のヒント数字を編集するボタンが表示されます。  
各種操作の内容は下記となります。  

**＊ヒント編集ボタン**  
・**▲（上矢印）／◀︎（左矢印）** ヒント数字の一つ前を選択します  
・**▼（下矢印）／▶︎（右矢印）** ヒント数字の一つ後ろを選択します  
・**ADD** ヒント数字の末尾に数字を追加し、追加した要素を選択します  
・**DEL/ZERO** 選択中のヒント数字を削除します（ヒント数字が１つの場合は０にします）  

**＊画面右下**  
・**各種数字（１〜５０）** 選択中のヒント数字の値を設定し、次のヒントに選択が移ります（一番最後のヒントを選択中の場合はそのまま）  

**＊画面右上**  
・**BACK** 解析画面へ戻ります  
・**SAVE** 名前をつけて編集内容を保存します（保存した内容は解析画面の「**LOAD**」より名前を指定して読み込めます）  
・**CHECK** ヒント数字の整合性を確認します（各列／各行のヒント数字の内容に問題がないか、タテのヒント数字の合計とヨコのヒント数字の合計が一致しているかを確認し、問題があればエラーを表示します）  

----
## 実装に関して  
ロジック解析関連の処理は **C++** にて実装しています。  

### ソースの格納場所  
ソースファイルは **/Prj_Android/app/src/main/jni/** 以下となり、**shared** フォルダ以下を **iOS** 側と共有しています。  

ロジック関連のコードは、**shared/app/logic/** に格納されています。  
テスト画面のコードは、**shared/app/loop/** に格納されています。  

細かな実装に関しては、ソースコードとコメントをご確認ください。  

----
## 仮置きに関して  

解析が行き詰まると、その状況で埋まっていないマスに黒／白マスを仮で置いて解析をすすめます。  
この際、盤面の状況に矛盾が発生すると、仮置きしたマスを反転して確定します。  
これは、マスを黒で仮定した場合にロジックが破綻するということは「そのマスは黒であってはいけない＝白であると」判断できるためです。  
また、仮置きのまま全てのマスが整合性を保って埋まった場合は、その仮置きが正しかったとして、ロジックの解析を終了します。  

解析画面では、仮置きを起点として埋まったマスは紫色で表示されます。  

![ss_45x45](https://user-images.githubusercontent.com/13220051/89090902-ca656880-d3e0-11ea-9699-eca46d80ea74.gif)  

### 複数の解放  
解析画面の「**ASM BLK**」ボタンにより、仮置きするマスの色を選択できますが、黒を選択した場合と白を選択した場合で解析の流れが変化するため、解析結果のイラストが一致しない場合があります。  

**↓ 黒マスで仮置きして解析した結果**  
![ss_assume_black](https://user-images.githubusercontent.com/13220051/89092327-b8d58e00-d3eb-11ea-9a37-0358ad385f02.png)  

**↓ 白マスで仮置きして解析した結果**  
![ss_assume_white](https://user-images.githubusercontent.com/13220051/89090900-c6394b00-d3e0-11ea-8984-1901b685eca6.png)  

正解画像のメジェド様のイラストですが、向かって右側の眉毛の形が違っています。  
これは、ロジックの内容によっては、ヒント数字に対して複数のマスの組み合わせが許容される場合があるからです（※例えば全てのヒント数字が１のロジックには、いくつもの解法が存在します）。  

ご自身で作成したロジックデータが期待と違うイラストになった場合、複数の解法が存在するデータになっていると考えられます。  

----
## 画面サイズに関する注意点  
画面サイズの小さな端末で、サイズの大きなロジックを編集するのは非常に大変です（思ったところをタップできない＆数字が潰れて読めない等）。  
申し訳ありませんが、大きなロジックの編集をする場合は、タブレット端末等、できるだけ大きな画面での操作をお願いします。  
