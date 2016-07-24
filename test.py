import fastparser
import time
l= ["/home/madura/ogre-master/OgreMain/include/"]
ll= [[]]
f = fastparser.FileReader(l,ll)
s = fastparser.Searcher()
w = fastparser.Worker(f, s)
time.sleep(2)
vv = s.search("AbstractNode")
print(len(vv))
for v in vv:
    print(v)
f.start()
f.join()