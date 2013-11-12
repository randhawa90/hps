import socket
import sys
import numpy as np
import random
from sklearn import linear_model
#import matplotlib.pyplot as plt

debug = False

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

teamname = 'illuminati'
sock = MySocket(port, host)


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

def np2str(a):
    rst = ''
    for i in a:
        rst += str(i) + ' '
    rst = rst[:-1]
    return rst

if __name__ == '__main__':
    msg = sock.read()
    sock.write(teamname)
    weight = None

    msg  = sock.read()
    lines = msg.split('\n')
    M, features = lines[0].split()

    num_feature = int(features)
    if has_to_pos is None:
      has_to_pos = np.zeros(num_feature).astype(np.int32)
      has_to_neg = np.zeros(num_feature).astype(np.int32)
      could_be_pos = np.zeros(num_feature).astype(np.float32)
      could_be_neg = np.zeros(num_feature).astype(np.float32)

    num = 20
    for line in lines[1:]:
        if line.strip() == '':
            continue
        candidate = [float(x) for x in line.split()]
        score = candidate[-1]
        cans.append(Candidate(candidate[:-1], score))

    for i in range(num):
      if weight is None:
        weight = linear_initialize(cans)
        weight = sgd(cans, weight)
        has_to_pos[weight > 2.5 / num_feature] = 1
        could_be_pos[weight > 0] = 1
        could_be_pos -= has_to_pos
        has_to_neg[weight < -2.5 / num_feature] = 1
        could_be_neg[weight < 0] = 1
        could_be_neg -= has_to_neg

      rst = np.zeros(num_feature).astype(np.float32)

      index1 = 0
      index2 = 1
      if i == num - 1:
        rst[has_to_pos == 1] = 1
      else:
        #for i in range(10000):
        #  index1 = random.randint(0, num_feature-1)
        #  index2 = random.randint(0, num_feature-1)
        #  if index1 == index2:continue
        #  if (could_be_pos[index1] == 1 or could_be_neg[index1] == 1) and could_be_pos[index2] == 1:
        #    break
        tmp_weight = np.zeros_like(weight)
        tmp_weight[could_be_pos == 1] = weight[could_be_pos == 1]
        index1 = tmp_weight.argmax()
        tmp_weight = np.ones_like(weight) * -1000
        tmp_weight[could_be_neg == 1] = weight[could_be_neg == 1]
        print tmp_weight
        index2 = tmp_weight.argmax()
        rst[index1] = 1
        rst[index2] = 1

      sock.write(np2str(rst))
      msg = sock.read()
      if i == num - 1:
        break
      cans = []
      lines = msg.split('\n')
      for line in lines[1:]:
        if line.strip() == '':
            continue
        candidate = [float(x) for x in line.split()]
        score = candidate[-1]
        cans.append(Candidate(candidate[:-1], score))
      print index1, index2, 'score is', cans[-1].score
      if cans[-1].score < 0:
        has_to_neg[index1] = 1
        has_to_neg[index1] = 1
      else:
        has_to_pos[index2] = 1
        has_to_pos[index2] = 1
      could_be_pos[index1] = 0
      could_be_neg[index1] = 0
      could_be_pos[index2] = 0
      could_be_neg[index2] = 0
