#!/usr/bin/python
import subprocess
from subprocess import PIPE
import time

DEBUG = '=Debug=:'
PID = 'pid='
ALLOC='alloc_when='
DEALLOC='dealloc_when='
INST='inst='

class Thread(object):
  def __init__(self, pid, alloc, dealloc, inst):
    self.pid = pid
    self.alloc = alloc
    self.dealloc = dealloc
    self.inst = inst

  def __str__(self):
    return 'pid = %d alloc = %d dealloc = %d inst = %d' % (self.pid, self.alloc, self.dealloc, self.inst)

def get_threads(filename):
  threads = []
  with open(filename, 'r') as f:
    for line in f:
      line = line.strip()
      if line.startswith(DEBUG):
        line = line[len(DEBUG):]
        pid, alloc, dealloc, inst= line.split()
        pid = int(pid[len(PID):])
        alloc = int(alloc[len(ALLOC):])
        dealloc = int(dealloc[len(DEALLOC):])
        inst = int(inst[len(INST):])
        print pid, alloc, dealloc, inst
        threads.append(Thread(pid, alloc, dealloc, inst))
    return threads
  

command = '../src/m2s --x86-config cpu-config --x86-sim detailed --mem-config mem-config main 3'.split()
filename = 'result'
with open(filename, 'w') as f:
  start = time.time()
  print ' '.join(command)
  pipe = subprocess.Popen(command, stderr = PIPE, stdout = PIPE)
  pipe.wait()
  lines = pipe.stderr.read()
  f.writelines(lines)
  print 'time is', time.time() - start


threads = get_threads(filename)

threads.sort(key = lambda x: x.pid)
for t in threads:
  print t
