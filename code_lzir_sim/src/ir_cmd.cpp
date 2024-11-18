#include "ir_cmd.hpp"
#include <iostream>
#include <map>
#include <cassert>

std::map<std::string, eIRcmd> eIRcmd_strings{
    {"ERROR", eIRcmd::ERROR},
    {"SYMBOL", eIRcmd::SYMBOL},
    {"FUNCTION", eIRcmd::FUNCTION},
    {"STRUCT", eIRcmd::STRUCT},
    {"LABEL", eIRcmd::LABEL},
    {"BEGIN", eIRcmd::BEGIN},
    {"END", eIRcmd::END},
    {"RET", eIRcmd::RET},
    {"JMP", eIRcmd::JMP},
    {"JNE", eIRcmd::JNE},
    {"JE", eIRcmd::JE},
    {"CALL", eIRcmd::CALL},
    {"FRAME", eIRcmd::FRAME},
    {"MOV", eIRcmd::MOV},
    {"ADD", eIRcmd::ADD},
    {"SUB", eIRcmd::SUB},
    {"DIV", eIRcmd::DIV},
    {"MUL", eIRcmd::MUL},
    {"MOD", eIRcmd::MOD},
    {"OR", eIRcmd::OR},
    {"NOT", eIRcmd::NOT},
    {"NEG", eIRcmd::NEG},
    {"EQUAL", eIRcmd::NOTEQUAL},
    {"GREATER", eIRcmd::GREATER},
    {"LESS", eIRcmd::LESS},
    {"FLOOR", eIRcmd::FLOOR},
    {"MIN", eIRcmd::MIN},
    {"MAX", eIRcmd::MAX},
    {"INSERT", eIRcmd::INSERT},
    {"COMMENT", eIRcmd::COMMENT},
    {"DEREF", eIRcmd::DEREF},
    {"ALLOC", eIRcmd::ALLOC},
    {"USING", eIRcmd::USING},
    {"DEBUG", eIRcmd::DEBUG},
};

std::map<std::string, eIRparam> eIRparam_strings = {
    {"ERROR", eIRparam::ERROR},
    {"VAR", eIRparam::VAR},
    {"STRING", eIRparam::STRING},
    {"LABEL", eIRparam::LABEL},
    {"FUNC", eIRparam::FUNC},
    {"STRUCT", eIRparam::STRUCT},
    {"ARRAY", eIRparam::ARRAY},
    {"ENTER", eIRparam::ENTER},
    {"LEAVE", eIRparam::LEAVE},
    {"GENERAL", eIRparam::GENERAL},
    {"SOURCE", eIRparam::SOURCE},
    {"LINE", eIRparam::LINE},
};

class Stream_quicksave{
    public:
    std::istream& stream;
    std::streampos init_pos;
    std::ios_base::iostate init_state;

    Stream_quicksave(std::istream& stream
    ):stream(stream){
        init_pos = stream.tellg();
        init_state = stream.rdstate();
    };
    void load(){
        stream.clear(init_state);
        stream.seekg(init_pos);
    }
} quicksave;

template<typename T> bool try_extract(std::istream& stream, T& val){
    std::streampos init_pos = stream.tellg();
    std::ios_base::iostate init_state = stream.rdstate();

    if(stream >> val){
        return true;
    }else{
        stream.clear(init_state);
        stream.seekg(init_pos);
        return false;
    }
}

std::istream& operator>>(std::istream& stream, eIRcmd& ecmd){
    if(!stream){return stream;}
    std::string S;
    Stream_quicksave save(stream);
    if(!(stream >> S)){goto bad;}
    if(!eIRcmd_strings.count(S)){goto bad;}

    good:
    ecmd = eIRcmd_strings.at(S);
    return stream;

    bad:
    save.load();
    stream.setstate(std::ios_base::failbit);
    return stream;
}
std::istream& operator>>(std::istream& stream, eIRparam& eparam){
    if(!stream){return stream;}
    std::string S;
    Stream_quicksave save(stream);
    if(!(stream >> S)){goto bad;}
    if(!eIRparam_strings.count(S)){goto bad;}
    
    good:
    eparam = eIRparam_strings.at(S);
    return stream;
    
    bad:
    save.load();
    stream.setstate(std::ios_base::failbit);
    return stream;
}
std::istream& operator>>(std::istream& stream, IR_val& val){
    if(!stream){return stream;}
    IR_val res;
    Stream_quicksave save(stream);
    if(!(stream>>res.val)){goto bad;}
    assert(res.val.length()); /// >> always returns an nonempty str
    
    if(res.val[0] == '*'){
        res.deref = true;
        res.val.erase(res.val.begin());
    }
    
    if(res.val[0] == '&'){
        res.ref = true;
        res.val.erase(res.val.begin());
    }
    
    if(res.ref && res.deref){
        goto bad;
    }
    
    good:
    val = res;
    return stream;
    
    bad:
    save.load();
    stream.setstate(std::ios_base::failbit);
    return stream;
}

std::istream& operator>>(std::istream& stream, IR_lit_val& litval){
    if(!stream){return stream;}
    IR_lit_val res;
    int n;
    std::string S;
    Stream_quicksave save(stream);
    
    if(stream >> n){
        res = n;
        goto good;
    }else{
        save.load();
        if(stream >> S){
            res = S;
            goto good;
        }else{
            goto bad;
        }
    }
    good:
    litval = res;
    return stream;

    bad:
    save.load();
    stream.setstate(std::ios_base::failbit);
    return stream;
}
std::istream& operator>>(std::istream& stream, IR_param& param){
    if(!stream){return stream;}
    IR_param res;
    Stream_quicksave save(stream);

    if(!(stream >> res.type)){goto bad;}

    switch(res.type){
        case(eIRparam::ARRAY):
            if(!(stream >> res.val)){goto bad;}
        break;
        default: break;
    }

    good:
    param = res;
    return stream;
    
    bad:
    save.load();
    stream.setstate(std::ios_base::failbit);
    return stream;
}
std::istream& operator>>(std::istream& stream, IR_arg& arg){
    if(!stream){return stream;}
    IR_arg res;
    IR_val val;
    IR_param param;
    Stream_quicksave save(stream);

    if(stream >> param){
        res = param;
        goto good;
    }else{
        save.load();
        if(stream >> val){
            res = val;
            goto good;
        }else{
            goto bad;
        }
    }
    good:
    arg = res;
    return stream;
    
    bad:
    save.load();
    stream.setstate(std::ios_base::failbit);
    return stream;
}
std::istream& operator>>(std::istream& stream, IR_Cmd& cmd){
    if(!stream){return stream;}
    IR_Cmd res;
    Stream_quicksave save(stream);
    if(!(stream >> res.type)){goto bad;}
    do{
        IR_arg arg;
        Stream_quicksave save2(stream);
        if(stream >> arg){
            res.args.push_back(arg);
        }else{
            save2.load();
            goto good;
        }
    }while(stream);

    good:
    cmd = res;
    return stream;
    
    bad:
    save.load();
    stream.setstate(std::ios_base::failbit);
    return stream;
}