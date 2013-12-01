
import socket, sys
import time
import numpy as np
from exceptions import ZeroDivisionError
from subprocess import PIPE, Popen, STDOUT

teamname="RandomBrie"
port=4567
eom = "<EOM>"
maxlen = 999999
dim=1000
print(sys.argv)
if len(sys.argv)>1:
    port = int(sys.argv[1])
  
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('127.0.0.1', port))

def readsocket(sock,timeout=0):
    inpData=''
    while True:
        chunk = sock.recv(maxlen)
        if not chunk: break
        if chunk == '':
            raise RuntimeError("socket connection broken")
        inpData = inpData + chunk
        if eom in inpData:
            break
    inpData=inpData.strip()[:-len(eom)]
    serversaid(inpData.replace('\n', ' [N] ')[:90])
    return inpData.strip()

def sendsocket(sock,msg):
    msg += eom
    totalsent=0
    MSGLEN = len(msg) 
    while totalsent < MSGLEN:
        sent = sock.send(msg[totalsent:])
        if sent == 0:
            raise RuntimeError("socket connection broken")
        totalsent = totalsent + sent
    isaid(msg)

def makemove(socket,pid,x,y):
    sendsocket(socket,"(%d,%d,%d)"%(pid,x,y))
    
def serversaid(msg):
    print("Server: %s"%msg[:80])
def isaid(msg):
    print("Client: %s"%msg[:80])
class State:
    def __init__(self,noplayers,Nstones,playerid):
        # personal
        self.playerid=playerid
        # game parameters
        self.Nstones=Nstones
        self.noplayers=noplayers
        # changing game 
        self.nextplayer=0
        self.timeleft=-1.00
        self.moves=[]
        self.areas=[]
        self.myareas=[]
        
    def parsestate(self,statestr):
        state=statestr.split('\n')
        line1=state[0].split(',')
        self.nextplayer=int(line1[0])
        if self.nextplayer==self.playerid:
            self.timeleft=float(line1[1])
        self.parsemoves(state[1])
        self.parseareas(state[2])
        #self.calcareas()
        #print "Areas check -- "+ ' | '.join(["%d:(%d/%d)"%(i,self.areas[i],self.myareas[i]) for i in range(1,self.noplayers+1)])
    
    def parsemoves(self,movestr):
        self.moves=[[] for i in range(0,self.noplayers+1)]
        if len(movestr)==0: return # no moves yet
        movelist=movestr.split('),(')
        movelist[0]=movelist[0][1:]
        movelist[-1]=movelist[-1][:-1]
        for m in movelist:
            m=m.split(',')
            mid=int(m[0])
            x =int(m[1])
            y =int(m[2])
            self.moves[mid].append((x,y))
    
    def parseareas(self,areastr):
        self.areas=[0] # dummy 0-index
        alist=areastr.split('),(')
        alist[0]=alist[0][1:]
        alist[-1]=alist[-1][:-1]
        for astr in alist:
            aid,area=astr.split(',')
            print "aid %s, area %s"%(aid,area)
            self.areas.append(int(area))
            assert(len(self.areas)==int(aid)+1)
    
    def calcareas(self):
        # This is a dumb and slow method
        print "Calculate areas -- DISABLE FOR SPEED"
        inf=1e10
        pulls=np.zeros((self.noplayers,dim,dim)) # note: indexshift
        for i,mlist in enumerate(self.moves):
            if i==0: continue
            for m in mlist:
                for j in xrange(dim):
                    for k in xrange(dim):
                        if j!=m[0] and k!= m[1]:
                            pulls[i-1,j,k]+=1./((j-m[0])**2+(k-m[1])**2)
                        else:
                            pulls[i-1,j,k]+=inf
        maxpulls=np.argmax(pulls,axis=0)
        self.myareas=[0]
        self.myareas.extend(list(np.bincount(maxpulls.flatten())))
        if len(self.myareas)<self.noplayers+1: 
            self.myareas.extend([0 for i in range(len(self.myareas)-self.noplayers+1)]) # pad zeros
        

if __name__=="__main__":
    print "Get question from socket"
    try:
        # Protocol 2
        question=readsocket(s,1)
        assert (question=='Team Name?')
        sendsocket(s, "ddod")
        # Protocol 3, read and parse parameters
        params=readsocket(s)
        params=params.split(',')
        n_pl=int(params[0])
        N=int(params[1])
        assert(int(params[2])==dim)
        pid=int(params[3])
        state=State(n_pl, N, pid)
        # Think of strategy
        dx=int(dim/N)
        mymoves=np.arange(int(dx/2),dim,dx)
        # Game phase 4
        for turn in xrange(N):
            for pl in range(1,n_pl+1):
                statestr=readsocket(s)
                state.parsestate(statestr)
                assert(pl==state.nextplayer)
                if pl == pid:
                    #print "Thinking really hard"
                    #time.sleep(1.11)
                    command = './illuminati %d' % pid
                    pipe = Popen(command.split(), stdin = PIPE, stdout = PIPE)
                    statestr = '\n'.join(statestr.split('\n')[1:])
                    print statestr
                    str = pipe.communicate(input = statestr)[0]
                    pipe.wait()
                    print str
                    x, y = [int(a) for a in str.split()]
                    #makemove(s,pid,mymoves[turn],mymoves[turn])
                    makemove(s, pid, x, y)
        state.parsestate(readsocket(s))
        if np.argmax(state.areas)==state.playerid:
            print "I won! with %.2f percent area"%(100*float(state.areas[state.playerid])/dim**2)
        else:
            print "I lost from player %d! with %.2f percent area"%(np.argmax(state.areas),100*float(state.areas[state.playerid])/dim**2)
        
    finally:
        print "Close socket"
        s.close()
    
