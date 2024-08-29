class Graph(): 
    def __init__(self, vertices): 
        self.V = vertices 
        self.graph = [] 
        self.graph_matrix = [[0 for column in range(vertices)]  
                    for row in range(vertices)]

    def addEdge(self,u,v,w):
        pass

    def get_min(self,X,a) -> int:
        b=[]
        for i in range(len(X)):
            if (X[i] != 0) & (i not in a):
                b.append(X[i])
        min=b[0]
        for i in b:
            if i < min:
                min=i
        return min

    def Dijkstra(self, s):
        #s起始值
        a=[s]
        self.graph_matrix[0] = self.graph[s]
        for x in range(self.V-1):
        # for的i跑index
            i=0
            while len(a) < (x+2):
                # 當最小值=你最新graph matrix的某項時
                if i not in a:
                    if self.get_min(self.graph_matrix[len(a)-1],a) == self.graph_matrix[len(a)-1][i]:
                        a.append(i)
                i+=1
            for i in range(self.V):
                # 都不等於0 取小
                if (self.graph[a[-1]][i] != 0) & (self.graph_matrix[len(a)-2][i] != 0):
                    self.graph_matrix[len(a)-1][i] = min(self.graph_matrix[len(a)-2][a[-1]]+self.graph[a[-1]][i],self.graph_matrix[len(a)-2][i])

                elif (self.graph[a[-1]][i] != 0) & (self.graph_matrix[len(a)-2][i] == 0):
                    if i not in a:
                        self.graph_matrix[len(a)-1][i] = self.graph[a[-1]][i] + self.graph_matrix[len(a)-2][a[-1]]

                elif (self.graph[a[-1]][i] == 0) & (self.graph_matrix[len(a)-2][i] != 0):
                    self.graph_matrix[len(a)-1][i] = self.graph_matrix[len(a)-2][i]

        D = dict((str(i),self.graph_matrix[-1][i]) for i in range(self.V))    
        return D

    def Kruskal(self):
        pass

g=Graph(9)
g.graph = [[0,4,0,0,0,0,0,8,0],
           [4,0,8,0,0,0,0,11,0],
           [0,8,0,7,0,4,0,0,2],
           [0,0,7,0,9,14,0,0,0],
           [0,0,0,9,0,10,0,0,0],
           [0,0,4,14,10,0,2,0,0],
           [0,0,0,0,0,2,0,1,6],
           [8,11,0,0,0,0,1,0,7],
           [0,0,2,0,0,0,6,7,0]
          ]

# g = Graph(7)
# g.graph = [[0,7,0,0,0,0,0],
#           [7,0,10,8,0,0,0],
#           [0,10,0,12,0,1,0],
#           [0,8,12,0,8,4,0],
#           [0,0,6,8,0,10,5],
#           [0,0,1,4,10,0,6],
#           [0,0,0,0,5,6,0],
#           ];

# g = Graph(6)
# g.graph = [[0,8,0,0,0,1],
#           [3,0,1,0,0,0],
#           [5,0,0,2,2,0],
#           [0,4,6,0,7,3],
#           [0,0,0,0,0,0],
#           [0,0,0,2,8,0]]

# g = Graph(6)
# g.graph = [[0,5,0,0,0,0],
#           [0,0,6,0,-4,0],
#           [0,0,0,0,-3,-2],
#           [0,0,4,0,0,0],
#           [0,0,0,1,0,6],
#           [3,7,0,0,0,0]]

print("Dijkstra",g.Dijkstra(1))