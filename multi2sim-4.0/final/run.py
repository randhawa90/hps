#!/usr/bin/python
import subprocess
from subprocess import PIPE
import time


def parser(filename):
  dic = {}
  name = ''
  with open(filename, 'r') as f:
    for line in f:
      line = line.strip()
      if line == '':
        continue
      if line[0] == '[':
        name = line[1: line.find(']')]
        name = name.strip()
        dic[name] = {}
      else:
        key , equ, value = line.split()
        dic[name][key] = value
  return dic

def write(filename, dic):
  with open(filename, 'w') as f:
    for name in dic:
      d = dic[name]
      print >> f, '[%s]' % name
      for key in d:
        print >> f, key, '=', d[key]
      print >> f
      print >> f

def get_information(lines):
  c, i = 1, 1
  for line in lines:
    line = line.strip()
    if line.startswith('Cycles'):
      name, equ, c = line.split()
    elif line.startswith('Instructions'):
      name, equ, i = line.split()
  return int(c), int(i)

if __name__== '__main__':
  filename = 'cpu.txt'
  dic = parser(filename)
  print dic
  command = '../bin/m2s --x86-config cpu.txt --x86-sim detailed --mem-config memory.txt main'.split()
  for cores, threads in [(2,2)]:
    dic['General']['Cores'] = cores
    dic['General']['Threads'] = threads
    write(filename, dic)
    for n in range(1, 5):
      with open('result-'+ str(cores)+str(threads)+'-'+str(n), 'w') as f:
        start = time.time()
        command.append(str(n))
        print ' '.join(command)
        pipe = subprocess.Popen(command, stderr = PIPE, stdout = PIPE)
        pipe.wait()
        lines = pipe.stderr.read()
        command = command[:-1]
        cycle, instruction = get_information(lines)
        print cycle, instruction
        print 1.0 * instruction / cycle
        f.writelines(lines)
        print 'time is', time.time() - start
