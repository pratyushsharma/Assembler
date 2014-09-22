//
//  main.cpp
//  hash_map
//
//  Created by Pratyush Sharma on 02/09/14.
//  Copyright (c) 2014 Pratyush Sharma. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <regex>

using namespace std;

const int TABLE_SIZE = 10000;

struct key_hash
{
    unsigned long operator()(const string & key) const
    {
        unsigned long sum = 0;
        for(int i = 0; i < key.length(); i++)
        {
            sum += key[i];
        }
        return sum % TABLE_SIZE;
    }
    unsigned long operator()(const pair <string, pair<string, string> > & key)
    {
        unsigned long sum = 0;
        for(int i = 0; i < (key.first).length(); i++)
        {
            sum += key.first[i];
        }
        return sum % TABLE_SIZE;
    }

    
};




template <typename k, typename v>
class hash_node {
public:
    hash_node(k key, v value):
    key(key), value(value), next(NULL){
        
    }
    
    k get_key()
    {
        return key;
    }
    
    v get_value()
    {
        return value;
    }
    
    void set_value(v value)
    {
        hash_node::value = value;
    }
    
    void set_next(hash_node *n)
    {
        next =n;
    }
    
    hash_node *get_next()
    {
        return next;
    }
    
private:
    k key;
    v value;
    hash_node *next;
    
};


template<typename k, typename v, typename F = key_hash >
class hash_map
{
private:
    hash_node<k, v> **hash_table;
    F hash_function;
    
public:
    
    hash_map()
    {
        hash_table = new hash_node<k, v> *[TABLE_SIZE];
    }
    
    bool get(k key, v &value)
    {
        unsigned long hash = hash_function(key);
        hash_node<k, v> *present = hash_table[hash];
        while (present) {
            if (present->get_key() == key)
            {
                value = present->get_value();
                return true;
            }
            else
                present = present->get_next();
        }
        return false;
    }
    
    void put(k key, v value)
    {
        hash_node<k,v> *new_node = new hash_node<k, v>(key, value);
        unsigned long hash = hash_function(key);
        hash_node<k, v> *pr = hash_table[hash];
        hash_node<k,v> *prev = NULL;
        
        while(pr != NULL && pr->get_key()!=key)
        {
            prev =pr;
            pr = pr->get_next();
        }
        
        if(pr==NULL)
        {
            if(prev==NULL)
            {
                hash_table[hash] = new_node;
                return;
            }
            
            else
            {
                prev->set_next(new_node);
                return;
            }
        }
        else if (pr->get_key()==key)
        {
            pr->set_value(value);
            return;
        }
        return;
    }
};

class Assembler
{
public:
    
    Assembler(string file_name):
    source_file(file_name)
    {
        read_opcodes();
        cout<<"Ypoh";
    }
    
    void first_pass()
    {
        fstream source(source_file);
        string line;
        ofstream output("/Users/pratyushsharma/Documents/compilers/hash_map/output.txt");
        int i=0;
        while (getline(source, line)) {
            
            pair<int, string> opco = search_line(line);
            if (opco.first==4)
            {
                cout<<"ERROR AT LINE"<<i<<"  "<<line<<endl;
                return;
            }
            cout<<std::hex<<first_address<<"    "<<opco.second<<endl;
            output<<std::hex<<first_address<<"    "<<opco.second<<endl;
            first_address += opco.first;
            
            
        }
        output.close();
        source.close();
    }
    
    pair<int, string> search_line(const std::string& input)
    {
        std::regex rgx("([a-z]+) +(([a-z][a-z]?)|[0-9]+),? ?([a-m])", std::regex_constants::icase);
        std::regex single_op("([a-z]+) +([a-z][a-z]?|[0-9]+).*", std::regex_constants::icase);
        std::regex no_op("([a-z]+) +.*", std::regex_constants::icase);

        std::smatch match;
        
        if (std::regex_match(input, match, rgx))
        {
            std::cout << "Match\n";
            
            for (auto m : match)
              std::cout << "  submatch " << m << '\n';
            
            
            pair<int, string> temp;
            cout<<match[1]<<" "<<match[2]<<match[4]<<endl;
            int code = opcode_map.get(make_pair(match[1], make_pair(match[2], match[4])), temp);
            cout <<temp.first<<" "<<code<<" "<<temp.second<<endl;
            return code ? make_pair(temp.first, temp.second) : make_pair(4,"Error");;

            
        }
        else if(std::regex_match(input, match, single_op))
        {
            std::cout << "1 Match\n";
            
            for (auto m : match)
                std::cout << "  submatch " << m << '\n';
            
            pair<int, string> tmp;
            int code = opcode_map.get(make_pair(match[1], make_pair("1", "0")), tmp);
            cout<<tmp.first<<match[1]<<endl;
            if(tmp.first==0)
            {
                code = opcode_map.get(make_pair(match[1], make_pair("2", "0")), tmp);
            }
            return code==1|code==2 ? make_pair(tmp.first, tmp.second) : make_pair(4,"Error");
            
        }
        else if(std::regex_match(input, match, no_op))
        {
            std::cout << "0 Match\n";
            
            for (auto m : match)
                std::cout << "  submatch " << m << '\n';
            
            pair<int, string> tmp;
            int code = opcode_map.get(make_pair(match[1], make_pair("0", "0")), tmp);
            return code==1 ? make_pair(tmp.first, tmp.second) : make_pair(4,"Error");

        }
        else
        {
            cout<<"NO match"<<endl;
            return make_pair(4, "Error");
        }
        
            
    }
    
    
private:
    string source_file;
    
    
    int first_address = 0x0000;
    
    hash_map< pair <string, pair<string, string> >, pair<int, string> > opcode_map;
    
    void read_opcodes()
    {
        string pneumonic, first_op, second_op, opcode;
        ifstream myfile("InstructionOpcode/symbols8085.txt");
        cout<<"inm";
        
        pair<int, string> temp;
        while(myfile>>pneumonic>>first_op>>second_op>>opcode)
        {
            int size = 1;
            cout<<pneumonic+" "+first_op+" "+second_op<<endl;
            
            if (first_op == "1")
                size++;
            else if (first_op=="2")
                size+=2;
            
            if(second_op == "2" )
                size+=2;
            else if( second_op =="1")
                size++;
            
            search_line(pneumonic+" "+first_op+" "+second_op);
            cout<<pneumonic<<" "<<"size"<<" "<<size<<endl;
            opcode_map.put( make_pair(pneumonic, make_pair(first_op, second_op)), make_pair(size, opcode) );
            
            //opcode_map.get(make_pair(pneumonic, make_pair(first_op, second_op)),  temp);
        }
        
        myfile.close();
        
    }
};



int main()
{
    Assembler aa = Assembler("input.txt");
    cout<<"Start"<<endl;
    aa.first_pass();
    cout<<"done"<<endl;
    return 0;
}

int test_hash()
{

    // insert code here...
    key_hash m;
    hash_map<string, int> map;
    cout<<m("hello")<<m("helkp")<<endl;
    
    map.put("hello", 5);
    map.put("helkp",6);
    int art;
    
    string check ="hello";
    map.get(check, art);
    cout<<check<<art<<endl;
    
    check = "helkp";
    map.get(check, art);
    cout<<check<<art<<endl;
    
    check ="hello";
    map.get(check, art);
    cout<<check<<art<<endl;
    
    return 0;
}

