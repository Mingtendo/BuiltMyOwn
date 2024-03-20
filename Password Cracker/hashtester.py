from hashlib import md5

def testhash(word: str):
    encoded = word.encode(encoding='ascii')
    return md5(encoded).hexdigest()

passwords = ["password", "The quick brown fox jumps over the lazy dog", ""]

for pw in passwords:
    print(pw, testhash(pw))