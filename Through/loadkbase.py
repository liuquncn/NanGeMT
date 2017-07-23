import re
import string
import globvar
from prsrbase import *

def loadspeword():
    spewordf=open(globvar.spewordfile,'r')
    globvar.shi=spewordf.readline().strip('\n')
    globvar.fou=spewordf.readline().strip('\n')
    globvar.hanzi=spewordf.readline().strip('\n')
    globvar.hanyucilei=spewordf.readline().strip('\n')
    globvar.hanyuduanyu=spewordf.readline().strip('\n')
    globvar.chongdie=spewordf.readline().strip('\n')
    globvar.liang=spewordf.readline().strip('\n')
    globvar.er=spewordf.readline().strip('\n')
    globvar.le=spewordf.readline().strip('\n')
    globvar.yi=spewordf.readline().strip('\n')
    globvar.guo=spewordf.readline().strip('\n')
    globvar.bu=spewordf.readline().strip('\n')
    globvar.yuyilei=spewordf.readline().strip('\n')
    globvar.yinjie=spewordf.readline().strip('\n')
    globvar.jufachengfen=spewordf.readline().strip('\n')
    globvar.yuyichengfen=spewordf.readline().strip('\n')
    globvar.zhongxinyu=spewordf.readline().strip('\n')
    globvar.yingyucilei=spewordf.readline().strip('\n')
    globvar.Rliang=spewordf.readline().strip('\n')
    globvar.Ner=spewordf.readline().strip('\n')
    globvar.NMer=spewordf.readline().strip('\n')
    globvar.VleV=spewordf.readline().strip('\n')
    globvar.VyiV=spewordf.readline().strip('\n')
    globvar.VleN=spewordf.readline().strip('\n')
    globvar.VguoN=spewordf.readline().strip('\n')
    globvar.VleyiN=spewordf.readline().strip('\n')
    globvar.VguoyiN=spewordf.readline().strip('\n')
    globvar.VbuleN=spewordf.readline().strip('\n')
    globvar.sczlxiaoshu=spewordf.readline().strip('\n')
    globvar.sczlfuhao=spewordf.readline().strip('\n')
    globvar.sczlshuliang=spewordf.readline().strip('\n')
    globvar.mczlng=spewordf.readline().strip('\n')
    spewordf.close()

def loadkbase():
    modelf=open(globvar.modelfile,'r')
    dictnf=open(globvar.dictnfile,'r')
    prsrbasef=open(globvar.prsrbasefile,'r')
    makrbasef=open(globvar.makrbasefile,'r')
    bldrbasef=open(globvar.bldrbasefile,'r')

    globvar.model=modelf.read()
    globvar.dictn=dictnf.read()
    globvar.prsrbase=prsrbasef.read()
    globvar.makrbase=makrbasef.read()
    globvar.bldrbase=bldrbasef.read()
    
    commre=re.compile(globvar.commreg,re.S)
    globvar.model=commre.sub("",globvar.model)
    globvar.dictn=commre.sub("",globvar.dictn)
    globvar.prsrbase=commre.sub("",globvar.prsrbase)
    globvar.makrbase=commre.sub("",globvar.makrbase)
    globvar.bldrbase=commre.sub("",globvar.bldrbase)

    globvar.prsrset=globvar.prsrbase.split(globvar.prsrintro)
    globvar.makrset=globvar.makrbase.split(globvar.makrintro)
    globvar.bldrset=globvar.bldrbase.split(globvar.bldrintro)
    del globvar.bldrset[0]
    del globvar.prsrset[0]
    del globvar.makrset[0]

    shallowprsrbase()
