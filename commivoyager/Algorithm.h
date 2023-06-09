#pragma once
//Algorithm.h
class Algorithm
{
public:
	char* name = "Algorithm"; // Название алгоритма
	std::vector<std::vector<int>> data; // Массив значений (матрица)
	Algorithm();
	Algorithm(std::vector<std::vector<int>>);
	Algorithm(char*);
	bool LoadData(std::vector<std::vector<int>>);
	bool LoadData(char*);
	virtual void Run(); // Метод для запуска алгоритма
protected:
	int GetStrCount(std::ifstream&); // Считываем количество строк в файле
	int GetColCount(std::ifstream&); // Считываем количество столбцов в файле
	virtual bool validateData(); // Метод для проверки значений в матрице. Вызывается перед Run()
};