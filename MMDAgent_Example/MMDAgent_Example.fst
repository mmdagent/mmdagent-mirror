# ----------------------------------------------------------------- #
#           MMDAgent "Sample Script"                                #
#           released by MMDAgent Project Team                       #
#           http://www.mmdagent.jp/                                 #
# ----------------------------------------------------------------- #
#                                                                   #
#  Copyright (c) 2009-2011  Nagoya Institute of Technology          #
#                           Department of Computer Science          #
#                                                                   #
# Some rights reserved.                                             #
#                                                                   #
# This work is licensed under the Creative Commons Attribution 3.0  #
# license.                                                          #
#                                                                   #
# You are free:                                                     #
#  * to Share - to copy, distribute and transmit the work           #
#  * to Remix - to adapt the work                                   #
# Under the following conditions:                                   #
#  * Attribution - You must attribute the work in the manner        #
#    specified by the author or licensor (but not in any way that   #
#    suggests that they endorse you or your use of the work).       #
# With the understanding that:                                      #
#  * Waiver - Any of the above conditions can be waived if you get  #
#    permission from the copyright holder.                          #
#  * Public Domain - Where the work or any of its elements is in    #
#    the public domain under applicable law, that status is in no   #
#    way affected by the license.                                   #
#  * Other Rights - In no way are any of the following rights       #
#    affected by the license:                                       #
#     - Your fair dealing or fair use rights, or other applicable   #
#       copyright exceptions and limitations;                       #
#     - The author's moral rights;                                  #
#     - Rights other persons may have either in the work itself or  #
#       in how the work is used, such as publicity or privacy       #
#       rights.                                                     #
#  * Notice - For any reuse or distribution, you must make clear to #
#    others the license terms of this work. The best way to do this #
#    is with a link to this web page.                               #
#                                                                   #
# See http://creativecommons.org/ for details.                      #
# ----------------------------------------------------------------- #

# 凡例
# ・"_EVENT_" が付かないものは、コマンド。FSTの第4フィールドで指定する。
# ・"_EVENT_" が付くものは、イベント通知。FSTの第3フィールドで指定する。
#
# モデル
# MODEL_ADD|モデルエイリアス|モデルファイル(.pmd)|相対座標|相対回転|基準モデルエイリアス|bone
# MODEL_CHANGE|モデルエイリアス|モデルファイル(.pmd)
# MODEL_DELETE|モデルエイリアス
# MODEL_EVENT_ADD|モデルエイリアス
# MODEL_EVENT_CHANGE|モデルエイリアス
# MODEL_EVENT_DELETE|モデルエイリアス
#
# ・「基準モデルエイリアス」「bone」は、指定モデルの指定ボーンに対してそのモデルを割り付け表示（相対表示）する。アクセサリ等に利用できる。
# ・「相対座標」「相対回転」は、基準モデルエイリアスおよびboneを指定した場合はそのボーンからの相対指定、
# 　指定無しの場合はワールド座標の原点に対する絶対指定となる。
# ・MODEL_ADDは「相対座標」以降を省略可能。省略した場合、相対座標(0,0,0)、相対回転無し、基準モデルエイリアス無しになる。
#
# モーション
# MOTION_ADD|モデルエイリアス|モーションエイリアス|モーションファイル(.vmd)|FULL or PART|ONCE or LOOP|ON or OFF|ON or OFF
# MOTION_CHANGE|モデルエイリアス|モーションエイリアス|モーションファイル(.vmd)
# MOTION_DELETE|モデルエイリアス|モーションエイリアス
# MOTION_EVENT_ADD|モデルエイリアス|モーションエイリアス
# MOTION_EVENT_CHANGE|モデルエイリアス|モーションエイリアス
# MOTION_EVENT_DELETE|モデルエイリアス|モーションエイリアス
#
# ・モーションは任意タイミングで重ね合わせできる。重ねられたモーションは平行処理され、後に追加されたものが優先される。
# ・「FULL」を「PART」にすると、キーフレームが指定されているボーンのみ、そのモーションで制御する。
# 　モーションで0フレーム目（初期姿勢）しか指定されていないボーンは無視され、他のモーションが透過する。
# 　これを使って、例えば、全体のモーションを保ったまま腕だけを別のモーションで制御する、といったことができる。
# ・「ONCE」は1回きりで、終端に達すると自動的に DELETE される。「LOOP」にすると無限ループする。
# ・一つ目の「ON or OFF」は、モーション開始時・終了時にスムージング（ブレンディング）するかの切替。デフォルトは ON
# ・二つ目の「ON or OFF」は、スムージング時にモデルの親座標を入れ替えるかどうかの切替。デフォルトは ON
# ・MOTION_ADDでは「FULL or PART」以降の引数を省略可能。省略した場合、"FULL|ONCE|ON|ON" となる。
#
# 移動・回転
# MOVE_START|モデルエイリアス|x座標,y座標,z座標|GLOBAL or LOCAL|移動速度
# MOVE_STOP|モデルエイリアス
# MOVE_EVENT_START|モデルエイリアス
# MOVE_EVENT_STOP|モデルエイリアス
# TURN_START|モデルエイリアス|x座標,y座標,z座標|GLOBAL or LOCAL|回転速度
# TURN_STOP|モデルエイリアス
# TURN_EVENT_START|モデルエイリアス
# TURN_EVENT_STOP|モデルエイリアス
# ROTATE_START|モデルエイリアス|x軸回転角，y軸回転角，y軸回転角|GLOBAL or LOCAL|回転速度
# ROTATE_STOP|モデルエイリアス
# ROTATE_EVENT_START|モデルエイリアス
# ROTATE_EVENT_STOP|モデルエイリアス
#
# 音楽・画像
# SOUND_START|サウンドエイリアス|音楽ファイル(.wav|.mp3)
# SOUND_STOP|サウンドエイリアス
# SOUND_EVENT_START|サウンドエイリアス
# SOUND_EVENT_STOP|サウンドエイリアス
# STAGE|ステージファイル(.xpmd)
# STAGE|床画像ファイル(.bmp, .png, .tga),背景画像ファイル(.bmp, .png, .tga)
#
# カメラ・照明
# LIGHTCOLOR|R,G,B
# LIGHTDIRECTION|x座標,y座標,z座標
#
# 音声認識
# RECOG_EVENT_START
# RECOG_EVENT_STOP|単語列
#
# 音声合成
# SYNTH_START|モデルエイリアス|ボイス|テキスト
# SYNTH_STOP|モデルエイリアス
# SYNTH_EVENT_START|モデルエイリアス
# SYNTH_EVENT_STOP|モデルエイリアス
# LIPSYNC_START|モデルエイリアス|時間情報付き音素列
# LIPSYNC_STOP|モデルエイリアス
# LIPSYNC_EVENT_START|モデルエイリアス
# LIPSYNC_EVENT_STOP|モデルエイリアス
#
# 変数
# TIMER_START|カウントダウンエイリアス|値
# TIMER_STOP|カウントダウンエイリアス
# TIMER_EVENT_START|カウントダウンエイリアス
# TIMER_EVENT_STOP|カウントダウンエイリアス
#
# プラグイン
# PLUGIN_ENABLE|プラグイン
# PLUGIN_DISABLE|プラグイン
# PLUGIN_EVENT_ENABLE|プラグイン
# PLUGIN_EVENT_DISABLE|プラグイン
#
# その他
# DRAGANDDROP|ファイル
# KEY|キー

# 0011-0020 Initialization

0   11  <eps> MODEL_ADD|メイ|Model\mei\mei.pmd|0.0,0.0,-14.0
11  12  <eps> MODEL_ADD|メニュー|Accessory\menu\menu.pmd|0.0,-4.5,0.0|0.0,0.0,0.0|メイ
12  13  <eps> MOTION_ADD|メニュー|回転|Motion\menu_rotation\menu_rotation.vmd|FULL|LOOP|OFF
13  14  <eps> STAGE|Stage\building2\floor.bmp,Stage\building2\background.bmp
14  2   <eps> MOTION_ADD|メイ|base|Motion\mei_wait\mei_wait.vmd|FULL|LOOP

# 0021-0030 Idle behavior

2   21  <eps>                  TIMER_START|idle1|10
21  22  <eps>                  TIMER_START|idle2|20
22  1   <eps>                  TIMER_START|idle3|30
1   1   RECOG_EVENT_START      MOTION_ADD|メイ|反応|Expression\mei_listen\mei_listen.vmd|PART|ONCE
1   1   TIMER_EVENT_STOP|idle1 MOTION_ADD|メイ|仕草|Motion\mei_idle\mei_idle_boredom.vmd|PART|ONCE
1   1   TIMER_EVENT_STOP|idle2 MOTION_ADD|メイ|仕草|Motion\mei_idle\mei_idle_touch_clothes.vmd|PART|ONCE
1   2   TIMER_EVENT_STOP|idle3 MOTION_ADD|メイ|仕草|Motion\mei_idle\mei_idle_think.vmd|PART|ONCE

# 0031-0040 Hello (key 1)

1   31  RECOG_EVENT_STOP|こんにちは SYNTH_START|メイ|メイ（普）|こんにちは。
1   31  RECOG_EVENT_STOP|こんにちわ SYNTH_START|メイ|メイ（普）|こんにちは。
1   31  KEY|1                       SYNTH_START|メイ|メイ（普）|こんにちは。
31  32  <eps>                       MOTION_ADD|メイ|挨拶|Motion\mei_greeting\mei_greeting.vmd|PART|ONCE
32  2   SYNTH_EVENT_STOP|メイ       <eps>

# 0041-0050 Self introduction (key 2)

1   41  RECOG_EVENT_STOP|自己紹介  SYNTH_START|メイ|メイ（普）|メイと言います。
1   41  RECOG_EVENT_STOP|あなた,誰 SYNTH_START|メイ|メイ（普）|メイと言います。
1   41  RECOG_EVENT_STOP|君,誰     SYNTH_START|メイ|メイ（普）|メイと言います。
1   41  KEY|2                      SYNTH_START|メイ|メイ（普）|メイと言います。
41  42  <eps>                      MOTION_ADD|メイ|挨拶|Motion\mei_self_introduction\mei_self_introduction.vmd|PART|ONCE
42  43  SYNTH_EVENT_STOP|メイ      SYNTH_START|メイ|メイ（普）|よろしくお願いします。
43  2   SYNTH_EVENT_STOP|メイ      <eps>

# 0051-0060 Thank you (key 3)

1   51  RECOG_EVENT_STOP|ありがと   SYNTH_START|メイ|メイ（普）|どういたしまして。
1   51  RECOG_EVENT_STOP|ありがとう SYNTH_START|メイ|メイ（普）|どういたしまして。
1   51  RECOG_EVENT_STOP|有難う     SYNTH_START|メイ|メイ（普）|どういたしまして。
1   51  RECOG_EVENT_STOP|有り難う   SYNTH_START|メイ|メイ（普）|どういたしまして。
1   51  KEY|3                       SYNTH_START|メイ|メイ（普）|どういたしまして。
51  52  <eps>                       MOTION_ADD|メイ|表情|Expression\mei_happiness\mei_happiness.vmd|PART|ONCE
52  53  SYNTH_EVENT_STOP|メイ       SYNTH_START|メイ|メイ（喜）|いつでも、話しかけてくださいね。
53  54  <eps>                       MOTION_CHANGE|メイ|base|Motion\mei_guide\mei_guide_happy.vmd
54  2   SYNTH_EVENT_STOP|メイ       MOTION_CHANGE|メイ|base|Motion\mei_wait\mei_wait.vmd

# 0061-0070 Positive comments (key 4)

1   61  RECOG_EVENT_STOP|可愛い   SYNTH_START|メイ|メイ（照）|恥ずかしいです。
1   61  RECOG_EVENT_STOP|かわいい SYNTH_START|メイ|メイ（照）|恥ずかしいです。
1   61  RECOG_EVENT_STOP|綺麗     SYNTH_START|メイ|メイ（照）|恥ずかしいです。
1   61  RECOG_EVENT_STOP|きれい   SYNTH_START|メイ|メイ（照）|恥ずかしいです。
1   61  KEY|4                     SYNTH_START|メイ|メイ（照）|恥ずかしいです。
61  62  <eps>                     MOTION_ADD|メイ|表情|Expression\mei_bashfulness\mei_bashfulness.vmd|PART|ONCE
62  2   SYNTH_EVENT_STOP|メイ     <eps>

# 0071-0090 Guide (key 5)

1   71  RECOG_EVENT_STOP|図書館 MODEL_DELETE|メニュー
1   71  KEY|5                   MODEL_DELETE|メニュー
71  72  <eps>                   MODEL_ADD|パネル|Accessory\map\map_library.pmd|0.0,2.8,2.5|0.0,0.0,0.0|メイ
72  73  <eps>                   MOTION_ADD|メイ|案内|Motion\mei_panel\mei_panel_on.vmd|PART|ONCE
73  74  <eps>                   MOTION_CHANGE|メイ|base|Motion\mei_guide\mei_guide_normal.vmd
74  75  <eps>                   SYNTH_START|メイ|メイ（普）|図書館は、正面から見ると、右前の方向にあります。
75  76  SYNTH_EVENT_STOP|メイ   MOTION_ADD|メイ|視線|Motion\mei_look\mei_look_down.vmd|PART|ONCE
76  77  <eps>                   SYNTH_START|メイ|メイ（普）|キャンパスマップでは、こちらになります。
77  78  <eps>                   MOTION_ADD|メイ|案内|Motion\mei_point\mei_point_center_buttom.vmd|PART|ONCE
78  79  SYNTH_EVENT_STOP|メイ   MOTION_CHANGE|メイ|base|Motion\mei_guide\mei_guide_happy.vmd
79  80  <eps>                   SYNTH_START|メイ|メイ（普）|お解りになりますか？
80  81  SYNTH_EVENT_STOP|メイ   MODEL_DELETE|パネル
81  82  <eps>                   MODEL_ADD|メニュー|Accessory\menu\menu.pmd|0.0,-4.5,0.0|0.0,0.0,0.0|メイ
82  83  <eps>                   MOTION_CHANGE|メイ|base|Motion\mei_wait\mei_wait.vmd
83  2   <eps>                   MOTION_ADD|メニュー|回転|Motion\menu_rotation\menu_rotation.vmd|FULL|LOOP|OFF
