from hashlib import md5

print("Result of hashing \'password\':")
print(md5(b"password").hexdigest())
print("Result of hashing \'The quick brown fox jumps over the lazy dog\'")
print(md5(b"The quick brown fox jumps over the lazy dog").hexdigest())