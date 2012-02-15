//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨  *****
//        --------------------------------------
//       	        ���������в���
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2010-9      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _BUZZER_H
#define _BUZZER_H

// ����һ��(�ķ�����)��ʱ�䳤��
#define BUZZER_BEAT_TIME		250 	// ms
// �������У�ʱ�䱻�ֳɾ��ȵĻ�����λ��ÿ����λ����һ�������ӡ��� ��һ�ġ�
// ���ӵ�ʱֵ����������ʱֵ����ʾ�ģ�һ�ĵ�ʱֵ�������ķ������������ķ�����Ϊһ�ģ���
// Ҳ�����Ƕ����������Զ�������Ϊһ�ģ���˷��������԰˷�����Ϊһ�ģ���
// ���ӵ�ʱֵ��һ����Ե�ʱ�������統�� ���Ĺ涨�ٶ�Ϊÿ���� 60 ��ʱ��
// ÿ��ռ�õ�ʱ����һ�룬�����Ƕ���֮һ �룻���涨�ٶ�Ϊÿ���� 120 ��ʱ��
// ÿ�ĵ�ʱ���ǰ��룬���ľ����ķ�֮һ �룬�������ơ����ӵĻ���ʱֵȷ��֮��
// ����ʱֵ����������������ϵ��һ�����磬�����ķ�����Ϊһ��ʱ��
// һ��ȫ�����൱�����ģ�һ�����������൱�����ģ� �˷������൱�ڰ��ģ�
// ʮ���������൱���ķ�֮һ�ģ�����԰˷�������Ϊ һ�ģ���ȫ�����൱�ڰ��ģ�
// �������������ģ��ķ����������ģ�ʮ������ ���ǰ��ġ�


// ������Ĵ���
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


// ������������
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
