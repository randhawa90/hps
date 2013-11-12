import socket
import sys
import numpy as np
import random
from sklearn import linear_model
#import matplotlib.pyplot as plt

debug = True

class MySocket(object):
    MSGLEN = 8888
    def __init__(self, port, host):
        self.port = port
        self.host = host
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((self.host, self.port))

    def read(self):
        msg = ''
        while True:
            chunk = self.socket.recv(MySocket.MSGLEN)
            msg = msg + chunk
            if chunk.find('<EOM>') != -1:
                break
            if chunk == '':
                raise RuntimeError("socket connection broken")
        if debug:
            print msg
        msg = msg.replace('<EOM>', '')
        return msg

    def write(self, msg):
        msg += '<EOM>\n'
        if debug:
            print msg
        totalsent = 0
        while totalsent < len(msg):
            sent = self.socket.send(msg[totalsent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totalsent = totalsent + sent

port = int(sys.argv[1])
host = '127.0.0.1'

teamname = 'illuminati-i'
sock = MySocket(port, host)


def np2str(a):
    rst = ''
    for f in a:
        rst +=  ('%.2f' % f) + ' '
    rst = rst[:-1]
    return rst

def constraint_weight(weight):
  weight[weight > 0] /= weight[weight>0].sum()
  weight[weight < 0] /= -1.0*weight[weight<0].sum()
  for i in range(len(weight)):
    if weight[i] == 0 or (weight[i] > 0 and weight[i] < 0.01):
      weight[i] = 0.01
    if weight[i] < 0 and weight[i] > -0.01:
      weight[i] = -0.01
  weight = np.array([float('%.2f' % x) for x in weight]).astype(np.float32)
  while weight[weight>0].sum() != 1.0 or weight[weight<0].sum() != -1.0:
    weight[weight.argmax()] -= weight[weight>0].sum() - 1
    weight[weight.argmin()] -= weight[weight<0].sum() + 1
    print weight[weight>0].sum(),  weight[weight<0].sum()
  return weight


def create_weight(num_weight):
  weight = np.random.randn(num_weight).astype(np.float32)
  return constraint_weight(weight)

if __name__ == '__main__':
    msg = sock.read()
    sock.write(teamname)
    weight = None

    msg  = sock.read()
    lines = msg.split('\n')
    P, features = lines[0].split()

    num_feature = int(features)
    weight = create_weight(num_feature)

    sock.write(np2str(weight))
    for i in range(20):
      sock.read()
      idx2, idx1 = weight.argsort()[-2:]
      amount = weight[idx2] * 0.2
      if amount > 0.01:
        weight[idx1] -= amount
        weight[idx2] += amount
      if num_feature >= 80:
        idx1, idx2 = weight.argsort()[:2]
        amount = weight[idx1] * 0.2
        if amount > 0.01:
          weight[idx1] -= amount
          weight[idx2] += amount
      sock.write(np2str(weight))
