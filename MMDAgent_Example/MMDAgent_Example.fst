# ----------------------------------------------------------------- #
#           MMDAgent Scenario Example                               #
#           released by MMDAgent Project Team                       #
#           http://www.mmdagent.jp/                                 #
# ----------------------------------------------------------------- #
#                                                                   #
#  Copyright (c) 2009-2010  Nagoya Institute of Technology          #
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

# モデル
# MODEL_ADD|モデルエイリアス|モデルファイル(.pmd)|相対座標|相対回転|基準モデルエイリアス|bone
# MODEL_CHANGE|モデルエイリアス|モデルファイル(.pmd)
# MODEL_DELETE|モデルエイリアス
# MODEL_EVENT_ADD|モデルエイリアス
# MODEL_EVENT_CHANGE|モデルエイリアス
# MODEL_EVENT_DELETE|モデルエイリアス
#
# モーション
# MOTION_ADD|モデルエイリアス|モーションエイリアス|モーションファイル(.vmd)|FULL or PART|ONCE or LOOP|ON or OFF|ON or OFF
# MOTION_CHANGE|モデルエイリアス|モーションエイリアス|モーションファイル(.vmd)
# MOTION_DELETE|モデルエイリアス|モーションエイリアス
# MOTION_EVENT_ADD|モデルエイリアス|モーションエイリアス
# MOTION_EVENT_CHANGE|モデルエイリアス|モーションエイリアス
# MOTION_EVENT_DELETE|モデルエイリアス|モーションエイリアス
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
# STAGE|背景画像ファイル(.bmp, .png, .tga),床画像ファイル(.bmp, .png, .tga)
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
# その他
# KEY|キー
# TIMER_START|カウントダウンエイリアス|値
# TIMER_STOP|カウントダウンエイリアス
# TIMER_EVENT_START|カウントダウンエイリアス
# TIMER_EVENT_STOP|カウントダウンエイリアス

# 0000-0011 Initialize

0   1   <eps> MODEL_ADD|メニュー|Accessory\menu\menu.pmd|0.0,-4.5,0.0|0.0,0.0,0.0|メイ|センター
1   2   <eps> MOTION_ADD|メニュー|表情|Motion\menu_rotation\menu_rotation.vmd|FULL|LOOP|OFF

# 0011-0020 Hello (key 1)

2   11  RECOG_EVENT_STOP|こんにちは SYNTH_START|メイ|メイ（普）|こんにちは。
2   11  RECOG_EVENT_STOP|こんにちわ SYNTH_START|メイ|メイ（普）|こんにちは。
2   11  KEY|1                       SYNTH_START|メイ|メイ（普）|こんにちは。
11  12  <eps>                       MOTION_ADD|メイ|挨拶|Motion\mei_greeting\mei_greeting.vmd|PART|ONCE
12  2   SYNTH_EVENT_STOP|メイ       <eps>

# 0021-0030 Self introduction (key 2)

2   21  RECOG_EVENT_STOP|自己紹介  SYNTH_START|メイ|メイ（普）|メイと言います。
2   21  RECOG_EVENT_STOP|あなた,誰 SYNTH_START|メイ|メイ（普）|メイと言います。
2   21  RECOG_EVENT_STOP|君,誰     SYNTH_START|メイ|メイ（普）|メイと言います。
2   21  KEY|2                      SYNTH_START|メイ|メイ（普）|メイと言います。
21  22  <eps>                      MOTION_ADD|メイ|挨拶|Motion\mei_self_introduction\mei_self_introduction.vmd|PART|ONCE
22  23  SYNTH_EVENT_STOP|メイ      SYNTH_START|メイ|メイ（普）|よろしくお願いします。
23  2   SYNTH_EVENT_STOP|メイ      <eps>

# 0031-0040 Thank you (key 3)

2   31  RECOG_EVENT_STOP|ありがと   SYNTH_START|メイ|メイ（普）|どういたしまして。
2   31  RECOG_EVENT_STOP|ありがとう SYNTH_START|メイ|メイ（普）|どういたしまして。
2   31  RECOG_EVENT_STOP|有難う     SYNTH_START|メイ|メイ（普）|どういたしまして。
2   31  RECOG_EVENT_STOP|有り難う   SYNTH_START|メイ|メイ（普）|どういたしまして。
2   31  KEY|3                       SYNTH_START|メイ|メイ（普）|どういたしまして。
31  32  <eps>                       MOTION_ADD|メイ|表情|Expression\mei_happiness\mei_happiness.vmd|PART|ONCE
32  33  SYNTH_EVENT_STOP|メイ       SYNTH_START|メイ|メイ（喜）|いつでも、話しかけてくださいね。
33  2   SYNTH_EVENT_STOP|メイ       <eps>

# 0041-0050 Positive comments (key 4)

2   41  RECOG_EVENT_STOP|可愛い   SYNTH_START|メイ|メイ（照）|恥ずかしいです。
2   41  RECOG_EVENT_STOP|かわいい SYNTH_START|メイ|メイ（照）|恥ずかしいです。
2   41  RECOG_EVENT_STOP|綺麗     SYNTH_START|メイ|メイ（照）|恥ずかしいです。
2   41  RECOG_EVENT_STOP|きれい   SYNTH_START|メイ|メイ（照）|恥ずかしいです。
2   41  KEY|4                     SYNTH_START|メイ|メイ（照）|恥ずかしいです。
41  42  <eps>                     MOTION_ADD|メイ|表情|Expression\mei_bashfulness\mei_bashfulness.vmd|PART|ONCE
42  2   SYNTH_EVENT_STOP|メイ     <eps>
