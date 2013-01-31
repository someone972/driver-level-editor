#include "INIParser.hpp"

int skipws(char* buffer,int pos,int length)
{
    if(pos < length && buffer[pos] == ' ')
    {
        do
        {
            pos++;
        }
        while(buffer[pos] == ' ' && pos < length);
    }
    return pos;
};

int reverse_skipws(char* buffer,int pos,int smallest)
{
    if(pos > smallest && buffer[pos] == ' ')
    {
        do
        {
            pos--;
        }
        while(buffer[pos] == ' ' && pos > smallest);
    }
    return pos;
};

int nextline(char* buffer,int pos,int length)
{
    while(buffer[pos] != '\n' && buffer[pos] != '\r' && pos < length)
    {
        pos++;
    }
    while((buffer[pos] == '\n' || buffer[pos] == '\r') && pos < length)
    {
        pos++;
    }
    return pos;
};

INIKey::INIKey()
{
    namesize = 0;
    name = NULL;
    commentsize = 0;
    comment = NULL;
    datasize = 0;
    data = NULL;
};

INIKey::~INIKey()
{
    if(name)
    {
        free(name);
        name = NULL;
        namesize = 0;
    }
    if(comment)
    {
        free(comment);
        comment = NULL;
        commentsize = 0;
    }
    if(data)
    {
        free(data);
        data = NULL;
        datasize = 0;
    }
};

const char* INIKey::get_name()
{
    return name;
};

const char* INIKey::get_comment()
{
    return comment;
};

int INIKey::get_int(int def,int base)
{
    char* lastchar;
    long int ret = strtol(data,&lastchar,base);
    if(data == lastchar)
    return def;
    return ret;
};

const char* INIKey::get_string(const char* def)
{
    if(data == NULL)
    return def;
    return data;
};

bool INIKey::get_bool(bool def)
{
    if(strcmp(data,"true") == 0 || strcmp(data,"1") == 0)
    return true;
    else if(strcmp(data,"false") == 0 || strcmp(data,"0") == 0)
    return false;
    return def;
};

void INIKey::set_name(const char* n)
{
    if(n == NULL)
    {
        if(namesize != 0)
        {
            free(name);
            namesize = 0;
        }
        name = NULL;
        return;
    }
    size_t size = strlen(n)+1;
    if(size > namesize)
    name = (char*)realloc(name,size);
    strcpy(name,n);
};

void INIKey::set_comment(const char* n)
{
    if(n == NULL)
    {
        if(commentsize != 0)
        {
            free(comment);
            commentsize = 0;
        }
        comment = NULL;
        return;
    }
    size_t size = strlen(n)+1;
    if(size > commentsize)
    comment = (char*)realloc(comment,size);
    strcpy(comment,n);
};

void INIKey::set_string(const char* value)
{
    if(value == NULL)
    {
        if(datasize != 0)
        {
            free(data);
            datasize = 0;
        }
        comment = NULL;
        return;
    }
    size_t size = strlen(value)+1;
    if(size > datasize)
    data = (char*)realloc(data,size);
    strcpy(data,value);
};

void INIKey::set_int(int value,int base)
{
    size_t size = sizeof(int)*8+1;
    if(datasize < size)
    {
        datasize = size;
        data = (char*)realloc(data,size);
    }
    itoa(value,data,base);
};

void INIKey::set_bool(bool value,bool astext)
{
    if(datasize < 6)
    {
        datasize = 6;
        data = (char*)realloc(data,6);
    }
    if(astext)
    {
        strcpy(data,(value == true ? "true" : "false"));
    }
    else strcpy(data,(value == true ? "1" : "0"));
};

INISection::INISection()
{
    name = NULL;
    namesize = 0;
    comment = NULL;
    commentsize = 0;
};

INISection::~INISection()
{
    if(name)
    {
        free(name);
        name = NULL;
        namesize = 0;
    }
    if(comment)
    {
        free(comment);
        comment = NULL;
        commentsize = 0;
    }
    for(size_t i = 0; i < keys.size(); i++)
    delete keys[i];
    keys.clear();
};

void INISection::set_name(const char* n)
{
    if(n == NULL)
    {
        if(namesize != 0)
        {
            free(name);
            namesize = 0;
        }
        name = NULL;
        return;
    }
    size_t size = strlen(n)+1;
    if(size > namesize)
    name = (char*)realloc(name,size);
    strcpy(name,n);
};

void INISection::set_comment(const char* n)
{
    if(n == NULL)
    {
        if(commentsize != 0)
        {
            free(comment);
            commentsize = 0;
        }
        comment = NULL;
        return;
    }
    size_t size = strlen(n)+1;
    if(size > commentsize)
    comment = (char*)realloc(comment,size);
    strcpy(comment,n);
};

INIKey* INISection::find_key(const char* key)
{
    for(size_t i = 0; i < keys.size(); i++)
    {
        if(strcmp(keys[i]->get_name(),key) == 0)
        return keys[i];
    }
    return NULL;
};

INIKey* INISection::make_new_key(const char* key)
{
    INIKey* k = new INIKey();
    k->set_name(key);
    keys.push_back(k);
    return k;
};

const char* INISection::get_name()
{
    return name;
};

const char* INISection::get_comment()
{
    return comment;
};

INISection* INI::find_section(const char* name)
{
    for(size_t i = 0; i < sections.size(); i++)
    {
        if(sections[i]->get_name() != NULL)
        {
            if(strcmp(sections[i]->get_name(),name) == 0)
            return sections[i];
        }
        else if(name == NULL)
        return sections[i];
    }
    return NULL;
};

INISection* INI::make_new_section(const char* name)
{
    INISection* section = new INISection();
    section->set_name(name);
    sections.push_back(section);
    return section;
};

size_t INISection::num_keys()
{
    return keys.size();
};

INIKey* INISection::get_key(int index)
{
    if(index >= 0 && index < (int)keys.size())
    return keys[index];
    return NULL;
};

INI::INI()
{

};

INI::~INI()
{
    cleanup();
};

void INI::cleanup()
{
    for(size_t i = 0; i < sections.size(); i++)
    delete sections[i];
};

int INI::load_ini(const char* filename)
{
    FILE* file = fopen(filename,"rb");
    if(!file)
    return -1;
    int ret = load_ini(file);
    fclose(file);
    return ret;
};




int INI::load_ini(FILE* file)
{
    if(!file)
    return -1;

    cleanup();

    size_t start,end;
    size_t pos = 0;

    fseek(file,0,SEEK_END);
    size_t length = ftell(file);
    fseek(file,0,SEEK_SET);

    char* buffer = (char*)malloc(length);
    fread(buffer,length,1,file);

    INISection* current_section = make_new_section(NULL);

    while(pos < length)
    {
        pos = skipws(buffer,pos,length);
        if(buffer[pos] == '[')
        {
            pos++;
            start = pos = skipws(buffer,pos,length);
            while(buffer[pos] != ']' && pos < length-1 && buffer[pos] != '\n' && buffer[pos] != '\r')
            {
                pos++;
            }
            end = reverse_skipws(buffer,pos-1,start)+1;
            if(end-start != 0)
            {
                buffer[end] = '\0';
                current_section = find_section((buffer+start));
                if(current_section == NULL)
                {
                    current_section = make_new_section((buffer+start));
                }
                while(buffer[pos] != ';' && pos < length-1 && buffer[pos] != '\n' && buffer[pos] != '\r')
                {
                    pos++;
                }
                if(buffer[pos] == ';')
                {
                    pos++;
                    start = pos = skipws(buffer,pos,length);
                    while(buffer[pos] != '\n' && buffer[pos] != '\r' && pos < length-1)
                    {
                        pos++;
                    }
                    end = reverse_skipws(buffer,pos-1,start)+1;
                    if(end-start != 0)
                    {
                        buffer[end] = '\0';
                        current_section->set_comment((buffer+start));
                    }
                }
            }
        }
        else if(buffer[pos] != '\n' && buffer[pos] != '\r')
        {
            start = pos;
            while(buffer[pos] != '=' && buffer[pos] != '\n' && buffer[pos] != '\r' && pos < length-1)
            {
                pos++;
            }
            if(buffer[pos] == '=' && pos > start)
            {
                end = reverse_skipws(buffer,pos-1,start);
                buffer[end+1] = '\0';
                INIKey* tempkey = current_section->find_key((buffer+start));
                if(tempkey == NULL)
                {
                    tempkey = current_section->make_new_key((buffer+start));
                }
                pos++;
                start = pos = skipws(buffer,pos,length);
                while(buffer[pos] != '\n' && buffer[pos] != '\r' && buffer[pos] != ';' && pos < length-1)
                {
                    pos++;
                }
                char lastchar = buffer[pos];
                if(pos < length-1)
                end = reverse_skipws(buffer,pos-1,start)+1;
                else end = length;
                if(end-start != 0)
                {
                    buffer[end] = '\0';
                    tempkey->set_string((buffer+start));
                }
                if(lastchar == ';' && pos < length-1)
                {
                    pos++;
                    start = pos = skipws(buffer,pos,length);
                    while(buffer[pos] != '\n' && buffer[pos] != '\r' && pos < length-1)
                    {
                        pos++;
                    }
                    end = reverse_skipws(buffer,pos-1,start)+1;
                    if(end-start != 0)
                    {
                        buffer[end] = '\0';
                        tempkey->set_comment((buffer+start));
                    }
                }
            }

        }
        pos = nextline(buffer,pos,length);
    }

    free(buffer);
/*
    for(int i = 0; i < sections.size(); i++)
    {
        if(sections[i]->get_name() != NULL)
        cout<<"["<<sections[i]->get_name()<<"]"<<endl;
        else cout<<"No Name"<<endl;
        for(int j = 0; j < sections[i]->num_keys(); j++)
        {
            cout<<"  "<<sections[i]->get_key(j)->get_name()<<" "<<sections[i]->get_key(j)->get_string("")<<endl;
        }
    }
    */
    return 0;
};

int INI::save_ini(const char* filename)
{
    FILE* file = fopen(filename,"w");
    if(!file)
    return -1;
    int ret = save_ini(file);
    fclose(file);
    return ret;
};

int INI::save_ini(FILE* file)
{
    if(!file)
    return -1;
    const char* msg = ";This file is generated by the Driver Car Importer\n";
    char section_start = '[';
    char section_end = ']';
    const char* comment = "    ;";
    char newline = '\n';
    const char* equal = " = ";

    fwrite(msg,strlen(msg),1,file);
    for(size_t i = 0; i < sections.size(); i++)
    {
        if(sections[i]->get_name() != NULL)
        {
            fwrite(&section_start,1,1,file);
            fwrite(sections[i]->get_name(),strlen(sections[i]->get_name()),1,file);
            fwrite(&section_end,1,1,file);
            if(sections[i]->get_comment() != NULL)
            {
                fwrite(comment,strlen(comment),1,file);
                fwrite(sections[i]->get_comment(),strlen(sections[i]->get_comment()),1,file);
            }
            fwrite(&newline,1,1,file);
        }
        for(size_t j = 0; j < sections[i]->num_keys(); j++)
        {
            INIKey* keyp = sections[i]->get_key(j);
            fwrite(keyp->get_name(),strlen(keyp->get_name()),1,file);
            fwrite(equal,strlen(equal),1,file);
            fwrite(keyp->get_string(""),strlen(keyp->get_string("")),1,file);
            if(keyp->get_comment() != NULL)
            {
                fwrite(comment,strlen(comment),1,file);
                fwrite(keyp->get_comment(),strlen(keyp->get_comment()),1,file);
            }
            fwrite(&newline,1,1,file);
        }
        fwrite(&newline,1,1,file);
    };

    return 0;
};

int INI::get_int(const char* sec,const char* key,int def,int base)
{
    INISection* sectionp;
    INIKey* keyp;

    sectionp = find_section(sec);
    if(sectionp == NULL || key == NULL)
    return def;

    keyp = sectionp->find_key(key);
    if(keyp == NULL)
    return def;

    return keyp->get_int(def,base);
};

bool INI::get_bool(const char* sec,const char* key,bool def)
{
    INISection* sectionp;
    INIKey* keyp;

    sectionp = find_section(sec);
    if(sectionp == NULL || key == NULL)
    return def;

    keyp = sectionp->find_key(key);
    if(keyp == NULL)
    return def;

    return keyp->get_bool(def);
};

const char* INI::get_string(const char* sec,const char* key,const char* def)
{
    INISection* sectionp;
    INIKey* keyp;

    sectionp = find_section(sec);
    if(sectionp == NULL || key == NULL)
    return def;

    keyp = sectionp->find_key(key);
    if(keyp == NULL)
    return def;

    return keyp->get_string(def);
};

const char* INI::get_comment(const char* sec,const char* key)
{
    INISection* sectionp;
    INIKey* keyp;

    sectionp = find_section(sec);
    if(sectionp == NULL)
    return NULL;

    if(key == NULL)
    return sectionp->get_comment();

    keyp = sectionp->find_key(key);
    if(keyp == NULL)
    return NULL;

    return keyp->get_comment();
};

void INI::set_int(const char* sec,const char* key,int value,int base)
{
    INISection* sectionp;
    INIKey* keyp;

    if(key == NULL)
    return;

    sectionp = find_section(sec);
    if(sectionp == NULL)
    {
        sectionp = make_new_section(sec);
    }
    keyp = sectionp->find_key(key);
    if(keyp == NULL)
    {
        keyp = sectionp->make_new_key(key);
    }
    keyp->set_int(value,base);
};

void INI::set_bool(const char* sec,const char* key, bool value,bool astext)
{
    INISection* sectionp;
    INIKey* keyp;

    if(key == NULL)
    return;

    sectionp = find_section(sec);
    if(sectionp == NULL)
    {
        sectionp = make_new_section(sec);
    }
    keyp = sectionp->find_key(key);
    if(keyp == NULL)
    {
        keyp = sectionp->make_new_key(key);
    }
    keyp->set_bool(value,astext);
};

void INI::set_string(const char* sec,const char* key,const char* value)
{
    INISection* sectionp;
    INIKey* keyp;

    if(key == NULL)
    return;

    sectionp = find_section(sec);
    if(sectionp == NULL)
    {
        sectionp = make_new_section(sec);
    }
    keyp = sectionp->find_key(key);
    if(keyp == NULL)
    {
        keyp = sectionp->make_new_key(key);
    }
    keyp->set_string(value);
};

void INI::set_comment(const char* sec,const char* key,const char* value)
{
    INISection* sectionp;
    INIKey* keyp;

    sectionp = find_section(sec);
    if(sectionp == NULL)
    {
        sectionp = make_new_section(sec);
    }
    if(key == NULL)
    {
        sectionp->set_comment(value);
        return;
    }
    keyp = sectionp->find_key(key);
    if(keyp == NULL)
    {
        keyp = sectionp->make_new_key(key);
    }
    keyp->set_comment(value);
};
