#include "elf.h"
//#include "mainwindow.h"
#include <QFile>
#include <QTextStream>

void elf64_parse_elf(char* m_data,elf_info_st& m_info);
void elf32_parse_elf(char* m_data,elf_info_st& m_info);
void elf64SectionLoad(QFile* outFile,char* m_data,elf_info_st m_info);
void elf32SectionLoad(QFile* outFile,char* m_data,elf_info_st m_info);

int checkElfFormate(QString filePath)
{
    QFile file(filePath);
    QByteArray fileData;

    if (!file.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    fileData = file.readAll();
    file.close();
    char * data = fileData.data();
    if (*((unsigned int*)data) != *((unsigned int*)ELFMAG))
        return 0;
    else
        return 1;
}

int elfToHex(QString filePath,QString definedFilePath,uint64_t *pcStartAddr)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return 1;
    }
    QByteArray fileData;
    char* m_data;
    QFile outFile((definedFilePath+"/out.txt"));
    if(!outFile.open(QIODevice::WriteOnly|QIODevice::Text)){
        return 2;
    }
    outFile.resize(0);

    fileData = file.readAll();
    file.close();
    m_data = new char[fileData.size()];
    memcpy(m_data, fileData.data(), fileData.size());
    elf_info_st m_info;
    m_info.elf_is_x64 = m_data[EI_CLASS] == (char)ELFCLASS64 ? true : false;
    if (m_info.elf_is_x64)
    {
        elf64_parse_elf(m_data,m_info);
    }
    else
    {
        elf32_parse_elf(m_data,m_info);
    }
    *pcStartAddr = m_info.elf_entry_address;
    //开始读取程序段
    if(m_info.elf_is_x64)
    {
        elf64SectionLoad(&outFile,m_data,m_info);
    }
    else
    {
        elf32SectionLoad(&outFile,m_data,m_info);
    }

    outFile.close();
    return 0;
}
uint64_t getPcFromElf(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return 1;
    }
    QByteArray fileData;
    char* m_data;

    fileData = file.readAll();
    file.close();
    m_data = new char[60];
    memcpy(m_data, fileData.data(), 60);
    elf_info_st m_info;
    m_info.elf_is_x64 = m_data[EI_CLASS] == (char)ELFCLASS64 ? true : false;
    if (m_info.elf_is_x64)
    {
        elf64_parse_elf(m_data,m_info);
    }
    else
    {
        elf32_parse_elf(m_data,m_info);
    }
    return m_info.elf_entry_address;
}
void elf64_parse_elf(char* m_data,elf_info_st& m_info)
{
    Elf64_Ehdr *elf_header = (Elf64_Ehdr*)m_data;
    m_info.elf_type = elf_header->e_type;
    m_info.elf_entry_address = elf_header->e_entry;
    m_info.elf_phoff = elf_header->e_phoff;
    m_info.elf_phentsize = elf_header->e_phentsize;
    m_info.elf_phnum = elf_header->e_phnum;
    m_info.elf_shoff = elf_header->e_shoff;
    m_info.elf_shentsize = elf_header->e_shentsize;
    m_info.elf_shnum = elf_header->e_shnum;
    m_info.elf_ehsize = elf_header->e_ehsize;
}
void elf32_parse_elf(char* m_data,elf_info_st& m_info)
{
    Elf32_Ehdr *elf_header = (Elf32_Ehdr*)m_data;
    m_info.elf_type = elf_header->e_type;
    m_info.elf_entry_address = elf_header->e_entry;
    m_info.elf_phoff = elf_header->e_phoff;
    m_info.elf_phentsize = elf_header->e_phentsize;
    m_info.elf_phnum = elf_header->e_phnum;
    m_info.elf_shoff = elf_header->e_shoff;
    m_info.elf_shentsize = elf_header->e_shentsize;
    m_info.elf_shnum = elf_header->e_shnum;
    m_info.elf_ehsize = elf_header->e_ehsize;
}
void elf64SectionLoad(QFile* outFile,char* m_data,elf_info_st m_info)
{
    QTextStream out(outFile);
    for (uint32_t i = 0; i < m_info.elf_phnum; i++)
    {
        Elf64_Phdr* programHeader;
        programHeader = (Elf64_Phdr*)(m_data + m_info.elf_phoff + m_info.elf_phentsize*i);

        QString loadStartAddr = QString::number(programHeader->p_paddr,16);
        out<<"@"<<loadStartAddr<<"\n";
        int size = programHeader->p_memsz;
        if(size == 0)
            continue;
        char *startAddr = m_data + programHeader->p_offset;
        for(int j = 0;j<size;j+=16)
        {
            for(int k=0;k<16;k++)
            {
                QString temp = QString::number(*(startAddr+j+k),16);
                out<<temp<<" ";
            }
            out<<"\n";
        }
    }
}
void elf32SectionLoad(QFile* outFile,char* m_data,elf_info_st m_info)
{
    QTextStream out(outFile);
    for (uint32_t i = 0; i < m_info.elf_phnum; i++)
    {
        int j = 0;
        Elf32_Phdr* programHeader;
        programHeader = (Elf32_Phdr*)(m_data + m_info.elf_phoff + m_info.elf_phentsize*i);

        QString loadStartAddr = QString::number(programHeader->p_paddr,16);
        int size = programHeader->p_filesz;
        if(size == 0)
            continue;
        out<<"@"<<loadStartAddr<<"\n";
        char *startAddr = m_data + programHeader->p_offset;
        if(size%16 != 0)
            size -= 16;
        for(j = 0;j<size;j+=16)
        {
            for(int k=0;k<16;k++)
            {
                QString temp = QString::number(*(unsigned char*)(startAddr+j+k),16);
                if(temp.length()==1)
                    temp.prepend("0");
                out<<temp<<" ";
            }
            out<<"\n";
        }
        for(int k =0;k<size%16;k++)
        {
            QString temp = QString::number(*(unsigned char*)(startAddr+j+k),16);
            if(temp.length()==1)
                temp.prepend("0");
            out<<temp<<" ";
        }
        if(size%16 != 0)
            out<<"\n";
    }
    out<<"q\n";
}
