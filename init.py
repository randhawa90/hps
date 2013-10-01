f = open('board.txt', 'w')
for i in range(31):
  if i != 11:
    print >> f, i -15, 0, 0
  else:
    print >> f, i -15, 3, 0

f.close()
