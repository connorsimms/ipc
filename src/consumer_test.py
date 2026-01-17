import mmap
import struct
import time

class Consumer:
    def __init__(self, name):
        self._file_path = f"/dev/shm/{name}"
        self._file = open(self._file_path, "r+b")  
        self._mmap = mmap.mmap(self._file.fileno(), 0)

        self._write_index = struct.unpack_from('<Q', self._mmap, offset=0)[0]
        self._magic = struct.unpack_from('<L', self._mmap, offset=8)[0]
        self._version = struct.unpack_from('<L', self._mmap, offset=12)[0]
        self._capacity = struct.unpack_from('<Q', self._mmap, offset=16)[0]
        self._element_size = struct.unpack_from('<Q', self._mmap, offset=24)[0]
        self._read_index = struct.unpack_from('<Q', self._mmap, offset=64)[0]

    def __del__(self):
        self._mmap.close()
        self._file.close()

    def refresh(self):
        self._write_index = struct.unpack_from('<Q', self._mmap, offset=0)[0]
        self._read_index = struct.unpack_from('<Q', self._mmap, offset=64)[0]

    def empty(self):
        self.refresh()
        return self._read_index == self._write_index

    def update_rIdx(self, new_idx):
        struct.pack_into('<Q', self._mmap, 64, new_idx)

    def read_all(self, ):
        self.refresh()

        rIdx = self._read_index & (self._capacity - 1)
        wIdx = self._write_index & (self._capacity - 1)

        rByte = self._element_size * rIdx + 128
        wByte = self._element_size * wIdx + 128

        if (wIdx < rIdx):
            eByte = self._element_size * self._capacity + 128
            data1 = [x[0] for x in struct.iter_unpack('<i', self._mmap[rByte:eByte])]
            data2 = [x[0] for x in struct.iter_unpack('<i', self._mmap[128:wByte])]

            self.update_rIdx(self._write_index)
            return data1 + data2
        else:
            data = [x[0] for x in struct.iter_unpack('<i', self._mmap[rByte:wByte])]

            self.update_rIdx(self._write_index)
            return data

def main():
    client = Consumer("test_shm")

    while True:
        if client.empty():
            time.sleep(0.000001)
            continue

        batch = client.read_all()

        for item in batch:
            print(f"Received: {item}")

if __name__ == "__main__":
    main()
