#include "NTL/ZZ.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <codecvt>
#include <regex>
#include <chrono>

using namespace NTL;
using namespace std;

wstring ZZToWstring(const ZZ& a)
{
    stringstream ss;
    ss << a;
    string temp = ss.str();
    return wstring(temp.begin(), temp.end());
}

ZZ FileStringToZZ(const wstring& s)
{
    ZZ result;
    wsmatch wideMatch;
    wregex wrx(L"\\s*=\\s*([0-9]+)\\s\\(([0-9]+)");

    if (regex_search(s.cbegin(), s.cend(), wideMatch, wrx))
    {
        wstring ws = wideMatch[1].str();
        result = conv<ZZ>(string(ws.begin(), ws.end()).c_str());
        long bits = stol(wideMatch[2]);
        if (bits != NumBits(result))
        {
            wcout << L"Считано бит: " << NumBits(result) << L"\tДолжно быть: " << bits << endl;
        }
    }
    else
    {
        wcout << L"Неудалось считать строку: " << s << endl;
    }
    return result;
}

class Problem
{
private:
    ZZ p;	// простое число
    ZZ q;	// простое число
    ZZ e;	// Открытая экспонента
    ZZ Mc;	// Зашифрованное сообщение
    ZZ Md;  // Расшифрованное сообщение
    ZZ n;	// n=p*q
    ZZ phi;	// значение функции Эйлера от n
    ZZ d;	// Закрытая экспонента
    ZZ tmp, tmp1;	// Временные переменные
public:

    Problem(ZZ p, ZZ q, ZZ e, ZZ Mc)
    {
        this->p = p;
        this->q = q;
        this->e = e;
        this->Mc = Mc;
    }

    ZZ Solve()
    {
        // Вычисляем n=p*q
        this->n = this->p * this->q;
        // Вычисляем значение функции Эйлера phi(n)=(p-1)*(q-1)
        this->phi = (this->p - 1) * (this->q - 1);
        // Находим такое d, чтобы d*e = 1 mod phi, d<phi (Обратное по модулю)
        XGCD(this->tmp, this->d, this->tmp1, this->e, this->phi); // XGCD - Расширенный алгоритм Евклида
        this->d = (this->d % this->phi + this->phi) % this->phi;
        this->tmp = MulMod(this->e, this->d, this->phi); // ZZ x = MulMod(const ZZ& a, const ZZ& b, const ZZ& n); x	= (a * b) % n
        if (this->tmp != 1) {
            return (ZZ)-1;
        }
        this->Md = PowerMod(this->Mc, this->d, this->n);
        return this->Md;
    }

    wstring GetAdditionalInfo()
    {
        wstringstream ss;

        ss << L"Вычисляем n=p*q= " << ZZToWstring(this->n) << L" (" << NumBits(this->n) << L" бит)" << endl;
        ss << L"Обратное по модулю к e (найденное с помощью расширенного алгоритма Евклида) значение d = " << ZZToWstring(this->d) << L" (" << NumBits(this->d) << L" бит)" << endl;
        ss << L"Проверка правильности нахождения обратного значения: e*d mod phi=" << ZZToWstring(this->tmp);
        if (this->tmp == 1)
        {
            ss << L"\tOk" << endl;
            ss << L"Секретный ключ: {d=" << ZZToWstring(this->d) << L", n=" << ZZToWstring(this->n) << L"}" << endl;
            ss << L"Открытый ключ: {e=" << ZZToWstring(this->e) << L", n=" << ZZToWstring(this->n) << L"}" << endl;
            ss << L"Расшифрованное сообщение: Md = " << ZZToWstring(this->Md) << L" (" << NumBits(this->Md) << L" бит)" << endl;
        }
        else
        {
            ss << L"\n\tОшибка: Обратный элемент найден не верно!" << endl;
        }
        
        return ss.str();
    }
};



int main() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    setlocale(LC_ALL, "Russian");
#endif
    std::wifstream wif("Problems.txt");
    wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));

    while (!wif.eof())
    {
        wstring line;
        getline(wif, line);
        wcout << line << endl;
        size_t variant_pos = line.find_last_of(L"Вариант", line.size());
        if (variant_pos + 1 != line.size())
        {
            getline(wif, line);
            wcout << line << endl;
            ZZ p = FileStringToZZ(line);
            getline(wif, line);
            wcout << line << endl;
            ZZ q = FileStringToZZ(line);
            getline(wif, line);
            wcout << line << endl;
            ZZ e = FileStringToZZ(line);
            getline(wif, line);
            wcout << line << endl;
            ZZ Mc = FileStringToZZ(line);

            Problem problem(p, q, e, Mc);
            auto t1 = chrono::high_resolution_clock::now();
            ZZ Md = problem.Solve();
            auto t2 = chrono::high_resolution_clock::now();

            chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
            wcout << L"Затраченное время: " << time_span.count() << L" Секунд" << endl;

            wstring info = problem.GetAdditionalInfo();
            wcout << info << endl;
        }
    }
    wif.close();

    system("pause");
    return 0;
}