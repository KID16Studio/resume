from collections import defaultdict

class Graph:
    def __init__(self): 
        self.graph = defaultdict(list)

    def addEdge(self,u,v): 
        self.graph[u].append(v)

    def add1(self,X,y):                                     #代替append
        X+=[y]
    
    def add3(self,X,Y):                                     #代替extend
        X+=Y

    def BFS(self, s):
        for i in list(self.graph):                          #用list把字典(self.graph)裡(鍵:值)的鍵收集起來        *註1*
            if self.graph[i]==[None]:                       #(對於所有鍵) 當有值是None時                         *註2* *註3*
                del self.graph[i]                           #從dict刪除 (永久刪除 DFS那裡看不到)
        if s in self.graph:                                 #起點是否在字典(鍵:值)的鍵裡                          *註4*
            b,a=[s],[]                                      #list用途: b存result a暫存(鍵:值的)值
            self.add3(a,self.graph[s])                      #把(鍵:值的)值存入a
            self.add1(b,a[0])                               #把a的第一項抓下來 存進b
            a.pop(-(len(a)))                                #把a的第一項刪除
            while len(b)!=len(self.graph.keys()):           #當結果!=(鍵:值的)鍵個數 就一直跑                     *註5*
                for i in self.graph[b[-1]]:                 #更新起點 抓取此起點(鍵:值)的值
                    if (i not in b) & (i not in a):         #當值並未被抓取過時
                        self.add1(a,i)                      #存進a的最後
                self.add1(b,a[0])                           
                a.pop(-(len(a)))
            return b                                        #回傳BFS的result

    def DFS(self, s):                                       #與BFS相似 差別於b先存後進a的 a先刪後進的
        for i in list(self.graph):
            if self.graph[i]==[None]:
                del self.graph[i]
        if len(self.graph[s])!=0:
            b,a=[s],[]
            self.add3(a,self.graph[s])
            self.add1(b,a[-1])
            a.pop()
            while len(b)!=len(self.graph.keys()):
                for i in self.graph[b[-1]]:
                    if (i not in a) & (i not in b):
                        self.add1(a,i)
                self.add1(b,a[-1])
                a.pop()
            return b

# **註0**
# a=self.graph[s]時
# 會被視為一體 導致對a pop時 self.graph[s]的值跟著被刪除
# 猜測:可能是pop的關係 (或append)
# -> 自己設def 並改用extend的概念存入

# *註1*
# 不能用self.graph.keys()來代替
# 會回傳dict_keys()

# *註2*
# 當有值是None 意旨例如 g.addEdge(4,None)

# *註3*
# 若無視None的存在 下方while條件會出問題
# 會佔長度 導致後續的list out of range

# *註4*
# 避免測資輸入的起點不在字典(鍵:值)鍵的情況

# *註5*
# 起出condition為a=[]時跳出while
# 但因為順序上的問題 抓完a[0] 馬上刪掉a[0] 可能導致a=[]
# ! 改順序也能解決問題 !