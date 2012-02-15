//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台  *****
//        --------------------------------------
//       	        简单音符序列播放
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2010-9      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _BUZZER_H
#define _BUZZER_H

// 定义一拍(四分音符)的时间长度
#define BUZZER_BEAT_TIME		250 	// ms
// 在音乐中，时间被分成均等的基本单位，每个单位叫做一个“拍子”或 称一拍。
// 拍子的时值是以音符的时值来表示的，一拍的时值可以是四分音符（即以四分音符为一拍），
// 也可以是二分音符（以二分音符为一拍）或八分音符（以八分音符为一拍）。
// 拍子的时值是一个相对的时间概念，比如当乐 曲的规定速度为每分钟 60 拍时，
// 每拍占用的时间是一秒，半拍是二分之一 秒；当规定速度为每分钟 120 拍时，
// 每拍的时间是半秒，半拍就是四分之一 秒，依此类推。拍子的基本时值确定之后，
// 各种时值的音符就与拍子联系在一起。例如，当以四分音符为一拍时，
// 一个全音符相当于四拍，一个二分音符相当于两拍， 八分音符相当于半拍，
// 十六分音符相当于四分之一拍；如果以八分音符做为 一拍，则全音符相当于八拍，
// 二分音符是四拍，四分音符是两拍，十六分音 符是半拍。


// 定义节拍代码
#define BEAT            0x10
#define SEMIBREVE       (BEAT+BEAT+BEAT+BEAT)
#define BEAT4           (SEMIBREVE)
#define MINIM           (BEAT+BEAT)
#define BEAT2           (BEAT+BEAT)
#define CROTCHET        (BEAT)
#define BEAT1           (BEAT)
#define QUAVER          (BEAT/2)
#define BEAT_2          (BEAT/2)
#define SEMIQUAVER      (BEAT/4)
#define BEAT_4          (BEAT/4)
#define DEMISEMIQUAVER  (BEAT/8)
#define BEAT_8          (BEAT/8)


// 定义音符代码
#define Mute    0
#define LDo     1
#define LDoH    2
#define LRe     3
#define LReH    4
#define LMe     5
#define LFa     6
#define LFaH    7
#define LSo     8
#define LSoH    9
#define LLa     10
#define LLaH    11
#define LSi     12
#define Do      13
#define DoH     14
#define Re      15
#define ReH     16
#define Me      17
#define Fa      18
#define FaH     19
#define So      20
#define SoH     21
#define La      22
#define LaH     23
#define Si      24
#define HDo     25
#define HDoH    26
#define HRe     27
#define HReH    28
#define HMe     29
#define HFa     30
#define HFaH    31
#define HSo     32
#define HSoH    33
#define HLa     34
#define HLaH    35
#define HSi     36
#define HHDo	37


#endif // _BUZZER_H
