## 注意事項 

1.請同學 host檔裡的xxx請改成你座位的號碼，如不知道號碼為多少，可以看螢幕後面，如果還是沒找到，可以舉手跟助教說，
例如50號就改成，192.168.34.50:8
例如123號就改成，192.168.34.123:8

![image](https://hackmd.io/_uploads/rJaMywXEp.png)

2.如果同學要使用手機驗證來登入GitHub，請舉手跟助教告知，如沒有舉手告知而拿出手機，等同於作弊



3.皆需平行化







# 多處理機平行程式設計 期末考第一題 (50%)


## 題目:古老遺跡的解謎之路-二


### 題目敘述

在一次偶然的探險中，小明發現了一座隱匿在叢林深處的神秘遺跡，據說這座遺跡承載著遠古文明的智慧和寶藏。在遺跡的深處，小明驚喜地發現了一個古老的寶箱，這個寶箱傳說藏有無比珍貴的寶物。

然而，解開這個古老寶箱的唯一線索是一張神秘的密碼卡，這張卡片上除了一串看似毫無規則的數字之外，上面還寫著一串文字寫說:<font color="#f00">使用counting sort</font>將密碼卡的內容進行升序排列即可打開寶箱

小明深知這是一個極具挑戰性的任務，為了揭開神秘寶箱的秘密，小明請求你的協助，設計一個演算法，以解開這串古老密碼，揭示寶箱內的神秘之物。

在這場數字的冒險中，你的演算法將成為解謎的關鍵，而唯有成功排序這串神秘數字，才能啟動寶箱中的機關，揭開神秘寶藏的面紗。你的演算法將成為小明解謎之旅的核心，為他帶來無窮的興奮與期待。


pseudo code:
![image](https://hackmd.io/_uploads/SJUN0Cvwa.png)


### 輸入輸出說明
輸入測資的部分第一行為$n$以及$max$，是總共有$n$個數字，所有數字中最大值為$max$，後面$n$行皆是密碼卡上的內容。

![image](https://hackmd.io/_uploads/HJmymhcD6.png)






資料範圍：
* $1 \leq n < 70001$
* $1 \leq max < 100001$
輸出的部分:如下圖所示，無須換行，印出數字的順序，每個數字需要以一個空白區隔。

![image](https://hackmd.io/_uploads/HJDzHV7Na.png)

time out：9.6s


### 繳交格式

在Github上傳一個程式碼檔案<font color="#f00">以及對應的Makefile檔案</font>，程式碼檔名為 學號_final1
例如：p12345678_final1.c p12345678_final1.cpp都可
Makefile就叫Makefile

Github classroom網址:https://classroom.github.com/a/gSeeRnGS





# 多處理機平行程式設計 期末考第二題 (50%)

## 題目:城市交通網絡最短路徑分析

### 題目敘述
你是一位交通工程師，負責分析一個大型城市的交通網絡。這個城市有多個交通節點（例如公交站、地鐵站），這些節點通過多種交通路線（如公路、地鐵線）相互連接。每條路線都有其行駛時間。請使用 Dijkstra 算法來找出起點到各點的最短路徑。
pseudo code:
![image](https://hackmd.io/_uploads/rk2QN1ODa.png)
![image](https://hackmd.io/_uploads/SkOrEJdvT.png)


### 輸入輸出說明
第一行為交通節點的數量$n$，接下來為$n*n$的矩陣，記錄各個節點到各個節點的距離，只有對角項會是$0$，節點之間為雙向邊
測資說明：
![image](https://hackmd.io/_uploads/rklV0ztcvT.png)




資料範圍：
* $1 \leq n < 2000$
* $1 \leq 節點距離 < 100$


### 繳交格式

在Github上傳一個程式碼檔案<font color="#f00">以及對應的Makefile檔案</font>，程式碼檔名為 學號_final2
例如：p12345678_final2.c p12345678_final2.cpp都可
Makefile就叫Makefile

Github classroom網址:https://classroom.github.com/a/IW1Txwmt
