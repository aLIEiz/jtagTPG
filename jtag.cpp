#include "jtag.h"
#include <QTextStream>

JtagTemp::JtagTemp()
{
    m_nextState = RUN_TEST_IDLE_STATE;
    next = nullptr;
    prev = nullptr;
}
JtagTemp::JtagTemp(Uint64 value,Uint32 bitCount)
{
    m_value = value;
    m_bitCount = bitCount;
    m_type = IR;
    m_nextState = RUN_TEST_IDLE_STATE;
    next = nullptr;
    prev = nullptr;
}
JtagTemp::JtagTemp(Uint64 value,Uint32 bitCount,Uint8 type)
{
    m_value = value;
    m_bitCount = bitCount;
    m_type = type;
    m_nextState = RUN_TEST_IDLE_STATE;
    next = nullptr;
    prev = nullptr;
}
JtagTemp::JtagTemp(Uint64 value,Uint32 bitCount,Uint8 type,Uint32 tap)
{
    m_value = value;
    m_bitCount = bitCount;
    m_type = type;
    m_tap = tap;
    m_nextState = RUN_TEST_IDLE_STATE;
    next = nullptr;
    prev = nullptr;
}
JtagTemp::JtagTemp(Uint64 value,Uint32 bitCount,Uint8 type,Uint32 tap,Uint8 jtagState)
{
    m_value = value;
    m_bitCount = bitCount;
    m_type = type;
    m_tap = tap;
    m_nextState = jtagState;
    next = nullptr;
    prev = nullptr;
}
void JtagTemp::setValue(Uint64 value)
{
    m_value = value;
}
Uint64 JtagTemp::getValue()
{
    return m_value;
}
void JtagTemp::setBitCount(Uint32 bitCount)
{
    m_bitCount = bitCount;
}
Uint32 JtagTemp::getBitCount()
{
    return m_bitCount;
}
void JtagTemp::setType(Uint8 type)
{
    m_type = type;
}
Uint8 JtagTemp::getType()
{
    return m_type;
}
void JtagTemp::setTap(Uint32 tap)
{
    m_tap = tap;
}
Uint32 JtagTemp::getTap()
{
    return m_tap;
}
void JtagTemp::setNextState(Uint8 jtagState)
{
    m_nextState = jtagState;
}
Uint8 JtagTemp::getNextState()
{
    return m_nextState;
}
JtagTemp* JtagTemp::getNext()
{
    return this->next;
}
JtagTemp* JtagTemp::getPrev()
{
    return this->prev;
}
void JtagTemp::operator=(const JtagTemp& b)
{
    this->m_value = b.m_value;
    this->m_bitCount = b.m_bitCount;
    this->m_type = b.m_type;
    this->m_nextState = b.m_nextState;
    this->m_tap = b.m_tap;
    this->prev = b.prev;
    this->next = b.next;
}
JtagList::JtagList()
{
    head = nullptr;
    tail = head;
    current = head;
    length = 0;
}
void JtagList::operator=(const JtagList b)
{
    this->removeAll();
    JtagTemp* temp = b.head;
    while(temp != nullptr)
    {
        this->append(temp);
        temp = temp->getNext();
    }
}
JtagList::JtagList(const JtagTemp* temp)
{
    JtagTemp* p = new JtagTemp;
    *p = *temp;
    head = p;
    tail = p;
    current = p;
    length=1;
    p = nullptr;
}
void JtagList::append(const JtagTemp* temp)
{
    JtagTemp* p = new JtagTemp;
    *p = *temp;
    if(this->head == nullptr)//空表的情况
    {
        this->head = p;
        this->tail = p;
        this->current = p;
        this->length = 1;
    }
    else
    {
        tail->next = p;
        p->prev = tail;
        tail = p;
        length++;
    }
    p = nullptr;
}
void JtagList::remove()
{
    JtagTemp* p;
    if(this->head == this->tail)//最后一个
    {
        p = this->tail;
        tail = nullptr;
        head = nullptr;
        length = 0;
    }
    else
    {
        p = tail;
        tail = tail->prev;
        tail->next = nullptr;
        length --;
        delete(p);
    }
    p = nullptr;
}
void JtagList::removeAll()
{
    JtagTemp* p;
    current = nullptr;
    while(this->tail)
    {
        if(this->head == this->tail)//最后一个
        {
            p = this->tail;
            tail = nullptr;
            head = nullptr;
            length = 0;
        }
        else
        {
            p = this->tail;
            this->tail = this->tail->prev;
            this->tail->next = nullptr;
            this->length --;
            delete(p);
        }
    }
    p = nullptr;
}
void LongCmdTemp::operator=(const LongCmdTemp& b)
{
    this->cmdValue = b.cmdValue;
}
LongCmdList::LongCmdList()
{
    head = nullptr;
    current = head;
    tail = head;
}
void LongCmdList::append(const LongCmdTemp* temp)
{
    LongCmdTemp* p = new LongCmdTemp;
    *p = *temp;
    if(this->head == nullptr)//空表的情况
    {
        this->head = p;
        this->tail = p;
        this->current = p;
    }
    else
    {
        tail->next = p;
        tail = p;
    }
    p = nullptr;
}
void LongCmdList::removeAll()
{
    LongCmdTemp* p;
    current = nullptr;
    while(this->head)
    {
        if(this->head == this->tail)//最后一个
        {
            p = this->tail;
            tail = nullptr;
            head = nullptr;
            delete(p);
        }
        else
        {
            p = this->head;
            this->head = this->head->next;
            delete(p);
        }
    }
    p = nullptr;
}
void BinCmdTemp::operator=(const BinCmdTemp& b)
{
    this->cmdValue = b.cmdValue;
    this->timeStamp = b.timeStamp;
    this->cycle = b.cycle;
}
BinCmdList::BinCmdList()
{
    head = nullptr;
    current = head;
    tail = head;
}
void BinCmdList::append(const BinCmdTemp* temp)
{
    BinCmdTemp* p = new BinCmdTemp;
    *p = *temp;
    if(this->head == nullptr)//空表的情况
    {
        this->head = p;
        this->tail = p;
        this->current = p;
    }
    else
    {
        tail->next = p;
        tail = p;
    }
    p = nullptr;
}
void BinCmdList::removeAll()
{
    BinCmdTemp* p;
    current = nullptr;
    while(this->head)
    {
        if(this->head == this->tail)//最后一个
        {
            p = this->tail;
            tail = nullptr;
            head = nullptr;
            delete(p);
        }
        else
        {
            p = this->head;
            this->head = this->head->next;
            delete(p);
        }
    }
    p = nullptr;
}
void ExcelCmdFor407::operator=(const ExcelCmdFor407& b)
{
    this->timeStamp = b.timeStamp;
    this->state = b.state;
}
ExcelCmdFor407List::ExcelCmdFor407List()
{
    head = nullptr;
    current = head;
    tail = head;
}
void ExcelCmdFor407List::append(const ExcelCmdFor407* temp)
{
    ExcelCmdFor407* p = new ExcelCmdFor407;
    *p = *temp;
    if(this->head == nullptr)//空表的情况
    {
        this->head = p;
        this->tail = p;
        this->current = p;
    }
    else
    {
        tail->next = p;
        tail = p;
    }
    p = nullptr;
}
void ExcelCmdFor407List::removeAll()
{
    ExcelCmdFor407* p;
    current = nullptr;
    while(this->head)
    {
        if(this->head == this->tail)//最后一个
        {
            p = this->tail;
            tail = nullptr;
            head = nullptr;
            delete(p);
        }
        else
        {
            p = this->head;
            this->head = this->head->next;
            delete(p);
        }
    }
    p = nullptr;
}
//延时功能
void sleep(unsigned int msec){
    //currnentTime 返回当前时间 用当前时间加上我们要延时的时间msec得到一个新的时刻
    QTime reachTime = QTime::currentTime().addMSecs(msec);
    //用while循环不断比对当前时间与我们设定的时间addMSecs(msec)
    while(QTime::currentTime()<reachTime){
        //如果当前的系统时间尚未达到我们设定的时刻，就让Qt的应用程序类执行默认的处理，
        //以使程序仍处于响应状态。一旦到达了我们设定的时刻，就跳出该循环，继续执行后面的语句。
    }
}
void jtagDelay(Uint32 count)
{
    for(Uint32 i = 0;i<count;i++);
}

//进入SHIFT_IR状态
void enterIrState(coreCfgTemp coreCfg,QFile* outFile,bool emu)
{
    int j;
    uint32_t jtagFreqMult = coreCfg.jtagFreqMult;
    uint32_t tdiState = coreCfg.tdiIdleState;
    bool emuWithTck = coreCfg.codeForEmuWithTck;
    if(tdiState)
    {
        if(emu)
        {
            if(jtagFreqMult == 1)
            {
                if(emuWithTck)
                    outFile->write("101\n");
                outFile->write("111\n");
                if(emuWithTck)
                    outFile->write("101\n");
                outFile->write("111\n");
                if(emuWithTck)
                    outFile->write("001\n");
                outFile->write("011\n");
                if(emuWithTck)
                    outFile->write("001\n");
                outFile->write("011\n");
            }
            else
            {
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("101\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("111\n");
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("101\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("111\n");
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("001\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("011\n");
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("001\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("011\n");
            }
        }
        else
        {
            if(jtagFreqMult == 1)
                outFile->write("0xDF,0xDF,0xCE,0xCE,");
            else
            {
                int count = (jtagFreqMult+1)/2;
                for(j=count;j>0;j--)
                    outFile->write("0xDD,");
                for(j=count;j>0;j--)
                    outFile->write("0xFF,");
                for(j=count;j>0;j--)
                    outFile->write("0xDD,");
                for(j=count;j>0;j--)
                    outFile->write("0xFF,");
                for(j=count;j>0;j--)
                    outFile->write("0xCC,");
                for(j=count;j>0;j--)
                    outFile->write("0xEE,");
                for(j=count;j>0;j--)
                    outFile->write("0xCC,");
                for(j=count;j>0;j--)
                    outFile->write("0xEE,");
                outFile->write("\n");
            }
        }
    }
    else//tdi 0
    {
        if(emu)
        {
            if(jtagFreqMult == 1)
            {
                if(emuWithTck)
                    outFile->write("100\n");
                outFile->write("110\n");
                if(emuWithTck)
                    outFile->write("100\n");
                outFile->write("110\n");
                if(emuWithTck)
                    outFile->write("000\n");
                outFile->write("010\n");
                if(emuWithTck)
                    outFile->write("000\n");
                outFile->write("010\n");
            }
            else
            {
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("100\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("110\n");
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("100\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("110\n");
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("000\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("010\n");
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("000\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("010\n");
            }
        }
        else
        {
            if(jtagFreqMult == 1)
                //                outFile->write("0xDF,0xDF,0xCE,0xCE,");
                outFile->write("0x9B,0x9B,0x8A,0x8A,");
            else
            {
                int count = (jtagFreqMult+1)/2;
                for(j=count;j>0;j--)
                    outFile->write("0x99,");
                for(j=count;j>0;j--)
                    outFile->write("0xBB,");
                for(j=count;j>0;j--)
                    outFile->write("0x99,");
                for(j=count;j>0;j--)
                    outFile->write("0xBB,");
                for(j=count;j>0;j--)
                    outFile->write("0x88,");
                for(j=count;j>0;j--)
                    outFile->write("0xAA,");
                for(j=count;j>0;j--)
                    outFile->write("0x88,");
                for(j=count;j>0;j--)
                    outFile->write("0xAA,");
                outFile->write("\n");
            }
        }
    }

    return;
}
//退出到run-test-idle状态
void quitState(coreCfgTemp coreCfg,QFile* outFile,bool emu)
{
    uint32_t j;
    uint32_t jtagFreqMult = coreCfg.jtagFreqMult;
    uint32_t tdiState = coreCfg.tdiIdleState;
    bool emuWithTck = coreCfg.codeForEmuWithTck;
    uint32_t tdiPluseCount = coreCfg.tdiInsertPluseInIdle;
    if(tdiState)
    {
        if(emu)
        {
            if(emuWithTck)
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("101\n");
            for(j=jtagFreqMult;j>0;j--)
                outFile->write("111\n");
            if(emuWithTck)
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("001\n");
            for(j=jtagFreqMult;j>0;j--)
                outFile->write("011\n");
        }
        else
        {
            if(jtagFreqMult == 1)
                outFile->write("0xDF,0xCE,");
            else
            {
                int count = (jtagFreqMult+1)/2;
                for(j=count;j>0;j--)
                    outFile->write("0xDD,");
                for(j=count;j>0;j--)
                    outFile->write("0xFF,");
                for(j=count;j>0;j--)
                    outFile->write("0xCC,");
                for(j=count;j>0;j--)
                    outFile->write("0xEE,");
            }
        }
    }
    else//tdi 0
    {
        if(emu)
        {
            if(emuWithTck)
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("100\n");
            for(j=jtagFreqMult;j>0;j--)
                outFile->write("110\n");
            if(emuWithTck)
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("000\n");
            for(j=jtagFreqMult;j>0;j--)
                outFile->write("010\n");
        }
        else
        {
            if(jtagFreqMult == 1)
                //                outFile->write("0xDF,0xCE,\n");
                outFile->write("0x9B,0x8A,");
            else
            {
                int count = (jtagFreqMult+1)/2;
                for(j=count;j>0;j--)
                    outFile->write("0x99,");
                for(j=count;j>0;j--)
                    outFile->write("0xBB,");
                for(j=count;j>0;j--)
                    outFile->write("0x88,");
                for(j=count;j>0;j--)
                    outFile->write("0xAA,");
            }
        }
    }

    //    for(j=0;j<tdiPluseCount;j++)//插入TDI pluse
    if(tdiPluseCount)
    {
        for(uint32_t i=0;i<tdiPluseCount;i++)
        {
            if(tdiState)
            {
                if(emu)
                {
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("010\n");
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("011\n");
                }
                else
                {
                    if(jtagFreqMult == 1)
                        outFile->write("0xAE,");
                    else
                    {
                        int count = (jtagFreqMult+1)/2;
                        for(j=count;j>0;j--)
                            outFile->write("0xAA,");
                        for(j=count;j>0;j--)
                            outFile->write("0xEE,");
                    }
                }
            }
            else//tdi默认为低
            {
                if(emu)
                {
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("011\n");
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("010\n");
                }
                else
                {
                    if(jtagFreqMult == 1)
                        outFile->write("0xEA,");
                    else
                    {
                        int count = (jtagFreqMult+1)/2;
                        for(j=count;j>0;j--)
                            outFile->write("0xEE,");
                        for(j=count;j>0;j--)
                            outFile->write("0xAA,");
                    }
                }
            }
        }
    }
    if(!emu)
        outFile->write("\n");

    return;
}
//经过pauseDr退出到run-test-idle状态
void quitStatePauseDr(QFile* outFile,uint32_t jtagFreqMult,bool emu,bool emuWithTck)
{
    uint32_t j;

    if(emu)
    {
        if(jtagFreqMult == 1)
        {
            if(emuWithTck)
                outFile->write("001\n");
            outFile->write("011\n");
            if(emuWithTck)
                outFile->write("101\n");
            outFile->write("111\n");
            if(emuWithTck)
                outFile->write("101\n");
            outFile->write("111\n");
        }
        else {
            if(emuWithTck)
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("001\n");
            for(j=jtagFreqMult;j>0;j--)
                outFile->write("011\n");
            if(emuWithTck)
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("101\n");
            for(j=jtagFreqMult;j>0;j--)
                outFile->write("111\n");
            if(emuWithTck)
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("101\n");
            for(j=jtagFreqMult;j>0;j--)
                outFile->write("111\n");
        }
    }
    else
    {
        if(jtagFreqMult == 1)
            outFile->write("0xCE,0xDF,0xDF,\n");
        else
        {
            int count = (jtagFreqMult+1)/2;
            for(j=count;j>0;j--)
                outFile->write("0xCC,");
            for(j=count;j>0;j--)
                outFile->write("0xEE,");
            for(j=count;j>0;j--)
                outFile->write("0xDD,");
            for(j=count;j>0;j--)
                outFile->write("0xFF,");
            for(j=count;j>0;j--)
                outFile->write("0xDD,");
            for(j=count;j>0;j--)
                outFile->write("0xFF,");
            outFile->write("\n");
        }
    }

    return;
}
//进入SHIFT_DR状态
void enterDrState(coreCfgTemp coreCfg,QFile* outFile,bool emu)
{
    int j;
    uint32_t jtagFreqMult = coreCfg.jtagFreqMult;
    bool emuWithTck = coreCfg.codeForEmuWithTck;
    uint32_t tdiIdleState = coreCfg.tdiIdleState;
    if(tdiIdleState)
    {
        if(emu)
        {
            if(jtagFreqMult == 1)
            {
                if(emuWithTck)
                    outFile->write("101\n");
                outFile->write("111\n");
                if(emuWithTck)
                    outFile->write("001\n");
                outFile->write("011\n");
                if(emuWithTck)
                    outFile->write("001\n");
                outFile->write("011\n");
            }
            else
            {
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("101\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("111\n");
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("001\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("011\n");
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("001\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("011\n");

            }
        }
        else//not emu
        {
            if(jtagFreqMult == 1)
                outFile->write("0xDF,0xCE,0xCE,");
            else
            {
                int count = (jtagFreqMult+1)/2;
                for(j=count;j>0;j--)
                    outFile->write("0xDD,");
                for(j=count;j>0;j--)
                    outFile->write("0xFF,");
                for(j=count;j>0;j--)
                    outFile->write("0xCC,");
                for(j=count;j>0;j--)
                    outFile->write("0xEE,");
                for(j=count;j>0;j--)
                    outFile->write("0xCC,");
                for(j=count;j>0;j--)
                    outFile->write("0xEE,");
                outFile->write("\n");
            }
        }
    }
    else//tdi idle is 0
    {
        if(emu)
        {
            if(jtagFreqMult == 1)
            {
                if(emuWithTck)
                    outFile->write("100\n");
                outFile->write("110\n");
                if(emuWithTck)
                    outFile->write("000\n");
                outFile->write("010\n");
                if(emuWithTck)
                    outFile->write("000\n");
                outFile->write("010\n");
            }
            else
            {
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("100\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("110\n");
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("000\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("010\n");
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("000\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("010\n");
            }
        }
        else
        {
            if(jtagFreqMult == 1)
                //                outFile->write("0xDF,0xCE,0xCE,");
                outFile->write("0x9B,0x8A,0x8A,");
            else
            {
                int count = (jtagFreqMult+1)/2;
                for(j=count;j>0;j--)
                    outFile->write("0x99,");
                for(j=count;j>0;j--)
                    outFile->write("0xBB,");
                for(j=count;j>0;j--)
                    outFile->write("0x88,");
                for(j=count;j>0;j--)
                    outFile->write("0xAA,");
                for(j=count;j>0;j--)
                    outFile->write("0x88,");
                for(j=count;j>0;j--)
                    outFile->write("0xAA,");
                outFile->write("\n");
            }
        }
    }

    return;
}
uint64_t cmdChangeEndian(uint64_t cmdValue,int cmdLength)
{
    uint64_t temp = 0;
    for(int i=0;i<cmdLength;i++)
    {
        temp += ((cmdValue & 0x1) << (cmdLength-i-1));
        cmdValue >>= 1;
    }
    return temp;
}


void longCmdToBin(QString cmdValue,Uint32 cmdLength, QFile* outFile,bool emu,coreCfgTemp coreCfg)
{
    QString temp;
    uint32_t jtagFreqMult = coreCfg.jtagFreqMult;
    bool emuWithTck = coreCfg.codeForEmuWithTck;

    uint64_t cmdValueTemp=0,cmdLengthTemp=0,i=0,j,k;
    for (k = 0;k<cmdLength;k += cmdLengthTemp)
    {
        cmdLengthTemp = cmdValue.length();
        if(cmdLengthTemp>8)
        {
            cmdLengthTemp = 64;
            temp = cmdValue.right(8);cmdValue.chop(8);
        }
        else
        {
            temp = cmdValue.right(cmdLengthTemp);cmdValue.chop(cmdLengthTemp);
            cmdLengthTemp = cmdLengthTemp*8-1;
        }

        cmdValueTemp = temp.toULongLong();
        if(emu)
        {
            for(i = 0;i < cmdLengthTemp;i++)
                if((cmdValueTemp>>i)&0x01)
                {
                    if(emuWithTck)
                        for(j=jtagFreqMult;j>0;j--)
                            outFile->write("001\n");
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("011\n");//TDI 1
                }
                else
                {
                    if(emuWithTck)
                        for(j=jtagFreqMult;j>0;j--)
                            outFile->write("000\n");
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("010\n");//TDI 0
                }
        }
        else
        {
            if(jtagFreqMult == 1)//base frequence
                for(i = 0;i < cmdLengthTemp;i++)
                    if((cmdValueTemp>>i)&0x01)
                        outFile->write("0xCE,");//TDI 1
                    else
                        outFile->write("0x8A,");//TDI 1
            else//ferquence need to be multipled
            {
                int count = (jtagFreqMult+1)/2;
                for(i = 0;i < cmdLengthTemp;i++)
                    if((cmdValueTemp>>i)&0x01)
                    {
                        for(j=count;j>0;j--)
                            outFile->write("0xCC,");//TDI 1
                        for(j=count;j>0;j--)
                            outFile->write("0xEE,");//TDI 1
                    }
                    else
                    {
                        for(j=count;j>0;j--)
                            outFile->write("0x88,");//TDI 1
                        for(j=count;j>0;j--)
                            outFile->write("0xAA,");//TDI 1
                    }
            }
        }
    }
    if(emu)
    {
        if((cmdValueTemp>>i)&0x01)
        {
            if(emuWithTck)
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("101\n");
            for(j=jtagFreqMult;j>0;j--)
                outFile->write("111\n");//TDI 1
        }
        else
        {
            if(emuWithTck)
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("100\n");
            for(j=jtagFreqMult;j>0;j--)
                outFile->write("110\n");//TDI 0
        }
    }
    else
    {
        if(jtagFreqMult == 1)//base frequence
        {
            if((cmdValueTemp>>i)&0x01)
                outFile->write("0xDF,");//TDI 1
            else
                outFile->write("0x9B,");//TDI 1
        }
        else//ferquence need to be multipled
        {
            int count = (jtagFreqMult+1)/2;
            if((cmdValueTemp>>i)&0x01)
            {
                for(j=count;j>0;j--)
                    outFile->write("0xDD,");//TDI 1
                for(j=count;j>0;j--)
                    outFile->write("0xFF,");//TDI 1
            }
            else
            {
                for(j=count;j>0;j--)
                    outFile->write("0x99,");//TDI 1
                for(j=count;j>0;j--)
                    outFile->write("0xBB,");//TDI 1
            }
        }
    }
}
//将jtag指令以二进制表示写入到文件中,emu参数用于确定是生成407工程还是仿真码
//写入到407时，指令格式以4bit：TRST,TDI,TCK,TMS，每byte中存在两条指令
void jtagCmdListToBin(JtagList* localCmdList,LongCmdList* longCmdList,QFile* outFile,cfgTemp* localCfg)
{
    JtagTemp *jtagTemp;
    Uint32 listLength = localCmdList->length;
    Uint32 cmdLength,jtagFreqMult;
    Uint64 cmdValue;
    Uint32 i,j;
    bool emu = (localCfg->codeFormate == EMU_CODE);
    bool emuWithTck = localCfg->coreCfg[localCfg->currentCore].codeForEmuWithTck;
    bool withIcePick = localCfg->coreCfg[localCfg->currentCore].withIcepick;
    uint8_t irEndian = localCfg->coreCfg[localCfg->currentCore].irEndian;
    uint8_t drEndian = localCfg->coreCfg[localCfg->currentCore].drEndian;
    Uint32 currentTap = localCfg->coreCfg[localCfg->currentCore].tapNum;

    while(listLength)
    {
        jtagTemp = localCmdList->current;
        cmdLength = jtagTemp->getBitCount();
        cmdValue = jtagTemp->getValue();
        jtagFreqMult = localCfg->coreCfg[localCfg->currentCore].jtagFreqMult;

        if(withIcePick&&((jtagTemp->getType()==IR)||(jtagTemp->getType()==DR))&&(jtagTemp->getTap() == 1))//切换tap
        {
            cmdValue = (cmdValue & ~((uint64_t)0xF<<(cmdLength-8))) | (uint64_t)(currentTap<<(cmdLength-8));
        }

        switch(jtagTemp->getType())
        {
        case DR:
            if(localCfg->coreCfg[localCfg->currentCore].idleWithTck)
                jtagGenerateClock(localCfg->coreCfg[localCfg->currentCore],localCfg->coreCfg[localCfg->currentCore].jtagEmptyClockBetweenCmd,outFile,emu);
            else
                jtagGenerateDelay(localCfg->coreCfg[localCfg->currentCore],localCfg->coreCfg[localCfg->currentCore].jtagEmptyClockBetweenCmd,outFile,emu);
            if(drEndian == BIG_ENDIAN)
                cmdValue = cmdChangeEndian(cmdValue,cmdLength);//大小端切换
            enterDrState(localCfg->coreCfg[localCfg->currentCore],outFile,emu);
            break;
        case IR:
            if(localCfg->coreCfg[localCfg->currentCore].idleWithTck)
                jtagGenerateClock(localCfg->coreCfg[localCfg->currentCore],localCfg->coreCfg[localCfg->currentCore].jtagEmptyClockBetweenCmd,outFile,emu);
            else
                jtagGenerateDelay(localCfg->coreCfg[localCfg->currentCore],localCfg->coreCfg[localCfg->currentCore].jtagEmptyClockBetweenCmd,outFile,emu);
            if(irEndian == BIG_ENDIAN)
                cmdValue = cmdChangeEndian(cmdValue,cmdLength);//大小端切换
            enterIrState(localCfg->coreCfg[localCfg->currentCore],outFile,emu);
            break;
        case DELAY:
            jtagGenerateDelay(localCfg->coreCfg[localCfg->currentCore],cmdValue,outFile,emu);
            listLength--;
            localCmdList->current = localCmdList->current->getNext();
            continue;
        case CLOCK:
            jtagGenerateClock(localCfg->coreCfg[localCfg->currentCore],cmdValue,outFile,emu);
            listLength--;
            localCmdList->current = localCmdList->current->getNext();
            continue;
        case SET_EMPTY_CLOCK:
            localCfg->coreCfg[localCfg->currentCore].jtagEmptyClockBetweenCmd = cmdValue;
            listLength--;
            localCmdList->current = localCmdList->current->getNext();
            continue;
        case SET_FREQ_MULT://change cmds frequence multipier
            localCfg->coreCfg[localCfg->currentCore].jtagFreqMult = cmdValue;
            listLength--;
            localCmdList->current = localCmdList->current->getNext();
            continue;
        case SET_IDLE_WITH_TCK:
            localCfg->coreCfg[localCfg->currentCore].idleWithTck = cmdValue;
            listLength--;
            localCmdList->current = localCmdList->current->getNext();
            continue;
        case SET_TDI_VALUE://设置TDI在edle的值
            if(cmdValue)
                localCfg->coreCfg[localCfg->currentCore].tdiIdleState = 1;
            else
                localCfg->coreCfg[localCfg->currentCore].tdiIdleState = 0;
            listLength--;
            localCmdList->current = localCmdList->current->getNext();
            continue;
        case SET_TDI_PLUSE:
            localCfg->coreCfg[localCfg->currentCore].tdiInsertPluseInIdle = cmdValue;
            listLength--;
            localCmdList->current = localCmdList->current->getNext();
            continue;
        }


        //处理超长指令
        if(cmdLength>64)
        {
            //            longCmdToBin(longCmdList->current->cmdValue,cmdLength,outFile,jtagFreqMult,emu,emuWithTck);
            longCmdToBin(longCmdList->current->cmdValue,cmdLength,outFile,emu,localCfg->coreCfg[localCfg->currentCore]);
            longCmdList->current = longCmdList->current->next;
            goto longCmdFinish;
        }

        if(emu)//TMS\TCK\TDI
        {
            for(i = 0;i < (cmdLength-1);i++)
            {
                if((cmdValue>>i)&0x01)
                {
                    if(emuWithTck)
                        for(j=jtagFreqMult;j>0;j--)
                            outFile->write("001\n");
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("011\n");//TDI 1
                }
                else
                {
                    if(emuWithTck)
                        for(j=jtagFreqMult;j>0;j--)
                            outFile->write("000\n");
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("010\n");//TDI 0
                }
            }
            if((cmdValue>>i)&0x01)
            {
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("101\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("111\n");//TDI 1
            }
            else
            {
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("100\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("110\n");//TDI 0
            }
        }
        else//TRST/TDI/TCK/TMS
        {
            if(jtagFreqMult == 1)//base frequence
            {
                for(i = 0;i < (cmdLength-1);i++)
                {
                    if((cmdValue>>i)&0x01)
                        outFile->write("0xCE,");//TDI 1
                    else
                        outFile->write("0x8A,");//TDI 1
                }
                if((cmdValue>>i)&0x01)
                    outFile->write("0xDF,");//TDI 1
                else
                    outFile->write("0x9B,");//TDI 1
            }
            else//ferquence need to be multipled
            {
                int count = (jtagFreqMult+1)/2;
                for(i = 0;i < (cmdLength-1);i++)
                {
                    if((cmdValue>>i)&0x01)
                    {
                        for(j=count;j>0;j--)
                            outFile->write("0xCC,");//TDI 1
                        for(j=count;j>0;j--)
                            outFile->write("0xEE,");//TDI 1
                    }
                    else
                    {
                        for(j=count;j>0;j--)
                            outFile->write("0x88,");//TDI 1
                        for(j=count;j>0;j--)
                            outFile->write("0xAA,");//TDI 1
                    }
                }
                if((cmdValue>>i)&0x01)
                {
                    for(j=count;j>0;j--)
                        outFile->write("0xDD,");//TDI 1
                    for(j=count;j>0;j--)
                        outFile->write("0xFF,");//TDI 1
                }
                else
                {
                    for(j=count;j>0;j--)
                        outFile->write("0x99,");//TDI 1
                    for(j=count;j>0;j--)
                        outFile->write("0xBB,");//TDI 1
                }
            }
        }
longCmdFinish:
        quitState(localCfg->coreCfg[localCfg->currentCore],outFile,emu);

        listLength--;
        localCmdList->current = localCmdList->current->getNext();
    }
    outFile->flush();
    longCmdList->current = longCmdList->head;
    localCmdList->current = localCmdList->head;
}

void jtagCmdToBin(Uint64 cmdValue,QFile* outFile,bool type,Uint32 cmdLength,cfgTemp localCfg)
{
    Uint32 i,j;
    uint32_t jtagFreqMult = localCfg.coreCfg[localCfg.currentCore].jtagFreqMult;
    bool emu = (localCfg.codeFormate==EMU_CODE);
    bool emuWithTck = localCfg.coreCfg[localCfg.currentCore].codeForEmuWithTck;
    if(type)
    {
        if(localCfg.coreCfg[localCfg.currentCore].idleWithTck)
            jtagGenerateClock(localCfg.coreCfg[localCfg.currentCore],localCfg.coreCfg[localCfg.currentCore].jtagEmptyClockBetweenCmd,outFile,emu);
        else
            jtagGenerateDelay(localCfg.coreCfg[localCfg.currentCore],localCfg.coreCfg[localCfg.currentCore].jtagEmptyClockBetweenCmd,outFile,emu);
        if(localCfg.coreCfg[localCfg.currentCore].irEndian == BIG_ENDIAN)
            cmdValue = cmdChangeEndian(cmdValue,cmdLength);//大小端切换
        enterIrState(localCfg.coreCfg[localCfg.currentCore],outFile,emu);
    }
    else//DR
    {
        if(localCfg.coreCfg[localCfg.currentCore].idleWithTck)
            jtagGenerateClock(localCfg.coreCfg[localCfg.currentCore],localCfg.coreCfg[localCfg.currentCore].jtagEmptyClockBetweenCmd,outFile,emu);
        else
            jtagGenerateDelay(localCfg.coreCfg[localCfg.currentCore],localCfg.coreCfg[localCfg.currentCore].jtagEmptyClockBetweenCmd,outFile,emu);
        if(localCfg.coreCfg[localCfg.currentCore].drEndian == BIG_ENDIAN)
            cmdValue = cmdChangeEndian(cmdValue,cmdLength);//大小端切换
        enterDrState(localCfg.coreCfg[localCfg.currentCore],outFile,emu);
    }
    if(emu)
    {
        for(i = 0;i < (cmdLength-1);i++)
        {
            if((cmdValue>>i)&0x01)
            {
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("001\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("011\n");//TDI 1
            }
            else
            {
                if(emuWithTck)
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("000\n");
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("010\n");//TDI 0
            }
        }
        if((cmdValue>>i)&0x01)
        {
            if(emuWithTck)
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("101\n");
            for(j=jtagFreqMult;j>0;j--)
                outFile->write("111\n");//TDI 1
        }
        else
        {
            if(emuWithTck)
                for(j=jtagFreqMult;j>0;j--)
                    outFile->write("100\n");
            for(j=jtagFreqMult;j>0;j--)
                outFile->write("110\n");//TDI 0
        }
    }
    else
    {
        if(jtagFreqMult == 1)//base frequence
        {
            for(i = 0;i < (cmdLength-1);i++)
            {
                if((cmdValue>>i)&0x01)
                    outFile->write("0xCE,");//TDI 1
                else
                    outFile->write("0x8A,");//TDI 1
            }
            if((cmdValue>>i)&0x01)
                outFile->write("0xDF,");//TDI 1
            else
                outFile->write("0x9B,");//TDI 1
        }
        else//ferquence need to be multipled
        {
            int count = (jtagFreqMult+1)/2;
            for(i = 0;i < (cmdLength-1);i++)
            {
                if((cmdValue>>i)&0x01)
                {
                    for(j=count;j>0;j--)
                        outFile->write("0xCC,");//TDI 1
                    for(j=count;j>0;j--)
                        outFile->write("0xEE,");//TDI 1
                }
                else
                {
                    for(j=count;j>0;j--)
                        outFile->write("0x88,");//TDI 1
                    for(j=count;j>0;j--)
                        outFile->write("0xAA,");//TDI 1
                }
            }
            if((cmdValue>>i)&0x01)
            {
                for(j=count;j>0;j--)
                    outFile->write("0xDD,");//TDI 1
                for(j=count;j>0;j--)
                    outFile->write("0xFF,");//TDI 1
            }
            else
            {
                for(j=count;j>0;j--)
                    outFile->write("0x99,");//TDI 1
                for(j=count;j>0;j--)
                    outFile->write("0xBB,");//TDI 1
            }
        }
    }
    quitState(localCfg.coreCfg[localCfg.currentCore],outFile,emu);
}

void delJtagList(JtagList* localJtagList)
{
    if(localJtagList == nullptr)
        return;
    Uint32 i;
    Uint32 listLength = localJtagList->length;
    JtagTemp* jtagTemp;
    localJtagList->current = localJtagList->head;
    for(i = 0;i < listLength;i++)
    {
        jtagTemp = localJtagList->current;
        localJtagList->current = localJtagList->current->getNext();
        delete jtagTemp;
    }
    delete localJtagList;
    localJtagList = nullptr;
}
int outFileGenerate(QFile *outFile,QFile *outTempFile,QFile *outAddrTempFile,uint64_t pcAddr,cfgTemp localCfg)
{
    QFile headFile;
    if(localCfg.codeFormate == ATE_CODE)
    {
        headFile.setFileName(localCfg.coreCfg[localCfg.currentCore].coreLibPath+localCfg.coreCfg[localCfg.currentCore].ateHeadFileName);
        if(!headFile.open(QIODevice::ReadOnly|QIODevice::Text)){
            return -1;}
    }
    else if(localCfg.codeFormate == EMU_CODE)
    {
        headFile.setFileName(localCfg.coreCfg[localCfg.currentCore].coreLibPath+localCfg.coreCfg[localCfg.currentCore].emuHeadFileName);
        if(!headFile.open(QIODevice::ReadOnly|QIODevice::Text)){
            return -1;}
    }
    else if(localCfg.codeFormate == TEST_CODE)
    {
        headFile.setFileName(localCfg.coreCfg[localCfg.currentCore].coreLibPath+localCfg.coreCfg[localCfg.currentCore].testHeadFileName);
        if(!headFile.open(QIODevice::ReadOnly|QIODevice::Text)){
            return -1;}
    }
    QTextStream headIn(&headFile);//头文件
    QTextStream out(outFile);//最终的目标文件
    QString line,temp;
    while(headIn.readLineInto(&line))
    {
        if(line.contains("#DATA"))
        {
            outTempFile->seek(0);
            QTextStream dataIn(outTempFile);//存储数据的临时文件
            while(dataIn.readLineInto(&temp))
                out<<temp<<"\n";
        }
        else if(line.contains("#ADDR_TABLE"))//地址表替换
        {
            outAddrTempFile->seek(0);
            QTextStream addrIn(outAddrTempFile);//存储数据的临时文件
            while(addrIn.readLineInto(&temp))
                out<<temp<<"\n";
        }
        else if(line.contains("#PC_ADDR"))//PC地址替换
        {
            QString strTemp = QString("0x%1").arg(pcAddr,8,16,QLatin1Char('0'));
            line.replace("#PC_ADDR",strTemp);
            out<<line<<"\n";
        }
        else
            out<<line<<"\n";
    }

    headFile.close();
    outFile->flush();
    return 0;
}

void changeTap(JtagList* localCmdList,Uint32 tapNum)
{
    uint64_t localValue;
    uint32_t localBitCount;
    while(localCmdList->current)
    {
        localBitCount = localCmdList->current->getBitCount();
        //        if((localBitCount == 32)||(localBitCount == 33))
        if(localCmdList->current->getTap()&&(localCmdList->current->getType() <= IR))
        {
            localValue = localCmdList->current->getValue();
            localValue = (localValue & ~((uint64_t)0xF<<(localBitCount-8))) | (uint64_t)(tapNum<<(localBitCount-8));
            localCmdList->current->setValue(localValue);
        }
        localCmdList->current = localCmdList->current->getNext();
    }
    localCmdList->current = localCmdList->head;
}
//将命令链表的指针向后移动指定长度
void changeCmdListPointer(JtagList* cmdList ,int num)
{
    for(int i = 0;i<num;i++)
    {
        cmdList->current = cmdList->current->getNext();
    }
}
/**********************************
 * 函数功能：在cmdList中寻找与templet相同的指令，并用changeValue替换templet
 * ********************************/
Uint32 changeCmdListValue(JtagList* localCmdList,LongCmdList* longCmdList,QString templet,Uint32 changeValue,Uint32 replaceBitCount,cfgTemp localCfg)
{
    //    QString cmdValueTemp;
    Uint32 cmdLength = 0;
    Uint32 listLength = localCmdList->length;
    int i,temp;
    uint32_t leftData=0,flowData=0,flowMask=0;
    bool ok;

    if(localCfg.coreCfg[localCfg.currentCore].addrLeftShiftBitCount)//地址移位操作
    {
        temp = localCfg.coreCfg[localCfg.currentCore].addrLeftShiftBitCount%4;//获取移位的bit数
        for(i=0;i<(4-temp);i++)
            leftData += changeValue&((uint32_t)1<<i);//获取移位低位的数据
        for(i=0;i<temp;i++)
            flowMask += (uint32_t)1<<(replaceBitCount-1-i);//获取高位溢出数据
        flowData = (changeValue & flowMask)>>(replaceBitCount-temp);
        changeValue &= (~flowMask);//干掉溢出数据
        changeValue>>=(4-temp);//干掉低位数据
    }
    QString change = QString::number(changeValue,16);

    //对字符串进行补齐操作
    if(templet.contains("0x",Qt::CaseInsensitive))
        templet.remove("0x",Qt::CaseInsensitive);
    if(templet.length()>change.length())
    {
        temp = templet.length()-change.length();
        for(i =0;i<temp;i++)
            change.prepend('0');
    }
    else if(templet.length()<change.length())
    {
        temp = change.length() - templet.length();
        for(i =0;i<temp;i++)
            templet.prepend('0');
    }

    localCmdList->current = localCmdList->head;
    while(listLength)
    {
        cmdLength = localCmdList->current->getBitCount();
        if((cmdLength>64)&&(longCmdList->current->cmdValue.contains(templet,Qt::CaseInsensitive)))
        {
            longCmdList->current->cmdValue.replace(templet,change,Qt::CaseInsensitive);
            break;
        }

        //        cmdValueTemp = QString::number(localCmdList->current->getValue(),16);
        QString cmdValueTemp = QString("%1").arg(localCmdList->current->getValue(),localCmdList->current->getBitCount()/4,16,QLatin1Char('0'));
        if(cmdValueTemp.contains(templet,Qt::CaseInsensitive))
        {
            int localRef = 0;
            if(localCfg.coreCfg[localCfg.currentCore].addrLeftShiftBitCount)//存在地址移位操作
            {
                localRef = cmdValueTemp.indexOf(templet,0,Qt::CaseInsensitive);//获取标志高位所在的索引值，从高字节开始的
                localRef += templet.length();//获取标志低位所在的索引值，从高字节开始的
                localRef = cmdValueTemp.length() - localRef;//指向标志字符串最低位所在
            }

            cmdValueTemp.replace(templet,change,Qt::CaseInsensitive);
            uint64_t cmdTemp = cmdValueTemp.toULongLong(&ok,16);
            if(localCfg.coreCfg[localCfg.currentCore].addrLeftShiftBitCount)//存在地址移位操作
            {
                cmdTemp = cmdTemp + ((leftData<<((localRef-1)*4))<<(localCfg.coreCfg[localCfg.currentCore].addrLeftShiftBitCount%4)) + ((uint64_t)flowData<<((localRef-1)*4+replaceBitCount));
            }
            localCmdList->current->setValue(cmdTemp);
            break;
        }
        listLength--;
        localCmdList->current = localCmdList->current->getNext();
    }
    localCmdList->current = localCmdList->head;
    longCmdList->current = longCmdList->head;
    return cmdLength;
}
//void jtagGenerateClock(Uint64 time,QFile* outFile,bool tdiState,uint32_t jtagFreqMult, bool emu,bool emuWithTck)
void jtagGenerateClock(coreCfgTemp coreCfg,Uint64 time,QFile* outFile, bool emu)
{
    int j;
    int tdiState = coreCfg.tdiIdleState;
    int jtagFreqMult = coreCfg.jtagFreqMult;
    bool emuWithTck = coreCfg.codeForEmuWithTck;
    if(time == 0)
        return;
    if(tdiState)
    {
        if(emu)
        {
            if(jtagFreqMult == 1)
                for(Uint64 i = 0;i<time;i++)
                {
                    if(emuWithTck)
                        outFile->write("001\n");
                    outFile->write("011\n");
                }
            else
            {
                for(Uint64 i = 0;i<time;i++)
                {
                    if(emuWithTck)
                        for(j=jtagFreqMult;j>0;j--)
                            outFile->write("001\n");
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("011\n");
                }
            }
        }
        else
        {
            if(jtagFreqMult == 1)
                for(Uint64 i = 0;i<time;i++)
                    outFile->write("0xCE,");
            else
            {
                int count = (jtagFreqMult+1)/2;
                for(Uint64 i = 0;i<time;i++)
                {
                    for(j=count;j>0;j--)
                        outFile->write("0xCC,");
                    for(j=count;j>0;j--)
                        outFile->write("0xEE,");
                }
            }
        }
    }
    else {
        if(emu)
        {
            if(jtagFreqMult == 1)
                for(Uint64 i = 0;i<time;i++)
                {
                    if(emuWithTck)
                        outFile->write("000\n");
                    outFile->write("010\n");
                }
            else
            {
                for(Uint64 i = 0;i<time;i++)
                {
                    if(emuWithTck)
                        for(j=jtagFreqMult;j>0;j--)
                            outFile->write("000\n");
                    for(j=jtagFreqMult;j>0;j--)
                        outFile->write("010\n");
                }
            }
        }
        else
        {
            if(jtagFreqMult == 1)
                for(Uint64 i = 0;i<time;i++)
                    outFile->write("0x8A,");
            else
            {
                int count = (jtagFreqMult+1)/2;
                for(Uint64 i = 0;i<time;i++)
                {
                    for(j=count;j>0;j--)
                        outFile->write("0x88,");
                    for(j=count;j>0;j--)
                        outFile->write("0xAA,");
                }
            }
        }
    }
}
//jtagGenerateDelay(localCfg->coreCfg[localCfg->currentCore].jtagEmptyClockBetweenCmd,outFile,true,jtagFreqMult,emu,emuWithTck);
void jtagGenerateDelay(coreCfgTemp coreCfg,Uint64 time,QFile* outFile, bool emu)
{
    int j;
    int tdiState = coreCfg.tdiIdleState;
    int jtagFreqMult = coreCfg.jtagFreqMult;
    bool emuWithTck = coreCfg.codeForEmuWithTck;
    if(tdiState)
    {
        if(emu)
            for(j=jtagFreqMult;j>0;j--)
                for(Uint64 i = 0;i<time;i++)
                {
                    if(emuWithTck)
                        outFile->write("001\n");
                    outFile->write("001\n");
                }
        else
            for(j=jtagFreqMult;j>0;j--)
                for(Uint64 i = 0;i<time;i++)
                    outFile->write("0xCC,");
    }
    else
    {
        if(emu)
            for(j=jtagFreqMult;j>0;j--)
                for(Uint64 i = 0;i<time;i++)
                {
                    if(emuWithTck)
                        outFile->write("000\n");
                    outFile->write("000\n");
                }
        else
            for(j=jtagFreqMult;j>0;j--)
                for(Uint64 i = 0;i<time;i++)
                    outFile->write("0x88,");
    }
}
