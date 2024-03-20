from hashlib import md5

from bitarray import bitarray
import struct

def testhash(word: str):
    encoded = word.encode(encoding='ascii')
    return md5(encoded).hexdigest()

passwords = ["password", "The quick brown fox jumps over the lazy dog", ""]

for pw in passwords:
    print(f"{pw}: {testhash(pw)}")

# Code inspired from  https://github.com/timvandermeij/md5.py/blob/master/md5.py
bA = bitarray(endian="big")
bA.frombytes(passwords[0].encode("ascii"))
print(f"{passwords[0]} in bits: {bA}")
bA.append(1)
while len(bA)%512 != 448:
    bA.append(0)

print(f"{passwords[0]} after padding: {bA}")
bA2 = bitarray(bA, endian="little")
print(f"{passwords[0]} after padding and little endian: {bA2}")

length_in_bits = (len(passwords[0])*8)%pow(2, 64)
len_bA = bitarray(endian="little")
len_bA.frombytes(struct.pack("<Q", length_in_bits)) # "<Q" = < means little-endian, Q means unsigned long long or 64-bit ints

result = bA2.copy()
result.extend(len_bA)

print(f"{passwords[0]} with length in it: {result}")