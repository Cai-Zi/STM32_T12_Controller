#ifndef __BEEPER_H
#define __BEEPER_H	 
#include "sys.h"

#define Beeper PBout(9)	//PB9

void BEEPER_Init(void);//��ʼ��
void beeperOnce(void);//����������һ��
		 				    
#endif
