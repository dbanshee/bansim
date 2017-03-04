# To change this license header, choose License Headers in Project Properties.
# To change this template file, choose Tools | Templates
# and open the template in the editor.

#!/usr/bin/env python 

import Tkinter as tk
import ttk as ttk
import tkFont
#import threading
import time
import sys
#import requests
import pycurl
import json
from StringIO import StringIO
from PIL import Image, ImageTk
import banpcarsscreens

__author__ = "onamaya"
__date__ = "$30-oct-2015 17:11:18$"

class Win(tk.Tk):

    def __init__(self,master=None):
        tk.Tk.__init__(self,master)
        self.overrideredirect(True)
        self._offsetx = 0
        self._offsety = 0
        self.bind('<Button-1>',self.clickwin)
        self.bind('<B1-Motion>',self.dragwin)
        self.bind('<Button-2>',self.quit)
        self.bind('<Button-3>',self.quit)
        self.wm_attributes('-topmost', 1)

    def dragwin(self,event):
        x = self.winfo_pointerx() - self._offsetx
        y = self.winfo_pointery() - self._offsety
        self.geometry('+{x}+{y}'.format(x=x,y=y))

    def clickwin(self,event):
        self._offsetx = event.x
        self._offsety = event.y
        
    def quit(self, event):
       self.destroy()


class BanScreen(tk.Frame):
    def __init__(self, master=None, dataSource=None):
        tk.Frame.__init__(self, master)
        
        self.dataSource = dataSource
        self.widgets = []

        self.configure(background='#000')
        self.pack(fill=tk.BOTH, expand=1)

        self.createScreen()

    # Load screns from files
    def createScreen(self):
        None
        
    def addItem(self, item):
        self.widgets.append(item)
        
    def refresh(self):
        # Refresh Data Components
        for i in self.widgets:
            i.refresh()
        
        # RefreskTK Gui
        self.update()

class BanWidget(tk.Frame):
    
    id = 0
    
    @staticmethod
    def getId():
        BanWidget.id = BanWidget.id + 1
        return BanWidget.id
    
    def __init__(self, master=None, posX=0, posY=0, height=10, width=10):
        tk.Frame.__init__(self, master)
        
        self.id     = BanWidget.getId()
        self.posX   = posX
        self.posY   = posY
        self.height = height
        self.width  = width
        
        if(DEBUG):
            self.configure(borderwidth=5, relief=tk.GROOVE)
            
    def place(self):
        tk.Frame.place(self, x=self.posX, y=self.posY, height=self.height, width=self.width)
        
    def refresh(self):
        None
        

class GenericLabel(BanWidget):
    
    def __init__(self, master=None, posX=0, posY=0, height=10, width=10, subLabelName=None, dataSource=None, dataFieldName=None, dataFieldIndex=0, defaultValue=None, labelFormat=None, labelFontName=None, labelFontSize=20, subLabelFontSize=5, labelColor='#fff', subLabelColor='#fff', subLabelFontName=None, subLabelSide=tk.BOTTOM, transFunc=None, colorTransFunc=None):
        
        BanWidget.__init__(self, master, posX, posY, height, width)
        
        # Data Source
        self.dataSource     = dataSource
        self.dataFieldName  = dataFieldName
        self.dataFieldIndex = dataFieldIndex
        self.defaultValue   = defaultValue
        
        # Tk
        self.master             = master
        self.labelVar           = tk.StringVar()
        self.subLabelName       = subLabelName
        self.labelFontSize      = labelFontSize;
        self.subLabelFontSize   = subLabelFontSize;
        self.labelColor         = labelColor
        self.subLabelColor      = subLabelColor
        self.labelFormat        = labelFormat
        self.subLabelSide       = subLabelSide
        self.labelFontName      = labelFontName
        self.subLabelFontName   = subLabelFontName
        self.colorTransFunc     = colorTransFunc
        
        
        # Misc
        self.transFunc      = transFunc
        
        # Label Var
        self.labelVar.set(self.defaultValue)
        
        self.draw()
        
    def draw(self):
        if(DEBUG):
            self.configure(borderwidth=5, relief=tk.GROOVE)
        
        if(self.labelFontName != None):
            mainLabelFont   = tkFont.Font(family=self.labelFontName, size=self.labelFontSize)
        else:
            mainLabelFont   = tkFont.Font(size=self.labelFontSize)
        
        self.mainLabel  = tk.Label(self, textvariable=self.labelVar, background='#000', foreground=self.labelColor, font=mainLabelFont)
        
        if(self.subLabelSide == tk.LEFT or self.subLabelSide == tk.RIGHT):
            if(self.subLabelSide == tk.LEFT):
                self.mainLabel.pack(fill=tk.BOTH, side=tk.RIGHT, expand = 1) # , expand = 1
            else:
                self.mainLabel.pack(fill=tk.BOTH, side=tk.LEFT, expand = 1) # , expand = 1
        else:
            if(self.subLabelSide == tk.TOP):
                self.mainLabel.pack(fill=tk.BOTH, side=tk.BOTTOM, expand = 1) #, expand = 1
            else:
                self.mainLabel.pack(fill=tk.BOTH, side=tk.TOP, expand = 1) #, expand = 1
        
        
        if(DEBUG):
            self.configure(borderwidth=5, relief=tk.GROOVE)
            
        if(self.subLabelName != None):
            if(self.subLabelFontName != None):
                subLabelFont    = tkFont.Font(family=self.subLabelFontName, size=self.subLabelFontSize)
            else:
                subLabelFont    = tkFont.Font(size=self.subLabelFontSize)
                
            self.subLabel   = tk.Label(self, text=self.subLabelName, background='#000', foreground=self.subLabelColor, font=subLabelFont)
            #self.subLabel.pack(fill=tk.BOTH, expand=1)
            
            if(self.subLabelSide == tk.LEFT or self.subLabelSide == tk.RIGHT):
                self.subLabel.pack(fill=tk.Y, side=self.subLabelSide) # , expand = 1
            else:
                self.subLabel.pack(fill=tk.X, side=self.subLabelSide) #, expand = 1
                
            if(DEBUG):
                self.subLabel.configure(borderwidth=5, relief=tk.GROOVE)
        
    # Update Label var with Source Data
    def refresh(self):
        val = self.dataSource.getField(self.dataFieldName)
        
        if(isinstance(val, (list))):
            val = self.dataSource.getField(self.dataFieldName)[self.dataFieldIndex]
        
        if(val == None):
            self.labelVar.set(self.defaultValue)
        else:
            if(self.transFunc != None):
                val = self.transFunc(val)
                
            if(self.labelFormat != None):
                val = self.labelFormat.format(val)
            
            self.labelVar.set(str(val))
            
        if(self.colorTransFunc != None):
            color = self.colorTransFunc(val)
            self.mainLabel.configure(foreground=color)
                
class TimeLabel(GenericLabel):
    def __init__(self, master=None, posX=0, posY=0, height=10, width=10, subLabelName=None, dataSource=None, dataFieldName=None, dataFieldIndex=0, defaultValue=None, labelFormat=None, labelFontName=None, labelFontSize=20, subLabelFontSize=5, labelColor='#fff', subLabelColor='#fff', subLabelFontName=None, subLabelSide=tk.BOTTOM, transFunc=None, colorTransFunc=None, condColor='#f00'):
        GenericLabel.__init__(self, master, posX, posY, height, width, subLabelName, dataSource, dataFieldName, dataFieldIndex, defaultValue, labelFormat, labelFontName, labelFontSize, subLabelFontSize, labelColor, subLabelColor, subLabelFontName, subLabelSide, transFunc, colorTransFunc)
        
        self.condColor = condColor
        
    # Update Label var with Source Data
    def refresh(self):
        val = self.dataSource.getField(self.dataFieldName)
        
        
        if(isinstance(val, (list))):
            if(val[1] == 1):
                self.mainLabel.configure(foreground=self.condColor)
            else:
                self.mainLabel.configure(foreground=self.labelColor)
            
            val = val[0]
        
        if(val == None or val == -1):
            self.labelVar.set("--:--:---")
        else:
            if(self.transFunc != None):
                val = self.transFunc(val)
            
            dec  = int((val % 1)*1000)
            r    = val
            
            mins = int(r / 60)
            secs = int(r % 60)
            
            self.labelVar.set('{:02d}:{:02d}:{:03d}'.format(mins, secs, dec))

class BarLabel(BanWidget):

    def __init__(self, master=None, posX=0, posY=0, height=10, width=10, subLabelName=None, dataSource=None, dataFieldName=None, defaultValue=None, subLabelFontSize=5, barColor='#0f0', subLabelColor='#fff', subLabelFontName=None, transFunc=None):
        BanWidget.__init__(self, master, posX, posY, height, width)
        
        # Data Source
        self.dataSource         = dataSource
        self.dataFieldName      = dataFieldName
        self.defaultValue       = defaultValue
        
        # Tk
        self.master             = master
        self.barVar             = tk.DoubleVar()
        self.barColor           = barColor
        self.subLabelName       = subLabelName
        self.subLabelFontSize   = subLabelFontSize;
        self.subLabelColor      = subLabelColor
        self.subLabelFontName   = subLabelFontName;
        
        # Misc
        self.transFunc          = transFunc
        
        self.draw()
        
    def draw(self):
        if(DEBUG):
            self.configure(borderwidth=5, relief=tk.GROOVE)
        
        s = ttk.Style() # s.theme_names() To show availables
        # print s.theme_names():  ('winnative', 'clam', 'alt', 'default', 'classic', 'vista', 'xpnative')
        #s.theme_use('classic') 
        s.theme_use('classic') 

        # WARN: Posible name colission with severals BarLabels
        styleId = 'barLabel_'+str(self.id)+'.Vertical.TProgressbar'
        s.configure(styleId, foreground='green', background=self.barColor, troughcolor ='#808080')
            
        self.progressBar = ttk.Progressbar(self, orient='vertical', mode='determinate', variable=self.barVar, style=styleId)
        
        self.progressBar.pack(fill=tk.BOTH, expand=1)

        if(self.subLabelName != None):
            if(self.subLabelFontName != None):
                subLabelFont    = tkFont.Font(family=self.subLabelFontName, size=self.subLabelFontSize)
            else:
                subLabelFont    = tkFont.Font(size=self.subLabelFontSize)
                
            self.subLabel   = tk.Label(self, text=self.subLabelName, background='#000', foreground=self.subLabelColor, font=subLabelFont)
            self.subLabel.pack(fill=tk.X)

            if(DEBUG):
                self.subLabel.configure(borderwidth=5, relief=tk.GROOVE)
            
            
    # Update Label var with Source Data
    def refresh(self):
        val = self.dataSource.getField(self.dataFieldName)

        if(val == None):
            self.barVar.set(float(0))
        else:
            if(self.transFunc != None):
                val = self.transFunc(val)
            
            self.barVar.set(float(val))


class TableLabels(BanWidget):
    def __init__(self, master=None, posX=0, posY=0, height=10, width=10, rows=0, columns=0, dataSource=None, dataFieldName=None, defaultValue='-', labelFontSize=5, tableLabelName=None, tableLabelFontSize=5, labelColor='#fff', tableLabelColor='#fff', tableLabelFontName=None, transFunc=None, colorTransFunc=None):
        BanWidget.__init__(self, master, posX, posY, height, width)
        
        
        self.rows                   = rows
        self.columns                = columns
        self.dataFieldName          = dataFieldName
        
        # Data Source
        self.dataSource             = dataSource
        self.dataFieldName          = dataFieldName
        self.defaultValue           = defaultValue
        
        # Tk
        self.master                 = master
        self.labelFontSize          = labelFontSize
        self.labelColor             = labelColor

        self.tableLabelName         = tableLabelName
        self.tableLabelFontSize     = tableLabelFontSize;
        self.tableLabelColor        = tableLabelColor
        self.tableLabelFontName     = tableLabelFontName
        self.colorTransFunc         = colorTransFunc
        
        # Misc
        self.transFunc            = transFunc
        
        self.items                = []
        self.draw()
        
    def draw(self):
        
        tableFrame = tk.Frame(self)
        tableFrame.pack(fill=tk.BOTH, expand = 1, side = tk.BOTTOM)
        
        if(DEBUG):
            tableFrame.configure(borderwidth=5, relief=tk.GROOVE, background='#123')
            
        if(self.dataFieldName != None):
        
            # Create Label Widgets
            for r in range(0, self.rows):
                for c in range(0, self.columns):
                    tyreLabel = GenericLabel(   master=tableFrame,
                                                dataSource=self.dataSource, dataFieldName=self.dataFieldName, dataFieldIndex=((r*self.columns)+c), defaultValue=self.defaultValue,
                                                labelFontSize=self.labelFontSize, labelColor=self.labelColor, labelFontName=self.tableLabelFontName, transFunc=self.transFunc, colorTransFunc=self.colorTransFunc)
                    tyreLabel.grid(row=r, column=c, sticky=(tk.W,tk.E,tk.N,tk.S)) 
                    
                    self.items.append(tyreLabel)
                    
                    if(DEBUG):
                        tyreLabel.configure(borderwidth=5, relief=tk.GROOVE, background='#fff')
            
            # WTF Si no relleno con otro elemento una fila mas,  el ultimo elemento de la tabla sale mas pequeno si la tabla tiene etiqueta ??
            tmpLabel=tk.Label(tableFrame,background='#000')
            tmpLabel.grid(row=self.rows+1, column=0, sticky=(tk.W,tk.E), columnspan=self.columns) 
                 
            for r in range(0, self.rows):
                tableFrame.rowconfigure(r, weight=1)
                
            for c in range(0, self.columns):
                tableFrame.columnconfigure(c, weight=1)
        
        if(self.tableLabelName != None):
            if(self.tableLabelFontName != None):
                tableLabelFont    = tkFont.Font(family=self.tableLabelFontName, size=self.tableLabelFontSize)
            else:
                tableLabelFont    = tkFont.Font(size=self.tableLabelFontSize)
                
            self.tableLabel   = tk.Label(self, text=self.tableLabelName, background='#000', foreground=self.tableLabelColor, font=tableLabelFont)
            self.tableLabel.pack(fill=tk.BOTH, side = tk.TOP) # side= tk.BOTTOM, LEFT, RIGHT, TOP
            
    def refresh(self):
        for i in self.items:
            i.refresh()
            
        
class PCarsDataSource():
    
    def __init__(self):
        # Inifializar WS
        self.jsonData           = None
        #self.url                = 'http://localhost:8080/getdata' Reinstall pycurl or change ipv6
        self.url                = 'http://192.168.15.101:8080/getdata'
        self.json_dataFields    = json.dumps({"fields": ["MGEAR", "MRPM", "MSPEED", "MFUELLEVEL", 
                                                         "EXT_MCURRENTTIME",  "MBESTLAPTIME", "EXT_MLASTLAPTIME",
                                                         "MBOOSTAMOUNT", "MFUELLEVEL", "MTYRETREADTEMP", "MBRAKETEMPCELSIUS", 
                                                         "MCARNAME", "MOILTEMPCELSIUS", "MWATERTEMPCELSIUS",
                                                         "EXT_MSESSIONSECTORGAP", "EXT_MSESSIONSECTORDELTA", "EXT_MPOSITION",
                                                         "MTHROTTLE", "MCLUTCH", "MBRAKE",
                                                         "MAERODAMAGE", "MENGINEDAMAGE", "EXT_MCRASHSTATE"]})                                                         
                                                         
        self.curl = pycurl.Curl()
        self.curl.setopt(pycurl.URL, self.url)
        self.curl.setopt(pycurl.POST, 1)
        self.curl.setopt(pycurl.POSTFIELDS, self.json_dataFields)
        
        if(DEBUG):
            self.curl.setopt(pycurl.VERBOSE, 1)
        
    def getField(self, fieldName):
        if (self.jsonData == None):
            return None
        else:
            return self.jsonData["data"][fieldName]
        
    def update(self):
        # Rest Call updating Data Source values
        
        # Option 1: requests library. So slow! 1 second per request
        #start       = time.time()
        #response    = requests.post(self.url, data=self.json_dataFields, verify=False)
        #print("Time Rest Request: "+ str(time.time()-start))
        #data_json = json.loads(response.content)
        
            
        # Option 2: pycurl
        buffer  = StringIO()
        self.curl.setopt(self.curl.WRITEDATA, buffer)
        self.curl.perform()
        
        # Update Data Source
        self.jsonData = json.loads(buffer.getvalue())
        buffer.close()
        #c.close()
        #print self.jSonData
        
        return
    
    
#def refreshData(dataSrc):
#    while True:
#        dataSrc.update()
#        time.sleep(DATA_REFRESH_DELAY_MILLIS/1000)
        
def refreshGui():
      
    try:
        # Data refresh
        if(DEBUG):
            start = time.time()
        
        DATA.update()
        
        if(DEBUG):
            print("Time Get Data : "+ str(time.time()-start))
    
        # Gui refresh
        if(DEBUG):
            start = time.time()
        
        GUI.refresh()

        if(DEBUG):
            print("Time Refresh Gui : "+ str(time.time()-start))
        
        GUI.after(GUI_REFRESH_DELAY_MILLIS, refreshGui)
    except:
        print("Error refreshing Data", sys.exc_info()[0])
        GUI.after(5000, refreshGui)
    
    
# Constants 
GUI_REFRESH_DELAY_MILLIS = 25

# Global Vars
GUI     = None
DATA    = None
DEBUG   = False 

#######
# Main
#######

if __name__ == "__main__":
    
    win = Win()
    win.geometry('%dx%d+%d+%d' % (1280, 720, 350, 150))

    DATA = PCarsDataSource()
    GUI  = banpcarsscreens.Screen1(master=win, dataSource=DATA)
    #GUI  = banpcarsscreens.Screen2(master=win, dataSource=DATA)
    GUI.master.title('BanPCars Display')
    
    #t = threading.Thread(target=refreshData, args=[dataSrc])
    #t.start()
    
    GUI.after(GUI_REFRESH_DELAY_MILLIS, refreshGui)
    win.mainloop()
    

