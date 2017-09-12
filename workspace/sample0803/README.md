(テーマ) Mergerを利用して複数台のPCからデータを収集する
===============================================================

ex11で~/MyDaq/にSampleReaderとSampleMonitorから構成される
システムを作成した。
このシステムに２つのSampleReaderとMergerを追加し、以下のようなシステムを作成する。

emulator --- SampleReader  ---\

emulator --- SampleReader2 --- Merger --- SampleMonitor

emulator --- SampleReader3 ---/


SampleReaderのコピー
--------------------------------
SampleReaderをコピーし、名前を変える

    % cd ~/MyDaq/
    % cp -r /usr/share/daqmw/examples/SampleReader/ SampleReader2
    % cp -r /usr/share/daqmw/examples/SampleReader/ SampleReader3
    % cd SampleReader2
    % cp /usr/share/daqmw/examples/change-SampleComp-name/change-SampleReader-name.sh .

change-SampleReader-name.shの中を修正

修正前）　new_name_camel_case=RawDataReader

修正後）　new_name_camel_case=SampleReader2


    % sh change-SampleReader-name.sh
    % make
    % cd ../SampleReader3
    % cp /usr/share/daqmw/examples/change-SampleComp-name/change-SampleReader-name.sh .

change-SampleReader-name.shの中を修正

修正前）　new_name_camel_case=RawDataReader

修正後）　new_name_camel_case=SampleReader3

    % sh change-SampleReader-name.sh
    % make



Mergerの取得
--------------------------------

    % cd ~/MyDaq/
    % wget http://research.kek.jp/people/ehamada/Merger.tar
    % tar xvf Merger.tar 
    % cd Merger

Merger.hを少しだけ修正する

MergerのInport数をInPortNumとして明記する。
今回はMergerのInportの数が3個であるので、

修正前）    static const int InPortNum = 2;  

修正後）    static const int InPortNum = 3;  

    % make



コンフィグレーションファイルの取得
--------------------------------
ReaderとMonitorの時のコンフィグレーションファイルにsample.xmlを利用した。
このファイルに2つのSampleReaderとMergerの設定も加えたファイルを用意したのでそれを利用する。

    % cd ~/MyDaq/
    % wget http://research.kek.jp/people/ehamada/sample3.xml


ReaderとMonitorの2つのコンポーネントの時の違いを確認して欲しい。なお、コンフィグレーションファイルの詳細はDAQ-Middleware 1.4.0開発マニュアル( http://daqmw.kek.jp/docs/DAQ-Middleware-1.1.0-Tech.pdf )の22ページに掲載されている。


SampleReaderはIPアドレス192.168.10.100から、
SampleReader2はIPアドレス192.168.10.101から、
SampleReader3はIPアドレス192.168.10.102からデータを取得している。
これらIPアドレスの設定はコンフィグレーションファイルで定義している。

実行
--------------------------------

以下のコマンドを実行

    % run.py -cl sample3.xml

あとは通常どおりに動かすことができる



（ちなみに...）2つのリードアウトモジュールでDAQしたい場合のコンフィグレーションファイルも用意している。

    % cd ~/MyDaq/
    % wget http://research.kek.jp/people/ehamada/sample2.xml

この場合、SampleReader3を利用していない。

（注）MonitorのMonitor.hのInPortNumを2とし、makeを行うこと

















