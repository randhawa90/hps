filename = 'data1'


class Node(object):
  def __init__(self, id, x, y):
    self.id = id
    self.x = x
    self.y = y
    self.flag = False
    self.visit = False

class Edge(object):
  def __init__(self, f, t):
    self.f = f
    self.t = t

nodes = []
edges = []

with open(filename, 'r') as f:
  node_pos = False
  edge_con = False
  for line in f:
    line = line.strip()
    if line.startswith('nodeid,'):
      node_pos = True
      continue
    elif line == '':
      continue
    elif line.startswith('nodeid1'):
      node_pos = False
      edge_con = True
    else:
      if node_pos:
        id, x, y = [int(x) for x in line.split(',')]
        nodes.append(Node(id, x, y))
      elif edge_con:
        x, y = [int(x) for x in line.split(',')]
        edges.append(Edge(x,y))


groups = []
node_group = []

def dfs(nodes, edges):
  global node_group
  def dfs_inner(node_id, nodes, edges):
    nodes[node_id].visit = True
    node_group.append(node_id)
    for edge in edges:
      if edge.f == node_id  and nodes[edge.t].visit == False:
        dfs_inner(edge.t, nodes, edges)

  for id, node in enumerate(nodes):
    if node.visit == False:
      node_group = []
      dfs_inner(id, nodes, edges)
      groups.append(node_group)

dfs(nodes, edges)
print len(groups)

groups.sort(key = lambda x : len(x), reverse = True)


