import globvar
from dataset import *
from basfunc import *

def compseg(sent):
    remain=sent
    segs=[]
    while (remain!=""):
        seg=segc()
        seg.seq,remain=getoneunit(remain)
        segs.append(seg)
    for i in range(0,len(segs)):
        if (segs[i].seq.find("(")!=-1):
            segs[i].flag="p"
        elif (segs[i].seq.find("/")!=-1):
            segs[i].flag="t"
        else:
            segs[i].flag="w"
    return(segs)
    
def dictsegment(sent):
    words=[]
    wordcount=[10000000 for i in range(len(sent)+1)]
    traceback=[i-1 for i in range(len(sent)+1)]
    tracefront=[i+1 for i in range(len(sent)+1)]
    wordcount[0]=0
    for i in range (0,len(sent)+1):
        for j in range (0,i):
            keyword="$$ "+sent[j:i]+"\n"
            ebpos=globvar.dictn.find(keyword)
            if (ebpos!=-1):
                currcount=wordcount[j]-2*(i-j)
            else:
                currcount=wordcount[j]+2*(i-j)
            if (currcount<wordcount[i]):
                traceback[i]=j
                wordcount[i]=currcount
    i=len(sent)
    while (i>0):
        tracefront[traceback[i]]=i
        i=traceback[i]
    i=0
    while (i<len(sent)):
        words.append(sent[i:tracefront[i]])
        i=tracefront[i]
    return(words)
