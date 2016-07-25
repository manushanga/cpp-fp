import fastparser
import time
l= ["D:\Dev\ccp_1\CCP_Platform"]
ll= [[]]
f = fastparser.FileReader(l,ll)
s = fastparser.Searcher()
w = fastparser.Worker(f, s)
f.start()
w.start()


while True:
    name = raw_input("")
    start = time.time()
    vv = s.search(name)
    print("time:", time.time() - start)
    print(len(vv))
    for x in xrange(0, len(vv)):
        print(vv[x])
f.stop()
w.stop()
f.join()
w.join()