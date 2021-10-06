#智能垃圾桶
* HS-SR04超声波测距感应器x1
* SG90伺服马达x1
* 锂电池X2 8.4V
* 无源嗡鸣器x1
* 红色LED指示灯x1
* Nano Mega128 Arduino 控制板x1
* 按键x1；SS8550，2N3904三极管各x1

#电气原理图
![image](https://dafeng1980.coding.net/p/dyi-xiaofaming/d/DIY_smart_trashbin/git/raw/master/Doc/smartSCH.png?tab=file)  <br/>

#控制原理
* 超声波HS-SR04感应器检测到物体距离小于20厘米时，执行驱动SG90伺服马达。
* 伺服SG90马达开启后达到最大角度110度后停留3秒，然后再回到0度位置。
* 指示灯亮时，超声波HS-SR04扫描测距工作，指示灯灭时，伺服马达工作。
* 嗡鸣声在开盖，和关盖时响声。
* 无任何操作1分钟后关闭电源。（为了电池演示省电而设置）
* 按键重新开启电源。

#图片实物
* 智能垃圾桶图片
![image](https://dafeng1980.coding.net/p/dyi-xiaofaming/d/DIY_smart_trashbin/git/raw/master/Doc/smart2.png?tab=file)  <br/> <br/>
![image](https://dafeng1980.coding.net/p/dyi-xiaofaming/d/DIY_smart_trashbin/git/raw/master/Doc/smart1.png?tab=file)  <br/>
* 内部结构图片
![image](https://dafeng1980.coding.net/p/dyi-xiaofaming/d/DIY_smart_trashbin/git/raw/master/Doc/smart4.png?tab=file)  <br/> <br/>
![image](https://dafeng1980.coding.net/p/dyi-xiaofaming/d/DIY_smart_trashbin/git/raw/master/Doc/smart5.png?tab=file)  <br/>
* Nano控制板和电池
![image](https://dafeng1980.coding.net/p/dyi-xiaofaming/d/DIY_smart_trashbin/git/raw/master/Doc/smart3.png?tab=file)  <br/>