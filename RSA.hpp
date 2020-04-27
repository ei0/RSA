#pragma once
#include<time.h>
#include<fstream>
#include<iostream>

#include<boost/multiprecision/cpp_int.hpp>//����
#include<boost/multiprecision/random.hpp>//�����
#include<boost/multiprecision/miller_rabin.hpp>//���Լ��

using std::cin;
using std::cout;
using std::endl;
#define NUMBER 256
static int64_t tr_or_fa = 0;//�������´����ɴ˱�Ǳ�����true_or_false
typedef boost::multiprecision::int1024_t DataType;
struct key {
	DataType m_ekey;//������Կ��Կ
	DataType m_dkey;//������Կ˽Կ
	DataType m_pkey;//n
};

class NumProcess {//���ݴ�������
public:
	static DataType gcdEx(DataType a, DataType b, DataType* x, DataType* y);//��չŷ�����
	static bool GetGcd(DataType data1, DataType data2);//���Լ����������𷨣�
};
 DataType NumProcess::gcdEx(DataType a, DataType b, DataType* x, DataType* y)
{
	if (b == 0)
	{
		*x = 1, * y = 0;
		return a;
	}
	else
	{
		DataType gcd = gcdEx(b, a % b, x, y);
		/* r = GCD(a, b) = GCD(b, a%b) */
		DataType t = *x;
		*x = *y;
		*y = t - a / b * *y;
		return gcd;
	}//��չŷ�����
}//��չŷ�����
 bool NumProcess::GetGcd(DataType data1, DataType data2) {
	//clock_t start, end;
	//start = clock();
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
	if (data1 == 1) {
		//end = clock();
		//cout << "���Լ��TIMEs��" << (end - start) / CLOCKS_PER_SEC << 's' << endl;
		return true;

	}
	else {
		//end = clock();
		//cout << "���Լ��TIMEs��" << (end - start) / CLOCKS_PER_SEC << 's' << endl;
		return false;
	}
}//���Լ��



class RSA {//�ӽ���ģ��
	key m_key;
public:
	RSA();
	//a^b%n
	void encipher(const char* filename, const char* fileout);//���ļ�����
	void decrypt(const char* filename, const char* fileout);//���ļ�����
	DataType encipher(DataType data);//�����ݼ���
	DataType decrypt(DataType data);//�����ݽ���
	key getallkey();//���ع�˽��Կ
private:
	//key GetKeys();
	//������Կ˽Կ����
	DataType GetPrime();//�õ�����
	bool isprime(DataType data);//�ж�����
	DataType GetPkey(DataType prime1, DataType prime2);//�õ����в���
	DataType GetOrla(DataType prime1, DataType prime2);//ŷ����ʽ
	DataType GetEkey(DataType orla);//��Կ��˽�в���
	DataType GetDkey(DataType ekey, DataType orla);//˽Կ��˽�в���
};
RSA::RSA() {//���ɹ�Կ˽Կ
	DataType prime1 = GetPrime();
	DataType prime2 = GetPrime();
	while (prime1 == prime2)
		prime2 = GetPrime();
	DataType orla = GetOrla(prime1, prime2);
	//cout << "OL OK!" << endl;
	m_key.m_pkey = GetPkey(prime1, prime2);
	//cout << "PK OK!" << endl;
	m_key.m_ekey = GetEkey(orla);
	//cout << "Ek OK!" << endl;
	m_key.m_dkey = GetDkey(m_key.m_ekey, orla);
	//cout << "DK OK!" << endl;
}
void RSA::encipher(const char* filename, const char* fileout) {//���ļ����м���
	std::ifstream fin(filename, std::ios::binary);
	std::ofstream fout(fileout, std::ios::binary);
	if (!fin.is_open()) {
		perror("input file open filed");
		return;
	}
	char* bufin = new char[NUMBER];
	DataType* bufout = new DataType[NUMBER];

	while (!fin.eof()) {
		fin.read(bufin, NUMBER);
		/*if (!fin.good()) {
			std::cout << "read file" << filename << "failed" << endl;
			return;
		}*/
		int curNum = fin.gcount();
		cout << "���ܡ�����ȡ�˶�����" << curNum << endl;
		for (int i = 0; i < curNum; i++) {
			bufout[i] = encipher((DataType)bufin[i]);
		}
		fout.write((char*)bufout, curNum * sizeof(DataType));
		if (!fout.good()) {
			std::cout << "write file" << fileout << "failed" << endl;
			return;
		}
		tr_or_fa += curNum;
	}
	delete[] bufin;
	delete[] bufout;
	fin.close();
	fout.close();
}

void RSA::decrypt(const char* filename, const char* fileout) {//���ļ����н���
	std::ifstream fin(filename, std::ios::binary);
	std::ofstream fout(fileout, std::ios::binary);
	if (!fin.is_open()) {
		perror("input file open filed");
		return;
	}

	DataType* bufin = new DataType[NUMBER];
	char* bufout = new char[NUMBER];
	int64_t sum = 0;

	while (!fin.eof()) {
		fin.read((char*)bufin, NUMBER * sizeof(DataType));
		/*if (!fin.good()) {
			std::cout << "read file" << filename << "failed" << endl;
			return;
		}*/
		int num = fin.gcount();
		cout <<" ���ܡ�����ȡ�˶���"<<num << endl;
		num /= sizeof(DataType);
		for (int i = 0; i < num; i++) {
			bufout[i] = (char)decrypt(bufin[i]);
		}
		fout.write(bufout, num);
		if (!fout.good()) {
			std::cout << "write file" << fileout << "failed" << endl;
			return;
		}
		sum += num;
	}
	if (sum != tr_or_fa) {
		cout << endl;
		cout << "���ڸ�Ч�Ĵ������Ե��¼�С���ʵĳ�����������������Կ���м��ܣ�" << endl;
	}
	delete[] bufin;
	delete[] bufout;
	fout.close();
	fin.close();
}
DataType RSA::encipher(DataType data) {//�����ݽ��м���
	//return (DataType)pow(data, ekey) % pkey;
	DataType Ai = data;
	DataType msgE = 1;
	while (m_key.m_ekey)
	{
		if (m_key.m_ekey & 1)
			msgE = (msgE * Ai) % m_key.m_pkey;
		m_key.m_ekey >>= 1;
		Ai = (Ai * Ai) % m_key.m_pkey;
	}
	return msgE;
}
DataType RSA::decrypt(DataType data) {//�����ݽ��н���
	DataType Ai = data;
	DataType msgE = 1;
	while (m_key.m_dkey)
	{
		if (m_key.m_dkey & 1)
			msgE = (msgE * Ai) % m_key.m_pkey;
		m_key.m_dkey >>= 1;
		Ai = (Ai * Ai) % m_key.m_pkey;
	}
	return msgE;
	//return encipher(data, dkey, pkey);
}
key RSA::getallkey() {//�õ���Կ˽Կ
	return m_key;
}


DataType RSA::GetPrime() {//�õ�һ������
	//srand(time(NULL));

	//clock_t start, end;
	//start = clock();
	//cout << "GetPrime:";
	boost::random::mt19937 gen(time(NULL));
	boost::random::uniform_int_distribution<DataType> dist(0, DataType(1) << 256);
	DataType prime;
	while (1) {
		prime = dist(gen);
		//cout << "rand��" << endl;
		//cout << prime << endl;
		if (isprime(prime))
			break;
	}
	//end = clock();
	//cout << "Get prime time: " << (end - start) / CLOCKS_PER_SEC << 's' << endl;
	return prime;
}
bool RSA::isprime(DataType data) {//�ж�һ�����Ƿ�Ϊ����
	/*if (data <= 0) {
		return false;
	}
	for (int i = 2; i <= sqrt(data); i++) {
		if (data % i == 0) {
			return false;
		}
	}*/
	boost::random::mt11213b gen(time(NULL));
	if (miller_rabin_test(data, 25, gen)) {
		if (miller_rabin_test((data - 1) / 2, 25, gen))
		{
			//cout <<data<<' '<< "OK!"<<endl;
			return true;
		}
	}
	return false;

}
//n = pq
DataType RSA::GetPkey(DataType prime1, DataType prime2) {//��Կ��˽Կ�Ĺ��в��ֵĲ���
	return prime1 * prime2;
}
//f(n) = (p-1)(q-1)
DataType RSA::GetOrla(DataType prime1, DataType prime2) {
	return (prime1 - 1) * (prime2 - 1);
}
DataType RSA::GetEkey(DataType orla) {//����ŷ����ʽ�������Կ�������ֵĲ���
	//srand(time(NULL));

	boost::random::mt19937 gen(time(NULL));
	boost::random::uniform_int_distribution<DataType> dist(2, orla);

	DataType ekey;
	while (1) {
		//ekey = rand() % orla + 2;
		ekey = dist(gen);
		if (NumProcess::GetGcd(ekey, orla)) {//ͨ���ж����������Լ���Ƿ�Ϊ1 -> �ж������Ƿ���
			return ekey;
		}
	}
}
DataType RSA::GetDkey(DataType ekey, DataType orla) {//���˽Կ�е�˽�в���
	//DataType dkey = orla / ekey;
	//while (1) {
	//	if ((dkey * ekey) % orla == 1) {
	//		return dkey;
	//	}
	//	++dkey;
	//}

	//clock_t start, end;
	//start = clock();
	//DataType num1 = 1, num2 = 1;
	//NumProcess::gcdEx(ekey, orla, &num1, &num2);
//	if ((num1 * ekey) % orla == 1) {
		//end = clock();
		//cout << "��չgcd-TIMEs��" << (end - start) / CLOCKS_PER_SEC << 's' << endl;
	//	return num1;
	//}

	//else /*if ((num2 * ekey) % orla == 1)*/ {
		//end = clock();
		//cout << "��չgcd-TIMEs��" << (end - start) / CLOCKS_PER_SEC << 's' << endl;
	//	return num2;
	//}
	DataType num1 = 0, num2 = 0;
	NumProcess::gcdEx(ekey, orla, &num1, &num2);
	return (num1 % orla + orla) % orla;

}

