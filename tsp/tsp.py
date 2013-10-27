import numpy as np
import time

def load(filename):
  cities = open('test').read().splitlines()
  cities = [ city.split() for city in cities]
  cities = [ [0, int(city[0]), int(city[1]), int(city[2]), int(city[3])] for city in cities ]
  return cities

def compute_cost(cities):
  cost = np.random.randn(num, num).astype(np.float32)

  def dis(a, b):
    a = np.array(a[2:]).astype(np.float32)
    b = np.array(b[2:]).astype(np.float32)

    return np.sqrt(np.sum((a-b) ** 2))

  for i in range(num):
    for j in range(num):
      cost[i, j] = dis(cities[i], cities[j])

  return cost


def tsp(cost, cities):
  tour = [cities[0][1]]
  total_cost = 0
  for i in range(num):
    pair = (-1, 100000000)
    for  j in range(num):
      if i != j and cities[j][0] != 1 and pair[1] > cost[i, j]:
        pair = (j, cost[i, j])
    tour.append(cities[pair[0]][1])
    total_cost += pair[1]
    cities[pair[0]][0] = 1

  return tour, total_cost


class Graph:
  def __init__(self, cost):
    self.num_v = len(cost)
    self.v = np.arange(len(cost))
    self.mark = np.zeros((len(cost), ))
    self.cost = cost.copy()
    self.p = np.ones( ( len(cost) , )) * -1

  def mst(self):
    cost  = np.ones((self.num_v, self.num_v)).astype(np.float32) * 1000000
    mst = Graph(cost)
    num = 1
    v = 0
    q = []
    marked = []
    marked.append(v)
    rst = 0
    while num < self.num_v:
      for  j in range(len(self.cost[v])):
        q.append((self.cost[v, j], v, j))
      q.sort()
      d, f, t = q[0]
      mst.p[t] = f
      mst.cost[f, t] = d
      rst += d
      num += 1
      v = t
      print num
    return mst, rst

def tsp_mst(cost, cities):
  g = Graph(cost)
  mst, t = g.mst()
  print t

if __name__ == '__main__':
  start = time.time()
  filename = 'test,small'
  cities = load(filename)
  num = len(cities)

  cost = compute_cost(cities)
  print 'finish construct cost, time', time.time() -start

  #tour, total_cost = tsp(cost, cities)
  tsp_mst(cost, cities)
  print 'time', time.time() - start
  print tour
  print total_cost
