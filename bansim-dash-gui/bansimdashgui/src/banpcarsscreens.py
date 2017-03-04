import bansimdashgui as bdash

import Tkinter as tk

class Screen1(bdash.BanScreen):
    
    
    def createScreen(self):
        
        # Car Name
        carLabel            = bdash.GenericLabel(   master=self, posX=400, posY=350, height=110, width=550, 
                                                    dataSource=self.dataSource, dataFieldName="MCARNAME", defaultValue='-', labelFontName='Digital-7 Mono',
                                                    labelFontSize=45, labelColor='#ffc90e', subLabelFontName='Digital-7 Mono')
        carLabel.place()
        self.addItem(carLabel)
        

        # Speed
        speedLabel          = bdash.GenericLabel(   master=self, posX=550, posY=10, height=110, width=180, 
                                                    #subLabelName="SPEED",  subLabelColor='#ffc90e', subLabelFontSize=12, 
                                                    dataSource=self.dataSource, dataFieldName="MSPEED", defaultValue='0',  labelFontName='Digital-7 Mono',
                                                    labelFontSize=50, labelColor='#ffc90e', subLabelFontName='Digital-7 Mono',
                                                    transFunc=lambda x: int(x*3.6))
        speedLabel.place()
        self.addItem(speedLabel)
        
        #Rpms
        rpmLabel            = bdash.GenericLabel(   master=self, posX=95, posY=20, height=50, width=280, 
                                                    subLabelName="RPM ", subLabelFontSize=35, subLabelColor='#80FF80', subLabelSide=tk.LEFT, labelFontName='Digital-7 Mono',
                                                    dataSource=self.dataSource, dataFieldName="MRPM", defaultValue='0',  subLabelFontName='Digital-7 Mono',
                                                    labelFontSize=50, labelColor='#ffc90e',
                                                    transFunc=lambda x: str(int(round(x))).rjust(5))
        rpmLabel.place()
        self.addItem(rpmLabel)
        
        # Gear
        gearLabel           = bdash.GenericLabel(   master=self, posX=550, posY=100, height=250, width=180, 
                                                    #subLabelName="GEAR", subLabelColor='#0f0', subLabelFontSize=20,  subLabelFontName='Digital-7 Mono'
                                                    dataSource=self.dataSource, dataFieldName="MGEAR", defaultValue='N',  labelFontName='Digital-7 Mono',
                                                    labelFontSize=200, labelColor='#ffc90e',
                                                    transFunc=lambda x: 'N' if x == 0 else ('R' if x == -1 else str(x)))
        gearLabel.place()
        self.addItem(gearLabel)
                 
       
        # Fuel Bar
        fuelBar             = bdash.BarLabel(   master=self, posX=85, posY=560, height=150, width=82, 
                                                subLabelName="FUEL", dataSource=self.dataSource, dataFieldName="MFUELLEVEL",
                                                subLabelFontSize=25, subLabelFontName = 'Digital-7 Mono',
                                                barColor='#804040', subLabelColor='#804040',
                                                transFunc=lambda x: x*100)   
                                
                                    
        fuelBar.place()
        self.addItem(fuelBar)
        
        
        # Session Info
        posLabel            = bdash.GenericLabel(   master=self, posX=775, posY=20, height=80, width=250, 
                                                    subLabelName="POS", subLabelFontSize=40, subLabelColor='#80FF80', subLabelSide=tk.LEFT, labelFontName='Digital-7 Mono',
                                                    dataSource=self.dataSource, dataFieldName="EXT_MPOSITION", defaultValue='0',  subLabelFontName='Digital-7 Mono',
                                                    labelFontSize=50, labelColor='#ffc90e')
        posLabel.place()
        self.addItem(posLabel)
        
        
        
        #################
        # Times 
        #################
        
        # Lap Time
        timesFontSize       = 35
        timesLabelFontSize  = 15
        timeHeight          = 95
        timeWidth           = 210
        
        pX = 1050
        pY = 10
        
        lapTimeLabel        = bdash.TimeLabel(  master=self, posX=pX, posY=pY, height=timeHeight, width=timeWidth, labelColor='#8ff',
                                                subLabelName="LAP TIME", dataSource=self.dataSource, dataFieldName="EXT_MCURRENTTIME", 
                                                labelFontSize=timesFontSize, subLabelFontSize=timesLabelFontSize,  labelFontName='Digital-7 Mono',
                                                subLabelColor='#8ff', condColor="#f00")
                                    
        lapTimeLabel.place()
        self.addItem(lapTimeLabel)
        
        pY = pY + timeHeight
        lastTimeLabel       = bdash.TimeLabel(  master=self, posX=pX, posY=pY, height=timeHeight, width=timeWidth, labelColor='#80FF80',
                                                subLabelName="LAST LAP TIME", dataSource=self.dataSource, dataFieldName="EXT_MLASTLAPTIME",
                                                labelFontSize=timesFontSize, subLabelFontSize=timesLabelFontSize,  labelFontName='Digital-7 Mono',
                                                subLabelColor='#80FF80', condColor="#f00")
                                    
        lastTimeLabel.place()
        self.addItem(lastTimeLabel)
        
        pY = pY + timeHeight
        bestTimeLabel       = bdash.TimeLabel(  master=self, posX=pX, posY=pY, height=timeHeight, width=timeWidth, 
                                                subLabelName="BEST LAP TIME", dataSource=self.dataSource, dataFieldName="MBESTLAPTIME", subLabelColor='#f8f',
                                                labelFontSize=timesFontSize, subLabelFontSize=timesLabelFontSize, labelFontName='Digital-7 Mono', labelColor='#f8f')
                                    
        bestTimeLabel.place()
        self.addItem(bestTimeLabel)


        batteryBar          = bdash.BarLabel(   master=self, posX=10, posY=10, height=700, width=70, 
                                                subLabelName="BAT", dataSource=self.dataSource, dataFieldName="MBOOSTAMOUNT",
                                                subLabelFontSize=25, subLabelFontName = 'Digital-7 Mono',
                                                barColor='#80FF80', subLabelColor='#80FF80')
                                    
        batteryBar.place()
        self.addItem(batteryBar)
        
        
        # Sector Gap 
        sectorGapTable      = bdash.TableLabels(    master=self, posX=90, posY=100, height=175, width=350, rows=1, columns=3, 
                                                    dataSource=self.dataSource, dataFieldName="EXT_MSESSIONSECTORGAP", 
                                                    labelColor='#8ff', labelFontSize=40,
                                                    tableLabelName="SESSION GAP", tableLabelFontSize=30, tableLabelColor='#8ff',  tableLabelFontName='Digital-7 Mono',
                                                    transFunc=lambda x: '--' if (x == -999999) else '{:.3f}'.format(x),
                                                    colorTransFunc=lambda x: '#8ff' if (x == '--') else ('#f00' if (float(x) > 0.0) else '#0f0'))
        
        sectorGapTable.place()
        self.addItem(sectorGapTable)
        
        # Delta Lap
        deltaLapLabel       = bdash.GenericLabel(   master=self, posX=90, posY=255, height=180, width=300, 
                                                    subLabelName="DELTA SEC", subLabelFontSize=25, subLabelColor='#8ff', subLabelSide=tk.BOTTOM,
                                                    dataSource=self.dataSource, dataFieldName="EXT_MSESSIONSECTORDELTA", defaultValue='0', labelFontName='Digital-7',
                                                    labelFontSize=80, labelColor='#ffc90e', subLabelFontName='Digital-7',
                                                    transFunc=lambda x: '-.---' if (x == -999999) else '{:.3f}'.format(x),
                                                    colorTransFunc=lambda x: '#8ff' if (x == '-.---') else ('#f00' if (float(x) > 0.0) else '#0f0'))
        deltaLapLabel.place()
        self.addItem(deltaLapLabel)

        
        
        
        
        # Tyres Temp
        tyreTempTable       = bdash.TableLabels(    master=self, posX=475, posY=500, height=225, width=225, rows=2, columns=2, 
                                                    dataSource=self.dataSource, dataFieldName="MTYRETREADTEMP", 
                                                    labelColor='#8ff', labelFontSize=45,
                                                    tableLabelName="TYRE TEMP", tableLabelFontSize=20, tableLabelColor='#8ff',  tableLabelFontName='Digital-7 Mono',
                                                    transFunc=lambda x: str(int(x-273.15)).rjust(3) if (x > 0) else '--',
                                                    colorTransFunc=lambda x: '#8ff' if (x == '--') else ('#8ff' if (float(x) < 130) else '#f00'))
        
        tyreTempTable.place()
        self.addItem(tyreTempTable)
        
        # Brakes Temp
        tyreTempTable       = bdash.TableLabels(    master=self, posX=700, posY=500, height=225, width=225, rows=2, columns=2, 
                                                    dataSource=self.dataSource, dataFieldName="MBRAKETEMPCELSIUS", 
                                                    labelColor='#ff80c0', labelFontSize=45,
                                                    tableLabelName="BRAKE TEMP", tableLabelFontSize=20, tableLabelColor='#ff80c0', tableLabelFontName='Digital-7 Mono',
                                                    transFunc=lambda x: str(int(x)).rjust(3) if (x > 0) else '--',
                                                    colorTransFunc=lambda x: '#ff80c0' if (x == '--') else ('#ff80c0' if (float(x) < 350) else '#f00'))
        
        
        tyreTempTable.place()
        self.addItem(tyreTempTable)
        
        # Oil Temp
        oilTempLabel        = bdash.GenericLabel(   master=self, posX=970, posY=580, height=75, width=260, 
                                                    subLabelName="OIL TEMP   ",  subLabelColor='#FF8080', subLabelFontSize=25, subLabelSide=tk.LEFT, subLabelFontName='Digital-7 Mono',
                                                    dataSource=self.dataSource, dataFieldName="MOILTEMPCELSIUS", defaultValue='0', 
                                                    labelFontSize=45, labelColor='#ffc90e',  labelFontName='Digital-7 Mono',
                                                    transFunc=lambda x: str(int(x)).rjust(3) if (x > 0) else '--')
        oilTempLabel.place()
        self.addItem(oilTempLabel)
        
        # Water Temp
        waterTempLabel      = bdash.GenericLabel(   master=self, posX=970, posY=640, height=75, width=260, 
                                                    subLabelName="WATER TEMP ",  subLabelColor='#FF8080', subLabelFontSize=25, subLabelSide=tk.LEFT, subLabelFontName='Digital-7 Mono',
                                                    dataSource=self.dataSource, dataFieldName="MWATERTEMPCELSIUS", defaultValue='0', 
                                                    labelFontSize=45, labelColor='#ffc90e',  labelFontName='Digital-7 Mono',
                                                    transFunc=lambda x: str(int(x)).rjust(3) if (x > 0) else '--')
        waterTempLabel.place()
        self.addItem(waterTempLabel)
        
        
        # Pedals
        #pedalsFontSize       = 35
        pedalsLabelFontSize  = 25
        pedalsHeight          = 250
        pedalsWidth           = 30
        
        pX = 1200
        pY = 320
        
        # Throttle
        pedalBar = bdash.BarLabel(  master=self, posX=pX, posY=pY, height=pedalsHeight, width=pedalsWidth, 
                                    subLabelName="T", dataSource=self.dataSource, dataFieldName="MTHROTTLE",
                                    subLabelFontSize=pedalsLabelFontSize, subLabelFontName = 'Digital-7 Mono',
                                    barColor='#80FF80', subLabelColor='#80FF80',
                                    transFunc=lambda x: x*100)   
        pedalBar.place()
        self.addItem(pedalBar)
        
        # Brake
        pX = pX - pedalsWidth
        pedalBar = bdash.BarLabel(  master=self, posX=pX, posY=pY, height=pedalsHeight, width=pedalsWidth, 
                                    subLabelName="B", dataSource=self.dataSource, dataFieldName="MBRAKE",
                                    subLabelFontSize=pedalsLabelFontSize, subLabelFontName = 'Digital-7 Mono',
                                    barColor='#FF8080', subLabelColor='#FF8080',
                                    transFunc=lambda x: x*100)   
        pedalBar.place()
        self.addItem(pedalBar)
        
        # Clutch
        pX = pX - pedalsWidth
        pedalBar = bdash.BarLabel(  master=self, posX=pX, posY=pY, height=pedalsHeight, width=pedalsWidth, 
                                    subLabelName="C", dataSource=self.dataSource, dataFieldName="MCLUTCH",
                                    subLabelFontSize=pedalsLabelFontSize, subLabelFontName = 'Digital-7 Mono',
                                    barColor='#8080FF', subLabelColor='#8080FF',
                                    transFunc=lambda x: x*100)   
        pedalBar.place()
        self.addItem(pedalBar)
        
        
        # Damages
        
        # Crash State
        posX                = 200
        posY                = 570
        damHeight           = 40
        damWidth            = 250
        damFontSize         = 30
        damSubFonSize       = 20
        
        crashStateLabel = bdash.GenericLabel(   master=self, posX=posX, posY=posY, height=damHeight, width=damWidth, 
                                                subLabelName="CRASH STATE ", subLabelFontSize=damSubFonSize, subLabelColor='#FF8080', subLabelSide=tk.LEFT, labelFontName='Digital-7 Mono',
                                                dataSource=self.dataSource, dataFieldName="EXT_MCRASHSTATE", defaultValue='0',  subLabelFontName='Digital-7 Mono',
                                                labelFontSize=damFontSize, labelColor='#ffc90e',
                                                colorTransFunc=lambda x: '#ffc90e' if (x == 'NONE') else '#f00')
        crashStateLabel.place()
        self.addItem(crashStateLabel)
        
        # Aero Damage
        posY = posY + damHeight
        aeroDamLabel = bdash.GenericLabel(  master=self, posX=posX, posY=posY, height=damHeight, width=damWidth, 
                                            subLabelName="AERO   DMG  ", subLabelFontSize=damSubFonSize, subLabelColor='#FF8080', subLabelSide=tk.LEFT, labelFontName='Digital-7 Mono',
                                            dataSource=self.dataSource, dataFieldName="MAERODAMAGE", defaultValue='0',  subLabelFontName='Digital-7 Mono',
                                            labelFontSize=damFontSize, labelColor='#ffc90e',
                                            transFunc=lambda x: '-' if (x < 0) else int(x*100),
                                            colorTransFunc=lambda x: '#ffc90e' if (x <= 0) else '#f00')
        aeroDamLabel.place()
        self.addItem(aeroDamLabel)
        
        # Engine Damage
        posY = posY + damHeight
        engineDamLabel = bdash.GenericLabel(    master=self, posX=posX, posY=posY, height=damHeight, width=damWidth, 
                                                subLabelName="ENGINE DMG  ", subLabelFontSize=damSubFonSize, subLabelColor='#FF8080', subLabelSide=tk.LEFT, labelFontName='Digital-7 Mono',
                                                dataSource=self.dataSource, dataFieldName="MENGINEDAMAGE", defaultValue='0',  subLabelFontName='Digital-7 Mono',
                                                labelFontSize=damFontSize, labelColor='#ffc90e',
                                                transFunc=lambda x: '-' if (x < 0) else int(x*100),
                                                colorTransFunc=lambda x: '#ffc90e' if (x <= 0) else '#f00')
        engineDamLabel.place()
        self.addItem(engineDamLabel)
        
        # Image Example
#        image = Image.open("../ext-resources/bmw_logo.bmp")
#        photo = ImageTk.PhotoImage(image)
#        
#        l = tk.Label(self, image=photo)
#        l.image = photo
#        l.place(x=800, y=130, height=195, width=195)
        
        
class Screen2(bdash.BanScreen):
    
    
    def createScreen(self):
        
        canvas = tk.Canvas(self, width=500, height=200, background='#808080')
        
        

        canvas.create_line(0, 100, 200, 50, 400, 75, smooth=1,  splinesteps=50, width=3)
#        canvas.create_line(0, 100, 200, 0, fill="red", dash=(4, 4))
#        canvas.create_rectangle(50, 25, 150, 75, fill="blue")
        
        
        
        canvas.place(x=200, y=200)
        self.addItem(canvas)