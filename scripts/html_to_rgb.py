import sys

def c(h):
  return str(int(h, 16) / 256.0)

html = sys.argv[1]
r = html[0] + html[1]
g = html[2] + html[3]
b = html[4] + html[5]
print "(" + c(r) + ", " + c(g) + ", " + c(b) + ")"
