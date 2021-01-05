import sys
sys.path.append("..")
from packages.API.CATM1 import CATM1

node = CATM1()
print(node.attachNetwork())
if node.isAttachNetwork():
    print("attached!")
else:
    print("not attached..")