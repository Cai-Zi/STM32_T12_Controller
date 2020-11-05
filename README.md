# T20焊台控制器
工欲善其事，必先利其器。
这个开源项目使用STM32F103C8T6-BluePill（8元包邮）来制作T12焊台控制器，助你拥有一个得心应手的焊台！！！<br>
此控制器命名为T20，和J20航模遥控器做成一个系列，20也象征着开发时间。<br>
原理图部分主要参考了sfrwmaker的开源仓库https://github.com/sfrwmaker/hakko_t12_stm32<br>
程序部分使用keil5和stm32标准库编写，PID参数是白嫖的wagiminator的开源仓库https://github.com/wagiminator/ATmega-Soldering-Station<br>
# 引脚说明
- OLED_D0  -> PB13
- OLED_D1	 -> PB15
- OLED_RES -> PB15
- OLED_DC -> PB11
- OLED_CS -> PB12
- BM_CLK -> PB0
- BM_DT  -> PB3
- BM_SW  -> PB1
- BEEPER -> PB9
- NTC 	  -> PA6
- T12_ADC -> PA4
- HEAT 	  -> PA0
- SLEEP 	-> PA8
# 版本说明
- V1.0(2020.10.20)<br>
配套的T12烙铁头为S型热电偶，如果是其他类型的热电偶，需要更改程序中的分度表；<br>
已完成的功能包含设置温度、自动休眠、自动关机、蜂鸣器开关、恢复默认设置；<br>
在home页面时，短按旋转编码器设置加热开关，长按进入菜单，旋转设置加热温度；<br>
在菜单页面时，短按进入选择，再短按退出，旋转设置参数，长按退出菜单。<br>
- V1.1(2020.11.3)<br>
实际测试发现T12烙铁里面的热电偶与S型不符（更不符合其他类型），使用万用表的温度计校准了分度表，读者可自行设置校准值；<br>
使用USB-TTL串口下载器和串口调试助手(115200bps)读取ADC值，然后更改adc.c文件中的TEMP100、TEMP200、TEMP300、TEMP420即可；<br>
实际使用中温度的波动在30℃左右，焊接时设置280℃即可。<br>
- V1.2(2020.11.5)<br>
参考wagiminator的PID控制方式，将PWM更新频率由10Hz提高到390Hz，细分数由100提高到255；<br>
使用分段式PID：温差>100℃时，全速加热；温差30~100℃时，激进的PID；温差<30℃时，保守的PID；<br>
得到的温度非常稳定！！！焊接大焊点时，波动在10℃左右。<br>
读者可自行使用万用表的温度计校准了分度表，使用USB-TTL串口下载器和串口调试助手(115200bps)读取ADC值，<br>
然后更改main.h文件中的TEMP100、TEMP200、TEMP300、TEMP420即可；<br>
