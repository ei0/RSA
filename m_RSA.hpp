#pragma once
#include<time.h>
#include<fstream>
#include<iostream>
#define NUMBER 256
typedef long DataType;
struct key {
	DataType m_ekey;//加密密钥公钥
	DataType m_dkey;//解密密钥私钥
	DataType m_pkey;//n
};
/*
1. 产生素数
2. 求n
3. 求f(n)
4. 求e 1<e<f(n),且e与f(n)互质
5. 求d e*d%f(n) = 1;
(e,n)(d,n)
*/
DataType gcdEx(DataType a, DataType b, DataType* x, DataType* y)
{
	if (b == 0)
	{
		*x = 1, * y = 0;
		return *x > * y ? *x : *y;
	}
	else
	{
		long gcd = gcdEx(b, a % b, x, y);
		/* r = GCD(a, b) = GCD(b, a%b) */
		DataType t = *x;
		*x = *y;
		*y = t - a / b * *y;
		return gcd;
	}
}//扩展欧几里得
class RSA {
	key m_key;
public:
	RSA();
	//a^b%n
    void encipher(const char* filename,const char* fileout);
    void decrypt(const char* filename,const char* fileout);
	DataType encipher(DataType data, DataType ekey, DataType pkey);//加密
	DataType decrypt(DataType data, DataType dkey, DataType pkey);//解密
	DataType GetPrime();//得到素数
	bool isprime(DataType data);//判断素数
	DataType GetPkey(DataType prime1, DataType prime2);//得到共有部分
	DataType GetOrla(DataType prime1, DataType prime2);//欧拉公式
	DataType GetEkey(DataType orla);//公钥的私有部分
	DataType GetDkey(DataType ekey, DataType orla);//私钥的私有部分
	bool GetGcd(DataType data1, DataType data2);//最大公约数（更相减损法）
	void GetKeys();
	key getallkey();

};
    RSA::RSA(){
      GetKeys();
    }
void RSA::encipher(const char* filename,const char* fileout){
    std::ifstream fin(filename,std::ios::binary);
    std::ofstream fout(fileout,std::ios::binary);
    if(!fin.is_open()){
        perror("input file open filed");
        return;
    }
    char* bufin = new char[NUMBER];
    DataType* bufout = new DataType[NUMBER];

    std::ofstream fout(filename,std::ios::binary);
    while(!fin.eof()){
        fin.read(bufin,NUMBER);
        int curNum = fin.gcount();
        for(int i = 0;i<curNum;i++){
            bufout[i] = encipher((DataType)bufin[i],m_key.m_ekey,m_key.m_pkey);
        }
        fout.write((char*)bufout,curNum*sizeof(DataType));
    }
    delete [] bufin;
    delete [] bufout;
    fin.close();
    fout.close();
}
void RSA::decrypt(const char* filename,const char* fileout){
    std::ifstream fin(filename,std::ios::binary);
    std::ofstream fout(fileout,std::ios::binary);
    if(!fin.is_open()){
        perror("input file open filed");
        return;
    }
    char* bufout = new char[NUMBER];
    DataType* bufin = new DataType[NUMBER];
    while(!fin.eof()){
        fin.read((char*)bufin,NUMBER*sizeof(DataType));
        int num = fin.gcount();
        num /= sizeof(DataType);
        for(int i = 0;i<num;i++){
            bufout[i] = decrypt(bufin[i],m_key.m_dkey,m_key.m_pkey);
        }
        fout.write(bufout,num);
    }
    delete [] bufin;
    delete [] bufout;
    fout.close();
    fin.close();
}
void RSA::GetKeys()//拿到密钥
{
	DataType prime1 = GetPrime();
	DataType prime2 = GetPrime();
	while (prime1 == prime2)
		prime2 = GetPrime();
	DataType orla = GetOrla(prime1, prime2);
	m_key.m_pkey = GetPkey(prime1, prime2);
	m_key.m_ekey = GetEkey(orla);
	m_key.m_dkey = GetDkey(m_key.m_ekey,orla);
}
key RSA::getallkey() {
	return m_key;
}
DataType RSA::encipher(DataType data, DataType ekey, DataType pkey) {
	//return (DataType)pow(data, ekey) % pkey;
	DataType Ai = data;
	DataType msgE = 1;
	while (ekey)
	{
		if (ekey & 1)
			msgE = (msgE * Ai) % pkey;
		ekey >>= 1;
		Ai = (Ai * Ai) % pkey;
	}
	return msgE;
}
DataType RSA::decrypt(DataType data, DataType dkey, DataType pkey) {
	return encipher(data, dkey, pkey);
}
DataType RSA::GetPrime() {//得到一个素数
	srand(time(NULL));
	DataType prime;
	while (1) {
		prime = rand() % 100 + 2;
		if (isprime(prime))
			break;
	}
	return prime;
}
bool RSA::isprime(DataType data) {//判断一个数是否为素数
	if (data <= 0) {
		return false;
	}
	for (int i = 2; i <= sqrt(data); i++) {
		if (data % i == 0) {
			return false;
		}
	}
	return true;

}
//n = pq
DataType RSA::GetPkey(DataType prime1, DataType prime2) {//公钥与私钥的共有部分的产生
	return prime1 * prime2;
}
//f(n) = (p-1)(q-1)
DataType RSA::GetOrla(DataType prime1, DataType prime2) {
	return (prime1 - 1) * (prime2 - 1);
}
DataType RSA::GetEkey(DataType orla) {//传入欧拉公式结果，公钥独立部分的产生
	srand(time(NULL));
	DataType ekey;
	while (1) {
		ekey = rand() % orla + 2;
		if (GetGcd(ekey, orla)) {//通过判断两数的最大公约数是否为1 -> 判断两数是否互质
			return ekey;
		}
	}
}
DataType RSA::GetDkey(DataType ekey, DataType orla) {//求解私钥中的私有部分
	//DataType dkey = orla / ekey;
	//while (1) {
	//	if ((dkey * ekey) % orla == 1) {
	//		return dkey;
	//	}
	//	++dkey;
	//}
	DataType num1 = 1, num2 = 1;
	if (gcdEx(ekey, orla, &num1, &num2) == 1) {
		if ((num1 * ekey) % orla == 1)
			return num1;
		else if((num2 * ekey) % orla == 1)
			return num2;
	}
		return -1;
}
bool RSA::GetGcd(DataType data1, DataType data2) {
	DataType tmp = 1;
	while (tmp) {
		if (data1 > data2)
			tmp = data1 - data2;
		else
			tmp = data2 - data1;
		if (tmp) {
			data1 = data2;
			data2 = tmp;
		}
	}
	if (data1 == 1)
		return true;
	else
		return false;
}//最大公约数
