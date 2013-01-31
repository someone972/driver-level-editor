#ifndef INIPARSER_HPP
#define INIPARSER_HPP
#include <fstream>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

class INIKey
{
    public:
        INIKey();
        ~INIKey();
        void set_int(int value,int base);
        void set_bool(bool value,bool astext);
        void set_string(const char* value);
        void set_comment(const char* value);
        void set_name(const char* n);

        int get_int(int def,int base = 10);
        bool get_bool(bool def);
        const char* get_string(const char* def);
        const char* get_comment();
        const char* get_name();

    protected:
        size_t namesize;
        size_t commentsize;
        size_t datasize;
        char* name;
        char* comment;
        char* data;
};

class INISection
{
    public:
        INISection();
        ~INISection();
        void set_name(const char* n);
        void set_comment(const char* value);
        const char* get_name();
        const char* get_comment();

        INIKey* find_key(const char* name);
        INIKey* make_new_key(const char* name);

        void write_to_file(FILE* file);
        size_t num_keys();
        INIKey* get_key(int index);

    protected:
        size_t namesize;
        size_t commentsize;
        char* name;
        char* comment;
        vector<INIKey*> keys;
};

class INI
{
    public:
        INI();
        ~INI();
        int load_ini(const char* filename);
        int load_ini(FILE* file);
        int save_ini(const char* filename);
        int save_ini(FILE* file);

        int get_int(const char* sec,const char* key,int def,int base = 10);
        bool get_bool(const char* sec,const char* key,bool def);
        const char* get_string(const char* sec,const char* key,const char* def);
        const char* get_comment(const char* sec,const char* key);

        void set_int(const char* sec,const char* key,int value,int base = 10);
        void set_bool(const char* sec,const char* key, bool value,bool astext = true);
        void set_string(const char* sec,const char* key,const char* value);
        void set_comment(const char* sec,const char* key,const char* value);

        void cleanup();

    protected:

        INISection* find_section(const char* name);
        INISection* make_new_section(const char* name);

        vector<INISection*> sections;
};

#endif
