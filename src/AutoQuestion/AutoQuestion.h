#pragma once

#include <vector>
#include <set>

enum class Cal
{
	Add,
	Decrease,
	Multiply,
	Div
};

struct OneQuestion
{
	std::vector<int> numbers;
	std::vector<Cal> cals;
	int result;
	int misNumberIndex;
};

class CAutoQuestion
{
public:
	CAutoQuestion();
	~CAutoQuestion();

	// 生成问题
	OneQuestion MakeQuestion(int numberCount, bool multiDivContinuous);

	// 设置加减最大最小值
	void SetAddDecreaseMinMaxNumber(int min, int max);

	// 设置乘除最大最小值
	void SetMultDivMinMaxNumber(int min, int max);

	// 设置支持的数学算法符号
	void SetUseCals(std::set<Cal> cals);

private:
	// 不带连续乘除的生成函数
	OneQuestion MakeQuestionNoMultiDivContinuous(int numberCount);

	// 带有连续乘除的生成函数
	OneQuestion MakeQuestionMultiDivContinuous(int numberCount);

	// 获得计算符号
	std::vector<Cal> GetCal(int calNum, bool multiDivContinuous);

	// 获取一个区间数字
	int GetUseableNumber(int min, int max);

	// 检查结果合法性
	bool CheckResult(OneQuestion& question);

private:
	int m_minAdd;	// 加减法最小值
	int m_maxAdd;	// 加减法最大值
	int m_minMut;	// 乘除法最小值
	int m_maxMut;	// 乘除法最大值

	std::vector<Cal> m_useCals;
};

