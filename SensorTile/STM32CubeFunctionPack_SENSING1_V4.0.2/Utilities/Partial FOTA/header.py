def addHeader(binFileName,HeaderType=0x0,signature=0x0123456789ABCDEF0123456789ABCDEF,DATA_WEIGHTS_SIZE=0,ACTIVATIONS_SIZE=0):
    import struct
    Version = 0x0000
    MagicNumber= 0xABADBABE
    header  = struct.pack('<H',Version)
    header += struct.pack('<H',HeaderType)
    header += struct.pack('<QQ',signature&0xFFFFFFFFFFFFFFFF,(signature>>64)&0xFFFFFFFFFFFFFFFF)
    header += struct.pack('<I',DATA_WEIGHTS_SIZE)
    header += struct.pack('<I',ACTIVATIONS_SIZE)
    header += struct.pack('12x')
    header  = struct.pack('<II',MagicNumber,len(header)+struct.calcsize("II"))+header
    with open(binFileName, 'rb') as f:
       payload = f.read()
    with open('out.bin','wb') as f:
        f.write(header)
        f.write(payload)
addHeader('network.bin', HeaderType = 1,signature = 0x03bd25e15ee5dc9b8dbcb8c850dcba01, DATA_WEIGHTS_SIZE=5556, ACTIVATIONS_SIZE=1728)
