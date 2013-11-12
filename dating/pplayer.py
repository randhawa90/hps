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

'''
has_to_pos = None
has_to_neg = None
could_be_pos = None
could_be_neg = None
glr = None

class Candidate(object):
    def __init__(self, feature, score):
        self.feature = np.array(feature).astype(np.float32)
        self.score = score

cans = []

def linear_initialize(cans):
  X = []
  Y = []
  for can in cans:
    X.append(can.feature.tolist())
    Y.append(can.score)
  clf = linear_model.BayesianRidge()
  clf.fit(X, Y)
  return clf.coef_


def train(cans, learning_rate, weight, epoch = 10000, plot = False):
  num = len(cans[0].feature)
  #weight = np.random.randn(num).astype(np.float32)
  weight[weight>0] /= weight[weight>0].sum()
  weight[weight<0] /= -1.0 * weight[weight<0].sum()

  absgrads = []
  for i in range(epoch):
    if len(absgrads) > 50:
      last = np.array(absgrads[-11:])
      prev = np.array(absgrads[-21:-11])
      if last.mean() > prev.mean():
        break
    for can in cans:
      grad = np.zeros_like(weight)
      input = can.feature
      score = can.score
      yc = (input * weight).sum()
      grad += grad * (yc - score)
      absgrads.append(abs(grad))
      weight = weight - learning_rate * grad
    weight[weight>0] /= weight[weight>0].sum()
    weight[weight<0] /= -1.0 * weight[weight<0].sum()

  return weight

def test(weight, can):
  return abs(can.score - (can.feature * weight).sum())

def sgd(cans, weight):
  print 'weight before training'
  print weight
  global glr
  train_set = cans[:16]
  test_set = cans[16:]

  old_weight = weight
  if glr is None:
    best = (None, 1000000, 0.0)
    for lr in [0.00001, 0.0001, 0.001, 0.01, 0.1]:
      weight = train(cans, lr, weight, epoch = 1000)
      cost = 0
      for can in test_set:
        cost += test(weight, can)
      if best[1] > cost:
        best = [weight, cost, lr]
    glr = best[2]

  weight = train(cans, glr, old_weight,  plot = True)
  print 'weight after training'
  print weight
  return weight
'''

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
      sock.write(np2str(weight))
