#!/usr/bin/python
import subprocess
from subprocess import PIPE
import time

import sys

DEBUG = '=DEBUG=:'
PID = 'PID='
AD = 'AD='
ALLOC='ALLOC='
DEALLOC='DEALLOC='
INST='INST='
CYCLES= 'Cycles'
INSTRUCTIONS = 'Instructions'
REALTIME = 'RealTime'
DEFAULT_ORDER = [0,1,2,3]

class Thread(object):
  def __init__(self, pid, alloc, dealloc, inst):
    self.pid = pid
    self.alloc = alloc
    self.dealloc = dealloc
    self.inst = inst
    self.cycle_count = 0

  def __str__(self):
    return 'pid = %d alloc = %d dealloc = %d inst = %d' % (self.pid, self.alloc, self.dealloc, self.inst)

  def get_ipc(self):
    return self.inst * 1.0 / (self.cycle_count)


class Stat(object):
  def __init__(self, pid, ipc):
    self.pid = pid
    self.ipc = ipc


def order_toArgs(order):
  args = ' '
  for o in order:
    args += (str(o))
    args += (' ')
  return args

def get_threads(filename):
  threads = []
  with open(filename, 'r') as f:
    for line in f:
      line = line.strip()
      if line.startswith(DEBUG):
        line = line[len(DEBUG):]
        ad, pid, alloc, dealloc, inst= line.split()
        ad = int(ad[len(AD):])
        pid = int(pid[len(PID):])
        alloc = int(alloc[len(ALLOC):])
        dealloc = int(dealloc[len(DEALLOC):])
        inst = int(inst[len(INST):])
        if ad == 0:
          threads.append(Thread(pid, alloc, dealloc, inst))
        else:
          for thread in threads:
            if thread.pid == pid:
              thread.inst = inst
              #if thread.alloc > -1:
              thread.dealloc = dealloc
              thread.cycle_count += dealloc - alloc
              #thread.alloc = -1
              break
    return threads

def get_stats(filename):
  cycles = 0
  instructions = 0
  realtime = 0.0
  with open(filename, 'r') as f:
    for line in f:
      print line
      if line.startswith(CYCLES):
        line = line[len(CYCLES)+3:]
        cycles = int(line.split()[0])
        print line
        break
      if line.startswith(INSTRUCTIONS):
        line = line[len(INSTRUCTIONS)+3:]
        instructions = int(line.split()[0])
      if line.startswith(REALTIME):
        line = line[len(REALTIME)+3:]
        realtime = int(line.split()[0])
  result = []
  result.append(instructions * 1.0 / cycles)
  result.append(realtime)
  return result

#ORACLE
args = ' '.join(sys.argv[1:])
command = ('../bin/m2s --x86-config cpu-config --x86-sim detailed '+ args + order_toArgs(DEFAULT_ORDER)).split()
filename = 'result'
with open(filename, 'w') as f:
  start = time.time()
  print ' '.join(command)
  pipe = subprocess.Popen(command, stderr = PIPE, stdout = PIPE)
  pipe.wait()
  lines = pipe.stderr.read()
  print lines[-5:]
  f.writelines(lines)
  print 'time is', time.time() - start
threads = get_threads(filename)
threads.sort(key = lambda x: x.pid)
threads = threads [1:]
threads.sort(key = lambda x: -x.get_ipc())
order = []
for t in threads:
  order.append(t.pid % 4)
print 'ORACLE'
print 'pid', 'ipc'
for t in threads:
  print t.pid, t.get_ipc()
#DEFAULT
args = ' '.join(sys.argv[1:])
print order_toArgs(DEFAULT_ORDER)
command = ('../bin/m2s --x86-config cpu-config --x86-sim detailed --mem-config updated_hetero_mem_config '+ args + order_toArgs(DEFAULT_ORDER)).split()
filename = 'result'
with open(filename, 'w') as f:
  start = time.time()
  print ' '.join(command)
  pipe = subprocess.Popen(command, stderr = PIPE, stdout = PIPE)
  pipe.wait()
  lines = pipe.stderr.read()
  f.writelines(lines)
  print 'time is', time.time() - start
stats = []
stats2 = []
stats2.append(0.0)
stats2.append(0.0)
for i in range (1, 5):
  stats.append(Stat(i,0.0))
for i in range (1, 11):
  threads = get_threads(filename)
  result = get_stats(filename)
  threads.sort(key = lambda x: x.pid)
  threads = threads [1:]
  for j in range (1, 5):
    stats[i].ipc += threads[i].get_ipc()
  stats.sort(key = lambda x: x.ipc)
  stats2[0] += result[0]
  stats2[1] += result[1]
stats2[0] /= 10.0
stats2[1] /= 10.0
print 'DEFAULT'
print 'pid', 'ipc'
for s in stats:
  s.ipc /= 10.0
  print s.pid, s.ipc
print 'overallIPC', 'runtime'
print stats2[0], stats2[1]
#OPTIMAL
args = ' '.join(sys.argv[1:])
command = ('../bin/m2s --x86-config cpu-config --x86-sim detailed --mem-config updated_hetero_mem_config '+ args + order_toArgs(order)).split()
filename = 'result'
with open(filename, 'w') as f:
  start = time.time()
  print ' '.join(command)
  pipe = subprocess.Popen(command, stderr = PIPE, stdout = PIPE)
  pipe.wait()
  lines = pipe.stderr.read()
  f.writelines(lines)
  print 'time is', time.time() - start
stats = []
stats2 = []
stats2.append(0.0)
stats2.append(0.0)
for i in range (1, 5):
  stats.append(Stat(i,0.0))
for i in range (1, 11):
  threads = get_threads(filename)
  result = get_stats(filename)
  threads.sort(key = lambda x: x.pid)
  threads = threads [1:]
  for j in range (1, 5):
    stats[i].ipc += threads[i].get_ipc()
  stats.sort(key = lambda x: x.ipc)
  stats2[0] += result[0]
  stats2[1] += result[1]
stats2[0] /= 10.0
stats2[1] /= 10.0
print 'OPTIMAL'
print 'pid', 'ipc'
for s in stats:
  s.ipc /= 10.0
  print s.pid, s.ipc
print 'overallIPC', 'runtime'
print stats2[0], stats2[1]
