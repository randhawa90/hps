import matplotlib.pyplot as plt
import numpy as np

def get_information(lines):
  c, i = 1, 1
  for line in lines:
    line = line.strip()
    if line.startswith('Cycles'):
      name, equ, c = line.split()
    elif line.startswith('Instructions'):
      name, equ, i = line.split()
  return int(c), int(i)

for cores, threads in [(1,1), (4, 1), (8, 1), (1, 4), (1, 8)]:
  y = []
  x = range(1, 17)
  for n in range(1, 17):
    with open('result-'+str(cores)+str(threads)+'-'+str(n), 'r') as f:
      lines = f.readlines()
      c, i = get_information(lines)
      print 'cores =', cores, 'threads =', threads
      print c, i
      ret = 1.0 * i / c
      y.append(ret)
  #plt.ylim([0.3, 1.5])
  fig = plt.figure()
  plt.xlabel('Number of Threads')
  plt.ylabel('IPC')
  fig.suptitle('cores=%d, threads=%d' % (cores, threads))
  plt.plot(np.array(x), np.array(y), 'ro-')
  plt.show()
  fig.savefig('cores=%dthreads=%d.jpg' % (cores, threads))
