class cfgc:
    def __init__(self):
        self.reduct=""
        self.part=[]

class prsrc:
    def __init__(self):
        self.prsid=""
        self.cfg=cfgc()
        self.head=0
        self.bind=""

class segc:
    def __init__(self):
        self.seq=""
        self.flag=""  # 'w': word string; 't': word and its tag; 'p': phrase

class regionc:
    def __init__(self,begin=0,end=0):
        self.begin=begin
        self.end=end

class tagc:
    def __init__(self):
        self.sentreg=regionc()
        self.word=""
        self.cat=""
        self.catid=""
        self.feat=""
        self.ruleset=[]

class edgec:
    def __init__(self):
        self.id=None
        self.prsid=None
        self.ruleid=""
        self.feat=""
        self.dotreg=regionc()
        self.sentreg=regionc()
        self.son=[]
        self.father=-2
        self.source="" # dict or prsrbase or input, d or p or i
        self.state=""  # active or silent or fix, a or s or f, fix 

class loccatc:
    def __init__(self,loc=0,cat=""):
        self.loc=loc
        self.cat=cat

class compedgekeyc:
    def __init__(self,reduct,sentreg,feat):
        self.reduct=reduct
        self.sentreg=sentreg
        self.feat=feat

class incedgekeyc:
    def __init__(self,prsid="",dotreg=regionc(),sentreg=regionc(),son=[]):
        self.prsid=prsid
        self.dotreg=dotreg
        self.sentreg=sentreg
        self.son=son
