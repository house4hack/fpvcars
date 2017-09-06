
# coding: utf-8

# In[1]:

# Generate fake serial message to simulate Arduino

import os, pty, serial
import random
import thread

def init_fake_serial():
    master, slave = pty.openpty()
    s_name = os.ttyname(slave)

    return master, s_name



def fake_messages(master):
    while True:
        if random.random()<0.05:
            button_rand = 1
        else:
            button_rand = 0
        os.write(master,'{"x":%d,"Y":%d,"Z":%d,"button":%d}\n' % (random.randint(-512,512),random.randint(-512,512),random.randint(-512,512), button_rand))
        time.sleep(1)


def start_fake_messages():
    master, fake_port = init_fake_serial()
    thread.start_new_thread(fake_messages, (master,))
    return(fake_port)




# In[2]:

def find_serial(use_fake_port=False):
    if use_fake_port:
        return(start_fake_messages())
    else:
        portlist = glob.glob("/dev/ttyACM*")
        if(len(portlist)>0):
            return(portlist[0])
        else:
            return(default_port)
    


# In[3]:

import pygame
from pygame import mixer
import json
import time
import serial
import glob
import random



# In[4]:

class Sound_effect(object):
    def __init__(self, source, channel, mixer, max_volume):
        self.source = source
        self.sound = mixer.Sound(source)
        self.channel = channel
        self.mixer = mixer
        self.max_volume = max_volume
        
        
    def play(self, controls):
        self.sound.play()
    
    def stop(self):
        self.sound.stop()
    
class Boat(Sound_effect):
    def __init__(self, source, channel, mixer,max_volume):
        super(Boat, self).__init__(source, channel, mixer,max_volume)
        mixer.Channel(self.channel).play(self.sound,-1)
        self.mixer.Channel(self.channel).set_volume(0,0)
    
    def play(self, controls):
        x = controls['X']/1023.0 
        y = controls['Y']/1023.0
        z = controls['Z']/1023.0 
        button = controls['shoot']
        self.mixer.Channel(self.channel).set_volume(max(0,-2*x+1.0*z)*self.max_volume,max(0,2*x+1.0*z)*self.max_volume)
        
    def stop(self):
        self.mixer.Channel(self.channel).set_volume(0,0)

class Torpedo(Sound_effect):
    def play(self, controls):
        if controls['shoot']==1:
            if(self.mixer.Channel(self.channel).get_busy()==0):
                self.mixer.Channel(self.channel).play(self.sound)
                

class Sonar(Sound_effect):
    def __init__(self, source, channel, mixer,max_volume):
        super(Sonar, self).__init__(source, channel, mixer,max_volume)
        self.next_play = 0
        self.mixer.Channel(self.channel).set_volume(self.max_volume, self.max_volume)

    def play(self, controls):
        if(time.time()>self.next_play):
            if(self.mixer.Channel(self.channel).get_busy()==0):
                self.mixer.Channel(self.channel).play(self.sound)
            self.next_play = random.randint(15,60) + time.time()

# In[5]:

def init_sound():
    mixer.pre_init(44100, -16, 8, 2048)
    mixer.init()
    mixer.music.load(BACKGROUND)
    


# In[8]:


def process_sound(effect_arr, background_volume, ser):
    isbackground_playing = False
    try:
        while True:
            d = None
            while ser.inWaiting():
                try:
                    d =  json.loads(ser.readline())
                except Exception as e:
                    print(e)
            if not d == None:
                if d['timeout']==0:
                    if not isbackground_playing:
                        mixer.music.play(-1)
                        mixer.music.set_volume(background_volume)
                        isbackground_playing = True
                    for effect in effect_arr:
                        effect.play(d)
                else:
                    for effect in effect_arr:
                        effect.stop()
                    mixer.music.stop()
                    isbackground_playing = False


    except KeyboardInterrupt:
        for effect in effect_arr:
            effect.stop()
        mixer.music.stop()


# In[10]:

sound_dir = "sound_effects/"
port = '/dev/ttyACM0'
BACKGROUND = sound_dir+"bubbles.mp3"
BOAT= sound_dir + "boat.wav"
SONAR= sound_dir+"sonar_ping.wav"
TORPEDO= sound_dir + "torpedo.wav"

port = find_serial(use_fake_port=False)
ser = serial.Serial(port,115200)  # open serial port
print(ser.name)

init_sound()
boat = Boat(BOAT, 0, mixer,0.5)
torpedo = Torpedo(TORPEDO, 1, mixer,1)
sonar = Sonar(SONAR,2,mixer,0.12)

process_sound([boat, torpedo, sonar], 0.7, ser)


# In[26]:




# In[27]:




# In[ ]:



