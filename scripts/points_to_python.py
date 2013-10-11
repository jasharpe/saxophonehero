import re

input = """0: (202, 178)
1: (232, 183)
2: (255, 193)
3: (288, 179)
4: (321, 165)
5: (331, 187)
6: (326, 222)
7: (321, 257)
8: (304, 395)
9: (299, 427)
10: (294, 459)
11: (305, 479)
12: (327, 479)
13: (349, 479)
14: (357, 480)
15: (364, 436)
16: (371, 392)
17: (379, 361)
18: (403, 342)"""

point_re = re.compile(r"\d+: \((\d+), (\d+)\)")
point_strings = []
inputs = input.split("\n")
l = len(inputs)
for i, point_string in enumerate(inputs):
  match = point_re.match(point_string)
  x = match.group(1)
  y = match.group(2)
  point_strings.append("  (" + x + ", " + y + ", 0)")
  if i != 0 and i % 3 == 0 and i != l - 1:
    point_strings.append("\n  (" + x + ", " + y + ", 0)")
print "cps = ["
print ",\n".join(point_strings)
print "]"
