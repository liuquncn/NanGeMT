import re
import globvar
from dataset import *

def postag(words,initpos):
    tags=[]
    currpos=initpos
    for i in range(0,len(words)):
        subtags=maketag(words[i],"",currpos)
        currpos+=len(words[i])
        tags+=subtags
    return(tags)

def procunkword(unkword):
    cat=""
    feat=""
    if re.match(globvar.numreg,unkword):
        cat="m"
        if (unkword.find(".")!=-1):
            feat="["+globvar.sczlxiaoshu+"]"
        elif (len(unkword)==4):
            feat="["+globvar.sczlfuhao+"]"
        else:
            feat="["+globvar.sczlshuliang+"]"
    else:
        cat="n"
        feat="["+globvar.mczlng+"]"
    return(cat,feat)
        
def maketag(word,cat,initpos):
    tags=[]
    entreg="\$\$ "+word+".+?\$\$"
    entsrch=re.search(entreg,globvar.dictn,re.S)
    if (entsrch):
        entry=entsrch.group(0)[3:-2]
        catentset=entry.split(globvar.catintro)
        del catentset[0]
        for i in range(0,len(catentset)):
            catsrch=re.search(globvar.catreg,catentset[i])
            assert(catsrch)
            dictcat=catsrch.group(2)
            if (cat=="" or dictcat==cat):
                tag=tagc()
                tag.sentreg.begin=initpos
                tag.sentreg.end=initpos+len(word)
                tag.word=word
                tag.cat=dictcat
                tag.catid=re.search(globvar.idreg,catentset[i]).group(0)[1:-1]
                featsrch=re.search(globvar.featreg,catentset[i])
                if (featsrch):
                    tag.feat=featsrch.group(0)[2:-1]
                else:
                    tag.feat=""
                tag.ruleset=catentset[i].split(globvar.prsrintro)
                del tag.ruleset[0]
                tags.append(tag)
    else:
        tag=tagc()
        tag.sentreg.begin=initpos
        tag.sentreg.end=initpos+len(word)
        tag.word=word
        tag.catid="unk"
        tag.cat,tag.feat=procunkword(word)
        tags.append(tag)
    return(tags)
