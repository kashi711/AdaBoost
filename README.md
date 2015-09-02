### 概要
---
#### C++で実装した二値分類AdaBoost
ユークリッド距離を弱仮説とした簡単なAdaBoostアルゴリズム  
学習データの数をあらかじめ与える、反復回数を手動で決めているなどまだ不便なところ有

　
### 主なプログラムの構成
---

  - AdaBoost.cpp　:　AdaBoostアルゴリズム
  - learning_data.txt　:　学習データ(2次元データ　一行に2つの数字を書く)
  - test.bmp　:　テストデータ(学習データと同じ記述方法)
  - data.xlsx　:　学習データ、テストデータのプロット(今回はsin関数を閾値とするデータを用意した)
　

　
### 動かし方

学習データとテストデータを記述する  
AdaBoost.cppのdefineにあるpos(正例の数)とneg(負例の数)とT(反復回数)を設定して実行