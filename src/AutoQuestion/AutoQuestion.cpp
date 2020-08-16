
#include <assert.h>
#include <stdlib.h>
#include "AutoQuestion.h"

CAutoQuestion::CAutoQuestion()
	:m_maxAdd(100)
	,m_minAdd(0)
	,m_minMut(1)
	,m_maxMut(9)
{
}


CAutoQuestion::~CAutoQuestion()
{
}

OneQuestion CAutoQuestion::MakeQuestion(int numberCount, bool multiDivContinuous)
{
	if (!multiDivContinuous)
	{
		return MakeQuestionNoMultiDivContinuous(numberCount);
	}

	return MakeQuestionMultiDivContinuous(numberCount);
}

OneQuestion CAutoQuestion::MakeQuestionNoMultiDivContinuous(int numberCount)
{
	OneQuestion question;
	// 只有加减乘除符号，所以数字个数比运算符多1；
	question.cals = GetCal(numberCount - 1, false);
	std::vector<int> numbers;
	numbers.resize(numberCount, -1);
	OneQuestion res;
	do
	{
		for (int i = 0;i < (int)question.cals.size();++i)
		{
			// 找乘除号计算所需数值
			switch (question.cals[i])
			{
			case Cal::Multiply:
				numbers[i] = GetUseableNumber(m_minMut, m_maxMut);
				numbers[i + 1] = GetUseableNumber(m_minMut, m_maxMut);
				break;
			case Cal::Div:
			{
				int number1 = GetUseableNumber(m_minMut, m_maxMut);
				int number2 = GetUseableNumber(m_minMut, m_maxMut);
				numbers[i] = number1*number2;
				numbers[i + 1] = number2;

				break;
			}
			default:
				break;
			}
		}
		question.numbers = numbers;

		// 剩下的为加减法
		res = question;
		// 填数
		for (auto&& val : res.numbers)
		{
			if (val == -1)
			{
				val = GetUseableNumber(m_minAdd, m_maxAdd);
			}
		}
	} while (!CheckResult(res));

	return res;
}

OneQuestion CAutoQuestion::MakeQuestionMultiDivContinuous(int numberCount)
{
	OneQuestion question;
	// 只有加减乘除符号，所以数字个数比运算符多1；
	question.cals = GetCal(numberCount - 1, true);
	std::vector<int> numbers;
	numbers.resize(numberCount, -1);
	OneQuestion res;
	do
	{
		bool lastIsMutDiv = false;
		for (int i = 0;i < (int)question.cals.size();++i)
		{
			// 找乘除号计算所需数值
			switch (question.cals[i])
			{
			case Cal::Multiply:
				if (!lastIsMutDiv)
				{
					numbers[i] = GetUseableNumber(m_minMut, m_maxMut);
				}
				numbers[i + 1] = GetUseableNumber(m_minMut, m_maxMut);
				lastIsMutDiv = true;
				break;
			case Cal::Div:
			{
				if (!lastIsMutDiv)
				{
					int number1 = GetUseableNumber(m_minMut, m_maxMut);
					int number2 = GetUseableNumber(m_minMut, m_maxMut);
					numbers[i] = number1*number2;
					numbers[i + 1] = number2;
				}
				else
				{
					numbers[i + 1] = GetUseableNumber(m_minMut, m_maxMut);
				}
				lastIsMutDiv = true;

				break;
			}
			default:
				lastIsMutDiv = false;
				break;
			}
		}
		question.numbers = numbers;

		// 剩下的为加减法
		res = question;
		// 填数
		for (auto&& val : res.numbers)
		{
			if (val == -1)
			{
				val = GetUseableNumber(m_minAdd, m_maxAdd);
			}
		}
	} while (!CheckResult(res));

	return res;
}

void CAutoQuestion::SetAddDecreaseMinMaxNumber(int min, int max)
{
	m_minAdd = min;
	m_maxAdd = max;
}

void CAutoQuestion::SetMultDivMinMaxNumber(int min, int max)
{
	m_minMut = min;
	m_maxMut = max;
}

void CAutoQuestion::SetUseCals(std::set<Cal> cals)
{
	assert(cals.size() > 0);
	for (auto one : cals)
		m_useCals.push_back(one);
}

std::vector<Cal> CAutoQuestion::GetCal(int calNum, bool multiDivContinuous)
{
	std::vector<Cal> res;
	while ((int)res.size() < calNum)
	{
		Cal newCal = m_useCals[rand() % m_useCals.size()];
		if (!multiDivContinuous
			&& res.size() > 0
			&& (newCal == Cal::Multiply || newCal == Cal::Div))
		{
			Cal last = res.back();
			if (last == Cal::Multiply || last == Cal::Div)
			{
				continue;
			}
		}

		res.push_back(newCal);
	}
	return res;
}

int CAutoQuestion::GetUseableNumber(int min, int max)
{
	int numeber = min;
	do
	{
		numeber = rand() % (max + 1);
	} while (numeber < min);
	return numeber;
}

bool CAutoQuestion::CheckResult(OneQuestion& question)
{
	OneQuestion qus = question;
	// 先计算乘除

	bool hasMutiDiv = false;
	do
	{
		hasMutiDiv = false;
		for (int i = 0;i < (int)qus.cals.size();i++)
		{
			int stepNumber = 0;
			if (qus.cals[i] == Cal::Multiply)
			{
				hasMutiDiv = true;
				if (qus.numbers[i] > m_maxMut || qus.numbers[i] < m_minMut
					|| qus.numbers[i + 1]> m_maxMut || qus.numbers[i + 1] < m_minMut)
					return false;
				stepNumber = qus.numbers[i] * qus.numbers[i + 1];
			}
			else if (qus.cals[i] == Cal::Div)
			{
				hasMutiDiv = true;

				stepNumber = qus.numbers[i] / qus.numbers[i + 1];

				if (stepNumber > m_maxMut || stepNumber < m_minMut
					|| qus.numbers[i + 1]> m_maxMut || qus.numbers[i + 1] < m_minMut)
					return false;
				if (qus.numbers[i] % qus.numbers[i + 1] != 0)
					return false;
			}

			// 有
			if (hasMutiDiv)
			{
				// 运算符减少
				qus.cals.erase(qus.cals.begin() + i);
				// 替换结果，减少数目
				qus.numbers[i] = stepNumber;
				qus.numbers.erase(qus.numbers.begin() + i + 1);
				break;
			}
		}

	} while (hasMutiDiv);

	// 剩下的都是加减法，直接算吧；
	int num = qus.numbers[0];
	for (int i = 0;i < (int)qus.cals.size();++i)
	{
		if (qus.cals[i] == Cal::Add)
		{
			num += qus.numbers[i + 1];
		}
		else if (qus.cals[i] == Cal::Decrease)
		{
			num -= qus.numbers[i + 1];
		}
	}
	if (num<m_minAdd || num>m_maxAdd)
		return false;

	question.result = num;
	question.misNumberIndex = GetUseableNumber(0, (int)question.numbers.size());
	return true;
}
