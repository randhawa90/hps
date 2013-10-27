import time
import threading


big = 10000000

def mintit(denom, score, func = sum):
  coins = [100] * 100
  coins[0] = 0
  for d in denom:
    coins[d] = 1
  s = 0
  for i in range(1, 100):
    m = coins[i]
    choises = []
    choises.append(m)
    for d in denom:
      if i - d > 0:
        choises.append(coins[i - d] + 1)
      else:
        break;
    coins[i] = min(choises)
    s += coins[i]
  for i in range(100):
    print i,':', coins[i]
  return s



class MyThread(threading.Thread):
  def __init__(self, denom, k, tmp, ind, score):
    threading.Thread.__init__(self)
    self.denom = denom[:]
    self.denom.append(k)
    self.tmp = tmp
    self.ind = ind
    self.score = score

  def run(self):
    self.mintit()

  def mintit(self):
    self.tmp[self.ind] = mintit(self.denom, self.score)

def valid(denom, n):
  for i in range(1, len(denom)):
    if n % denom[i] == 0:
      return 0
    return 1

def search_mint():
  mininum = big
  denom = [1]
  start = time.time()
  end = 51
  for i in range(3, 11):
    s = time.time()
    denom.append(i)
    for j in range(i + 2, end - 2):
      if not valid(denom, j):
        continue
      denom.append(j)
      for m in range(j+ 2, end - 1):
        if not valid(denom, m):
          continue
        denom.append(m)
        num = end - m
        tmp = [big] * num
        threads = []
        for n in range(num):
          if (not valid(denom, n + m + 2) ) or m + n + 2 < 22:
            continue
          t = MyThread(denom, n + m + 2, tmp, n, mininum)
          threads.append(t)
          t.start()
        for t in threads:
          t.join()
        for n in range(num):
          if tmp[n] is not None and tmp[n] < mininum:
            mininum = tmp[n]
            final = [1, i, j, m, n + m + 2]
            print final, mininum
        del denom[-1]
      del denom[-1]
    del denom[-1]
    print 'finish with', i, time.time() - s

  print time.time() - start, ': the denomination is', final, mininum

if __name__ == "__main__":
  denom = [1, 5, 16, 23, 33]
  print mintit(denom, 90000)
  #search_mint()
