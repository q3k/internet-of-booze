import sys

secret = 'secret'
magic = 'c29j'

out = ""

buf = ""
buf += '\x15\x0b\x00\x08' * 18
for i, c in enumerate(magic + buf):
    o = (ord(c) ^ ord(secret[i%6]))
    out += chr(o)

print 'CTRL' + out.encode('hex')

print len('CTRL' + out.encode('hex'))
