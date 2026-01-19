import mmap
import struct
import time
import json

import numpy as np

class Consumer:
    def __init__(self, name):
        self._file_path = f"/dev/shm/{name}"
        self._file = open(self._file_path, "r+b")  
        self._mmap = mmap.mmap(self._file.fileno(), 0)

        self._write_index = np.frombuffer(self._mmap, dtype='<u8', count=1, offset=0)
        self._magic = np.frombuffer(self._mmap, dtype='<u4', count=1, offset=8)
        self._version = np.frombuffer(self._mmap, dtype='<u4', count=1, offset=12)
        self._capacity = np.frombuffer(self._mmap, dtype='<u8', count=1, offset=16)
        self._element_size = np.frombuffer(self._mmap, dtype='<u8', count=1, offset=24)
        self._schema = np.frombuffer(self._mmap, dtype='U256', count=1, offset=32)
        self._read_index = np.frombuffer(self._mmap, dtype='<u8', count=1, offset=320)

    def __del__(self):
        self._mmap.close()
        self._file.close()

    def empty(self):
        return self._read_index[0] == self._write_index[0]

    def read_all(self, ):
        rIdx = self._read_index[0]
        wIdx = self._write_index[0]

        rByte = self._element_size[0] * (rIdx & (self._capacity[0] - 1)) + 128
        wByte = self._element_size[0] * (wIdx & (self._capacity[0] - 1)) + 128

        if (wByte < rByte):
            eByte = self._element_size[0] * self._capacity[0] + 128
            data1 = np.frombuffer(self._mmap, dtype='<i4', count=(eByte-rByte)//4, offset=rByte)
            data2 = np.frombuffer(self._mmap, dtype='<i4', count=(wByte-128)//4, offset=128)
            self._read_index[0] = wIdx
            return np.concatenate((data1, data2))
        else:
            self._read_index[0] = wIdx
            return np.frombuffer(self._mmap, dtype='<i4', count=(wIdx-rIdx), offset=rByte)

def main():
    client = Consumer("test_shm")

    print(np.__version__)

    while True:
        if client.empty():
            time.sleep(0.000001)
            continue

        batch = client.read_all()

        for item in batch:
            print(f"Received: {item}")

if __name__ == "__main__":
    main()
