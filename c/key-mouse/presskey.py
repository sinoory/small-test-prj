import virtkey
import time

#http://www.linuxidc.com/Linux/2013-11/92198.htm
#sudo apt-get install python-virtkey

v = virtkey.virtkey()
v.press_keysym(65507)
v.press_unicode(ord('v'))
v.release_unicode(ord('v'))
v.release_keysym(65507)
time.sleep(5)
v.press_keysym(65421) #Enter
v.release_keysym(65421)


