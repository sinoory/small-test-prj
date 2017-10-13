import virtkey
import time
def simulate():
    v = virtkey.virtkey()
    time.sleep(3)
    v.press_unicode(ord("a"))
    v.release_unicode(ord("a"))

simulate()
