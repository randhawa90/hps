import socket
import sys
import numpy as np

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

teamname = 'illuminati'
sock = MySocket(port, host)


class Candidate(object):
    def __init__(self, feature, score):
        self.feature = np.array(feature).astype(np.float32)
        self.score = score

cans = []



def sgd(cans):
  learning_rate = 0.0001
  num = len(cans[0].feature)
  weight = np.random.randn(num).astype(np.float32)
  weight[weight>0] /= weight[weight>0].sum()
  weight[weight<0] /= -1.0 * weight[weight<0].sum()
  print weight
  for i in range(10000):
    grad = np.zeros_like(weight)
    for can in cans:
      input = can.feature
      score = can.score
      yc = (input * weight).sum()
      grad += grad * (yc - score)
    weight = weight - learning_rate * grad
    #weight[weight>0] /= weight[weight>0].sum()
    #weight[weight<0] /= -1.0 * weight[weight<0].sum()
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

    msg  = sock.read()
    lines = msg.split('\n')
    M, features = lines[0].split()

    num_feature = int(features)
    num = 20
    for line in lines[1:]:
        if line.strip() == '':
            continue
        candidate = [float(x) for x in line.split()]
        score = candidate[-1]
        cans.append(Candidate(candidate[:-1], score))
    weight = sgd(cans)
    rst = np.zeros(num_feature).astype(np.float32)
    rst[weight>0] = 1
    sock.write(np2str(rst))
    sock.read()
