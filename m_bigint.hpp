#pragma once
#include<iostream>
#include<utility>

class bigint
{
private:
    std::string m_number;
    std::string add(std::string num1,std::string num2);
    std::string sub(std::string num1,std::string num2);
    std::string mul(std::string num1,std::string num2);
    std::pair<std::string,std::string> dev(std::string num1,std::string num2);
    bool less(std::string& num1,std::string& num2);
public:
    bigint(){};
    bigint(const std::string& num);
    bigint(const int num);

    bigint operator+(bigint& bi);
    bigint operator-(bigint& bi);
    bigint operator*(bigint& bi);
    bigint operator/(bigint& bi);
    bigint operator%(bigint& bi);
    friend std::ostream& operator<<(std::ostream& m_cout,bigint& bi);

    ~bigint();
};
std::string bigint::add(std::string num1,std::string num2){
    //补位
    int len1 = num1.size();
    int len2 = num2.size();
    int diffnum = abs(len1-len2);
    int longsize = len1>len2?len1:len2;
    if(len1<len2)
        num1.insert(0,diffnum,'0');
    else if(len1>len2)
        num2.insert(0,diffnum,'0');
    //计算
    std::string ret;
    ret.resize(longsize);
    int step = 0;
    for(int i = longsize-1;i>=0;--i){
        ret[i] = (num1[i]-'0')+(num2[i]-'0')+step;
        ret[i] += '0';
        if(ret[i]>'9'){
            ret[i] -= 10;
            step = 1;
        }
        else{
            step = 0;
        }
    }
    if(step == 1)
    ret.insert(0,1,'1');
    return ret;

}
std::string bigint::sub(std::string num1,std::string num2){
    //补位
    int len1 = num1.size();
    int len2 = num2.size();
    int diffnum = abs(len1-len2);
    int longsize = len1>len2?len1:len2;
    if(len1<len2)
        num1.insert(0,diffnum,'0');
    else if(len1>len2)
        num2.insert(0,diffnum,'0');
    //计算
    std::string ret;
    ret.resize(longsize);
    for(int i = longsize-1;i>=0;i--){
        if(num1[i]<num2[i]){
            num1[i]+=10;
            num1[i-1]--;
        }
        ret[i] = (num1[i]-'0')-(num2[i]-'0')+'0';
    }
    while(ret.size()>1&&ret[0] == '0'){
        ret.erase(0,1);
    }
    return ret;

}
std::string bigint::mul(std::string num1,std::string num2){
    if(num2.size()>num1.size()){
        swap(num2,num1);
    }
    std::string ret = "0";
    for(int i = num2.size()-1;i>= 0;--i){
        //获取当前乘数对应位的值
        int curDigit = num2[i]-'0';
        int step = 0;
        //当前位乘积结果
        std::string temp = num1;
        for(int j = temp.size()-1;j>=0;j--){
            //逐位相乘
            temp[j] = (temp[j]-'0')*curDigit+step;
            //更新进位
            if(temp[j]>9){
                step = temp[j]/10;
                temp[j] = temp[j] - step *10;
            }
            else
            {
                step = 0;
            }
            temp[j] += '0';
        }
        if(step>0)
            temp.insert(0,1,step+'0');
        temp.append(num2.size()-1-i,'0');//补零
        ret = add(ret,temp);
    }
    return ret;
}
std::pair<std::string,std::string> bigint::dev(std::string num1,std::string num2){
    //借助减法来实现
    std::string ret;//商
    std::string rem;//余数
    //给除数进行放大
    int diffnum = num1.size()-num2.size();
    num2.append(diffnum,'0');
    for(int i = 0;i<=diffnum;++i)
    {
        //记录减法执行的次数
        char count = '0';
        while(1){
            if(less(rem,num2))
                break;
            rem = sub(rem,num2);
            ++count;
        }
        ret += count;
        num2.pop_back();
    }
    while(ret.size()>1&&ret[0] == '0'){
        ret.erase(0,1);
    }
    return make_pair(ret,rem);
}
bool bigint::less(std::string& num1,std::string& num2){
    if(num1.size()<num2.size())
        return true;
    if(num1.size()>num2.size())
        return false;
    return num1<num2;
}

bigint bigint::operator+(bigint& bi){
    std::string ret = add(m_number,bi.m_number);
    return bigint(ret);
}
bigint bigint::operator-(bigint& bi){
    std::string ret = sub(m_number,bi.m_number);
    return bigint(ret);
}
bigint bigint::operator*(bigint& bi){
    std::string ret = mul(m_number,bi.m_number);
    return bigint(ret);
}
bigint bigint::operator/(bigint& bi){
    std::pair<std::string,std::string> ret = dev(m_number,bi.m_number);
    return bigint(ret.first);
}
bigint bigint::operator%(bigint& bi){
    std::pair<std::string,std::string> ret = dev(m_number,bi.m_number);
    return bigint(ret.second);
}
bigint::bigint(const std::string& num):
    m_number(num)
    {}
 std::ostream& operator<<(std::ostream& m_cout,bigint& bi){
     m_cout<<bi.m_number<<std::endl;
     return m_cout;
 }