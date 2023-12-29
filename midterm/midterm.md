## 注意事項 

1.請同學 host檔裡的xxx請改成你座位的號碼，如不知道號碼為多少，可以看螢幕後面，如果還是沒找到，可以舉手跟助教說，
例如50號就改成，192.168.34.50:8
例如123號就改成，192.168.34.123:8

![image](https://hackmd.io/_uploads/rJaMywXEp.png)

2.如果同學要使用手機驗證來登入GitHub，請舉手跟助教告知，如沒有舉手告知而拿出手機，等同於作弊

3.第一題跟第二題為計算題，不用平行化，直接印出答案即可

4.第三題到第五題為程式設計題，需平行化

# 多處理機平行程式設計 期中考第一題 (10%)





## 題目:Speedup


### 題目敘述

Assume we can“perfect＂parallelize 78% of a serial program. What is the maximum speedup of this program?




### 輸入輸出說明
直接印出答案，請印出到小數點第二位即可，
請使用
![image](https://hackmd.io/_uploads/SJwgBXQNp.png)







### 繳交格式

在Github上傳一個程式碼檔案，程式碼檔名為 學號_mid1 
例如：p12345678_mid1.c p12345678_mid1.cpp都可

Github classroom網址:https://classroom.github.com/a/e3WVGj_b


# 多處理機平行程式設計 期中考第二題 (10%)


## 題目:Speedup


### 題目敘述

Examine the task graph shown below. Each task is labelled with its run time. Answer the following question about the program’s run time. In all cases you may ignore any work scheduling or task spawning overheads. What is the speedup when the number of processes is 16? 


![image](https://hackmd.io/_uploads/SJwrGd4rp.png)





### 輸入輸出說明
直接印出答案，請印出到小數點第二位即可，
請使用
![image](https://hackmd.io/_uploads/SJwgBXQNp.png)




### 繳交格式

在Github上傳一個程式碼檔案，程式碼檔名為 學號_mid2 
例如：p12345678_mid2.c p12345678_mid2.cpp都可

Github classroom網址:https://classroom.github.com/a/L9afy9IC

# 多處理機平行程式設計 期中考第三題 (20%)


## 題目:古老遺跡的解謎之路


### 題目敘述

在一次偶然的探險中，小明發現了一座隱匿在叢林深處的神秘遺跡，據說這座遺跡承載著遠古文明的智慧和寶藏。在遺跡的深處，小明驚喜地發現了一個古老的寶箱，這個寶箱傳說藏有無比珍貴的寶物。

然而，解開這個古老寶箱的唯一線索是一張神秘的密碼卡，這張卡片上充滿了一串看似毫無規則的數字，根據古老的傳說，將密碼卡的內容進行升序排列即可打開寶箱。

小明深知這是一個極具挑戰性的任務，為了揭開神秘寶箱的秘密，小明請求你的協助，設計一個演算法，以解開這串古老密碼，揭示寶箱內的神秘之物。

在這場數字的冒險中，你的演算法將成為解謎的關鍵，而唯有成功排序這串神秘數字，才能啟動寶箱中的機關，揭開神秘寶藏的面紗。你的演算法將成為小明解謎之旅的核心，為他帶來無窮的興奮與期待。


### 輸入輸出說明
輸入測資的部分第一行為$n$，是總共有幾個數字，後面每一行皆是密碼卡上的內容。

![image](https://hackmd.io/_uploads/r1UxrNQE6.png)





資料範圍：
* $1 \leq n < 70001$

輸出的部分:如下圖所示，無須換行，印出數字的順序，每個數字需要以一個空白區隔。

![image](https://hackmd.io/_uploads/HJDzHV7Na.png)

time out：9.6s


### 繳交格式

在Github上傳一個程式碼檔案<font color="#f00">以及對應的Makefile檔案</font>，程式碼檔名為 學號_mid3
例如：p12345678_mid3.c p12345678_mid3.cpp都可
Makefile就叫Makefile

Github classroom網址:https://classroom.github.com/a/IvU3QRf-



# 多處理機平行程式設計 期中考第四題 (30%)


## 題目:風神無雙


### 題目敘述

財經專家龍哥認為地球不是圓的，應該是橢圓的。所以不應該用方型矩陣來掃氣壓圖，而是要用一個 $D_1 × D_2$ 的矩陣$K$ 來掃氣壓圖，其中 $D_1$ 與 $D_2$ 均為奇數。令 $A_t[0..m − 1, 0..n − 1]$代表在 $t$ 時間時的氣壓圖矩陣，其中 $A_0$ 為題目給定的初始矩陣。下一個時間點的矩陣經由以下公式產生。



$A_{t+1} [i,j]$ $=$  $($ $\frac{1}{D_1 * D_2}$ $\sum_{\Delta_i= - \frac{D_1-1}{2}}^\frac{D_1-1}{2}$ $\space$  $\sum_{\Delta_j= - \frac{D_2-1}{2}}^\frac{D_2-1}{2}$ $\space$ $K [\frac{D_1-1}{2} + \Delta_i , \frac{D_2-1}{2} + \Delta_j]  A_t[i+\Delta_i,j+ \Delta_j]$       $)$ 






其中 $0$ $\leq$ $i$ $\leq$ $m − 1$$,$ $0$ $\leq$ $j$ $\leq$ $n − 1$。由於地球是圓的，所以當 $[i$ $+$ $\Delta$$i$ $,$ $j$ $+$ $\Delta$$i$$]$ 超過$[0..m − 1, 0..n − 1]$ 時需轉換成對應的座標，比如 $[−1, −1]$ 轉換成 $[m − 1, n − 1]、[−2, −2]$ 轉換成 $[m − 2, n − 2]、[m, n]$ 轉換成 $[0, 0]$ 以此類推。請使用 MPI 來實作並輸出 $A_t$ 的結果。


### 輸入輸出說明

第一行只有一個數字 $t$，代表在 $t$ 時間的氣壓圖矩陣，第二行中的兩個數字  $n$ 和 $m$ 為氣壓圖矩陣的長寬。接下來$n$ $*$ $m$個數字是$A_0$裡的數值 $($$Row-Major$$)$，輸入完 $A_0$ 後下一行即是矩陣 $K$ 的長($D1$)和寬($D2$)以及矩陣$K$的內容。

例如：
![image](https://hackmd.io/_uploads/SkUtSVQNT.png)



資料範圍：
* $1 \leq n \leq 1000$
* $1 \leq m \leq 1000$
* $1 \leq t < 71$
* $1 \leq d_1 \leq 10$
* $1 \leq d_2 \leq 10$

輸出的 $A_t$ 內容請以$Row-Major$的方式印出，如以下格式
![image](https://hackmd.io/_uploads/ryJz8Vm4T.png)



<font color="#f00">注意:1.輸入的矩陣皆為int 
       $\space$$\space$$\space$$\space$$\space$$\space$$\space$ 2.輸出的數字後面都有空格</font>

time out：7.5s

### 繳交格式

在Github上傳一個程式碼檔案<font color="#f00">以及對應的Makefile檔案</font>，程式碼檔名為 學號_mid4
例如：p12345678_mid4.c p12345678_mid4.cpp都可
Makefile就叫Makefile

Github classroom網址:https://classroom.github.com/a/fNT_pcVX




# 多處理機平行程式設計 期中考第五題 (30%)


## 題目:進擊的哈士奇 II


### 題目敘述

在一個遙遠的國家中正被一群哈士奇所侵擾，這群哈士奇最喜歡說的一句話就是：你守家，我去偷拆。為此苦惱不已的國王決定在他的據點中建立一座圍牆。請選擇要建圍牆的據點與其建牆的順序，這座圍牆必須滿足任兩個據點的直線路徑不會經過圍牆外，要不然就會被不能拆家而狂爆化的哈士奇所襲擊。
雖然圍牆堅不可摧，但是哈士奇很快就發現圍牆間的據點是弱點。為了預防哈士奇襲擊，據點必須派重兵防守，但是國內兵力不足，無法同時防守全部據點。還好國內開發出一個新的技術，可以透過特殊通道快速移動。此快速通道只能在據點間直線連接，二個據點間的傳送不一定需要直接連接，可以透過其他據點轉送。但是建立此通道的費用非常昂貴，費用是二個據點間的歐式距離$（\sqrt{(x_1 − x_2)^2 + (y_1 − y_2)^2} )$，請求出使所有在圍牆上的據點連通的最小成本，你可以選擇部份圍牆內的據點當中繼點來節省建置的成本

hint:
1. 先找凸包，凸包部分不用平行化
2. Prim's algorithm pseudocode

![image](https://hackmd.io/_uploads/BkFjwV7Ep.png)


### 輸入輸出說明
輸入測資的部分第一行為$n$，是據點的總數，其後$n$行皆是以「據點座標$x$ 據點座標$y$」的格式。

資料範圍：
* $1 \leq n < 30$
* $-20 \leq x < 20$
* $-20 \leq y < 20$

輸出規範：
* 點與點之間的距離取至小數點後4位，其後無條件捨去
* 最終輸出也是只有小數點後四位
* 最終輸出只有一個數字，也就是成本，沒有空白、換行

舉例：
輸入檔案內：
${5}$ 
${0 \space 2}$
${2 \space 2}$
${2 \space 0}$
${0 \space 0}$
${1 \space 1}$

最終輸出為：
${5.6568}$

time out：35s




### 繳交格式

在Github上傳一個程式碼檔案<font color="#f00">以及對應的Makefile檔案</font>，程式碼檔名為 學號_mid5 
例如：p12345678_mid5.c p12345678_mid5.cpp都可
Makefile就叫Makefile

Github classroom網址:https://classroom.github.com/a/KvzNlLlp