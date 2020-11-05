# T20焊台控制器
工欲善其事，必先利其器。
这个开源项目使用STM32F103C8T6-BluePill（8元包邮）来制作T12焊台控制器，助你拥有一个得心应手的焊台！！！<br>
此控制器命名为T20，和J20航模遥控器做成一个系列，20也象征着开发时间。<br>
原理图部分主要参考了sfrwmaker的开源仓库https://github.com/sfrwmaker/hakko_t12_stm32<br>
程序部分使用keil5和stm32标准库编写，PID参数是白嫖的wagiminator的开源仓库https://github.com/wagiminator/ATmega-Soldering-Station<br>
# 引脚说明
<br>OLED显示屏(7脚SPI)：<br>
- OLED_D0  -> PB13
- OLED_D1	 -> PB15
- OLED_RES -> PB15
- OLED_DC -> PB11
- OLED_CS -> PB12<br>
旋转编码器：<br>
- BM_CLK -> PB0
- BM_DT  -> PB3
- BM_SW  -> PB1
蜂鸣器：<br>
- BEEPER -> PB9<br>
控制相关：<br>
- NTC 	  -> PA6
- T12_ADC -> PA4
- HEAT 	  -> PA0
- SLEEP 	-> PA8
# 版本说明
