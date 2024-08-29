class Solution(object):
    def MaxHeapify(self,i,a):                                       #函數:比較局部父節點與子節點的大小
        
        while len(a)%2==1:                                          #case1:最後的父節點只有一個子節點
            while a[-1]>a[len(a)//2]:                               #當最後一個子節點>它的父節點
                a[-1],a[len(a)//2]=a[len(a)//2],a[-1]               
            else:                                                   
                if max(a[i],a[2*i],a[2*i+1])!=a[i]:                 #如果 max(局部父節點與子節點)!=父節點
                    if a[2*i]>=a[2*i+1]:                            #如果 左子節點>右子節點
                        a[i],a[2*i]=a[2*i],a[i]
                    else:
                        a[i],a[2*i+1]=a[2*i+1],a[i]    
            break
        
        while len(a)%2==0:                                          #case2:每個父節點都有二個子節點
            if max(a[i],a[2*i],a[2*i+1])!=a[i]:
                if a[2*i]>=a[2*i+1]:
                    a[i],a[2*i]=a[2*i],a[i]
                else:
                    a[i],a[2*i+1]=a[2*i+1],a[i]
            break

    def BuildMaxHeap(self,a):                                       #函數:走訪每一個父節點
        
        for x in range(int(len(a)*(0.5))):                          #重複走訪 ∵新的父節點(剛換完)不一定大於子節點
            if len(a)>3:                                            #配合下行range範圍的條件 (至少有3筆資料)
                for i in range((len(a)//2)-1,0,-1):                 #從最後一個父節點往前走訪
                    self.MaxHeapify(i,a)                            #每走訪到一個父節點 視情況進行交換

            else:                                                   #(2筆 or 1筆資料)
                for i in range(1,0,-1):                             #只走訪第一個父節點
                    if a[-1]>a[len(a)//2]:                          #如果 a[-1]>第一筆資料 or a[-1]>自己
                        a[-1],a[len(a)//2]=a[len(a)//2],a[-1]
        return a[1:]                                                #從 index=1 回傳(原資料)

    def heap_sort(self,a):                                          #函數:用heap tree進行sort
        
        Mm=[]                                                       #用來存放sort後的測資(大至小)
        aa=[0]                                                      #把第一項(index=0)空下來
        a=aa+a                                                      #加上測資
        self.BuildMaxHeap(a)                                        #先排成max heap tree(父節點永遠大於子節點)
        
        while len(a)>1:                                             #(持續至pop完測資)(>1 ∵index=0不屬於測資)
            a[-1],a[1]=a[1],a[-1]                                   #最後一項與最大值交換
            num=a.pop()                                             #將最後一項(最大值)pop出 並暫存
            Mm.append(num)                                          #將暫存的值收集起來
            self.BuildMaxHeap(a)                                    #重新排成max heap tree
            
        #讓Mm從小至大排列
        mM=[]
        for i in range(len(Mm)):
            num=Mm.pop()
            mM.append(num)
        return mM