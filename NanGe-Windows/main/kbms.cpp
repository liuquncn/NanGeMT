#include "../kbase/model.h"
#include "../trans_ce/cetrans.h"
#include "../kbase/template.h"
#include "../trans/result.h"

int GlobalSrcLang;
int GlobalTgtLang;
Model * GlobalModel;
ChnDictn * GlobalDictn;
ChnDictn * GlobalDictnref;
Template * GlobalTemplate;
ChnOvlRbase * GlobalOvlRbase;
ChnSegRbase * GlobalSegRbase;
ChnTagRbase * GlobalTagRbase;
ChnPrsRbase * GlobalPrsRbase;
EngMakRbase * GlobalMakRbase;
EngBldRbase * GlobalBldRbase;
ChnLexDbase * GlobalLexDbase;

unsigned char OptionNewLine;
unsigned char OptionSpaceLine;
unsigned char OptionEmptyLine;
unsigned char OptionCommaColon;
unsigned char OptionLexdbase;
unsigned char OptionDictnref;
unsigned char OptionAttribute;
unsigned char OptionTrace;
unsigned int  OptionMultiUser;
unsigned int  OptionResultNumber;
unsigned int  OptionPreEdit;

int main(int argc, char* argv[])
{
  GlobalModel = new Model;
  GlobalDictn = new ChnDictn;
  GlobalDictnref = new ChnDictn;
  GlobalTemplate = new Template;
  GlobalOvlRbase = new ChnOvlRbase;
  GlobalSegRbase = new ChnSegRbase;
  GlobalTagRbase = new ChnTagRbase;
  GlobalPrsRbase = new ChnPrsRbase;
  GlobalMakRbase = new EngMakRbase;
  GlobalBldRbase = new EngBldRbase;
  GlobalLexDbase = new ChnLexDbase;
  
  Kbase::setHome("../../mtoutput/kbase"); 
  
  GlobalSrcLang=CHINESE;
  GlobalTgtLang=ENGLISH;
  GlobalModel->setPath("model");
  GlobalModel->setName("model");
  GlobalDictn->setPath("dictn");
  GlobalDictn->setName("dictn");
  GlobalDictn->setTgtLang(GlobalTgtLang);
  GlobalDictnref->setPath("dictnref");
  GlobalDictnref->setName("dictnref");
  GlobalDictnref->setTgtLang(GlobalTgtLang);
  GlobalTemplate->setPath("template");
  GlobalTemplate->setName("template");
  GlobalOvlRbase->setPath("ovlrbase");
  GlobalOvlRbase->setName("ovlrbase");
  GlobalSegRbase->setPath("segrbase");
  GlobalSegRbase->setName("segrbase");
  GlobalTagRbase->setPath("tagrbase");
  GlobalTagRbase->setName("tagrbase");
  GlobalPrsRbase->setPath("prsrbase");
  GlobalPrsRbase->setName("prsrbase");
  GlobalPrsRbase->setTgtLang(GlobalTgtLang);
  GlobalMakRbase->setPath("makrbase");
  GlobalMakRbase->setName("makrbase");
  GlobalBldRbase->setPath("bldrbase");
  GlobalBldRbase->setName("bldrbase");
  GlobalLexDbase->setPath("lexdbase");
  GlobalLexDbase->setName("lexdbase");
  
  // updateAll();
  GlobalModel->update();

  GlobalDictn->testSituation();
  GlobalDictn->update();
  GlobalDictnref->testSituation();
  GlobalDictnref->update();
  GlobalTemplate->testSituation();
  GlobalTemplate->update(); 
  GlobalOvlRbase->testSituation();
  GlobalOvlRbase->update();
  GlobalSegRbase->testSituation();
  GlobalSegRbase->update();
  GlobalTagRbase->testSituation();
  GlobalTagRbase->update(); 
  GlobalPrsRbase->testSituation();
  GlobalPrsRbase->update();
  GlobalMakRbase->testSituation();
  GlobalMakRbase->update();
  GlobalBldRbase->testSituation();
  GlobalBldRbase->update(); 
  GlobalLexDbase->testSituation();
  GlobalLexDbase->update(); 
  
  return 0;
}
