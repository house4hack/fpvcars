
# coding: utf-8

# In[3]:

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




# In[1]:

def find_serial(use_fake_port=False):
    if use_fake_port:
        return(start_fake_messages())
    else:
        portlist = glob.glob("/dev/ttyACM*")
        if(len(portlist)>0):
            return(portlist[0])
        else:
            return(default_port)
    


# In[28]:

import pygame
from pygame import mixer
import json
import time
import serial
import glob
import random

sound_dir = "/home/schalk/tmp/"
port = '/dev/ttyACM0'
BACKGROUND = sound_dir+"Underwater Bubbles Sound - Meditation White Noise Relaxation.mp3"
BOAT= sound_dir + "boat1.wav"
SONAR= sound_dir+"sonar_ping.wav"
TORPEDO= sound_dir + "Torpedo Launching Sub-SoundBible.com-618051175.wav"


# In[33]:

class Sound_effect(object):
    def __init__(self, source, channel, mixer):
        self.source = source
        self.sound = mixer.Sound(source)
        self.channel = channel
        self.mixer = mixer
        
    def play(self, controls):
        self.sound.play()
    
    def stop(self):
        self.sound.stop()
    
class Boat(Sound_effect):
    def __init__(self, source, channel, mixer):
        super(Boat, self).__init__(source, channel, mixer)
        mixer.Channel(self.channel).play(self.sound,-1)
    
    def play(self, controls):
        x = controls['X']/1023.0 + 0.5
        y = controls['Y']/1023.0 + 0.5
        z = controls['Z']/1023.0 + 0.5
        button = controls['button']
        self.mixer.Channel(self.channel).set_volume(1-x,1-y)
        
class Torpedo(Sound_effect):
    def play(self, controls):
        if controls['button']==1:
            if(self.mixer.Channel(self.channel).get_busy()==0):
                self.mixer.Channel(self.channel).play(self.sound)
                

class Sonar(Sound_effect):
    def __init__(self, source, channel, mixer):
        super(Sonar, self).__init__(source, channel, mixer)
        self.next_play = 0

    def play(self, controls):
        if(time.time()>self.next_play):
            if(self.mixer.Channel(self.channel).get_busy()==0):
                self.mixer.Channel(self.channel).play(self.sound)
            self.next_play = random.randint(15,60) + time.time()
       


# In[30]:

def init_sound():
    mixer.pre_init(44100, -16, 8, 2048)
    mixer.init()
    mixer.music.load(BACKGROUND)


# In[31]:


def process_sound(effect_arr, playbackground, ser):
    if playbackground:
        mixer.music.play(-1)
    try:
        while True:
            d = None
            while ser.in_waiting:
                try:
                    d =  json.loads(ser.readline())
                except Exception as e:
                    print(e)
            if not d == None:
                for effect in effect_arr:
                    effect.play(d)

    except KeyboardInterrupt:
        for effect in effect_arr:
            effect.stop()
        mixer.music.stop()


# In[34]:

port = find_serial(use_fake_port=False)
ser = serial.Serial(port,115200)  # open serial port
print(ser.name)

init_sound()
boat = Boat(BOAT, 0, mixer)
torpedo = Torpedo(TORPEDO, 1, mixer)
sonar = Sonar(SONAR,2,mixer)

process_sound([boat, torpedo, sonar], False, ser)




