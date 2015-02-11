// search.cpp: определяет точку входа для консольного приложения.
//


#include "stdafx.h"
#define MAX_TEXT_LEN 1024
#include <fstream>

struct linezap
{
	TCHAR left[MAX_TEXT_LEN];
	TCHAR right[MAX_TEXT_LEN];
	unsigned int left_lenght;

	linezap *pref;
	linezap *next;

	linezap *apref;
	linezap *anext;
};

struct arrayline 
{
	linezap *begin;
	linezap *end;
};

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale( LC_ALL,"Russian" );
	if(argc < 2) {
		_tprintf_s("Не достаточно параметров\n");
		return 1;
	}
	TCHAR *file_name = argv[1];
	TCHAR file_name_output_lines[256] = "";
	TCHAR file_name_output_dub[256] = "";

	std::fstream file_input(file_name, std::ios::in);
	if(!file_input.is_open())
	{
		_tprintf_s("Ошибка при открытиии файла %s\n", file_name);
		file_input.close();
		return 2;
	}
	
	TCHAR bufer[MAX_TEXT_LEN * 2];
	TCHAR *pdest;
	// Для сохранения
	pdest = _tcsrchr(file_name, '.');
	if(pdest != NULL) 
	{
		_tcsncpy_s(bufer, file_name, (int)(pdest - file_name)); 
		_tcscat_s(file_name_output_lines, _countof(file_name_output_lines), bufer);
		_tcscat_s(file_name_output_lines, _countof(file_name_output_lines), ".out");
		_tcscat_s(file_name_output_lines, _countof(file_name_output_lines), pdest);

		_tcscat_s(file_name_output_dub, _countof(file_name_output_dub), bufer);
		_tcscat_s(file_name_output_dub, _countof(file_name_output_dub), ".dub");
		_tcscat_s(file_name_output_dub, _countof(file_name_output_dub), pdest);
	}
	else 
	{
		_tcscat_s(file_name_output_lines, _countof(file_name_output_lines), file_name);
		_tcscat_s(file_name_output_lines, _countof(file_name_output_lines), ".out");

		_tcscat_s(file_name_output_dub, _countof(file_name_output_dub), file_name);
		_tcscat_s(file_name_output_dub, _countof(file_name_output_dub), ".dub");
	}
	// Чтение файла
	arrayline sortbycountlines[1024];
	for(int i = 0; i < MAX_TEXT_LEN; i++) 
	{
		sortbycountlines[i].begin = NULL;
		sortbycountlines[i].end = NULL;
	}
	linezap *begin_file_line = NULL;
	linezap *end_file_line = NULL;
	unsigned int count_lines = 0;
	while (file_input.good())
	{
		file_input.getline(bufer, _countof(bufer));
		pdest = _tcsstr(bufer, "\t");
		if(pdest == NULL) 
		{
			_tprintf_s("Разделитель не найден");
		} 
		else 
		{
			TCHAR left[MAX_TEXT_LEN];
			TCHAR right[MAX_TEXT_LEN];
			unsigned int len;

			_tcsncpy_s(left, bufer, (int)(pdest - bufer));
			
			_tcsncpy_s(right, pdest + 1, (int)(bufer - (bufer - pdest + 1)));
			//_tcprintf_s(L"Str: %s|%s\n", left, right);
			linezap *file_line = new linezap;
			_tcscpy_s(file_line->left, left);
			_tcscpy_s(file_line->right, right);
			len = _tcslen(file_line->left);
			file_line->left_lenght = len;
			file_line->pref = NULL;
			file_line->next = NULL;
			file_line->anext = NULL;
			file_line->apref = NULL;
			count_lines += 1;
			if(begin_file_line == NULL) 
			{
				begin_file_line = file_line;
				end_file_line = file_line;
			} 
			else 
			{
				file_line->pref = end_file_line;
				end_file_line->next = file_line;
				end_file_line = file_line;
			}
			// Сортируем в массив
			if(sortbycountlines[len - 1].begin == NULL) 
			{
				sortbycountlines[len - 1].begin = file_line;
				sortbycountlines[len - 1].end = file_line;
			}
			else
			{
				file_line->apref = sortbycountlines[len - 1].end;
				sortbycountlines[len - 1].end->anext = file_line;
				sortbycountlines[len - 1].end = file_line;
			}
		}
	}
	file_input.close();
	linezap *begin_dub_line = NULL;
	linezap *end_dub_line = NULL;
	linezap *select_line = NULL;
	linezap *cursor_line = NULL;
	unsigned int position_lines = 0;
	// Поиск совпадений
	for(int i = 0; i < MAX_TEXT_LEN; i++)
	{
		if(sortbycountlines[i].begin == NULL) 
		{
			_tprintf_s("Нет строк с длиной = %d\n", i + 1);
			continue;
		}
		_tprintf_s("Поиск совпадений в строках длиной = %d\n", i + 1);
		select_line = sortbycountlines[i].begin;
		do {
			position_lines += 1;
			cursor_line = sortbycountlines[i].begin;
			
			do {
				if(select_line == cursor_line) // Если сравниваемая строка и курсор совпали то переходим на следующую итерацию
				{
					cursor_line = cursor_line->anext;
					continue;
				}
				// Сравниваем без учета регистра
				if(_tcsicmp(select_line->left, cursor_line->left) == 0) 
				{
					// Переносим строку в список дублей
					if(cursor_line->pref != NULL)
					{
						cursor_line->pref->next = cursor_line->next;
					} 
					else
					{
						cursor_line->next->pref = NULL;
					}
					if(cursor_line->next != NULL)
					{
						cursor_line->next->pref = cursor_line->pref;
					}
					else
					{
						cursor_line->pref->next = NULL;
					}
					if(begin_dub_line == NULL) 
					{
						begin_dub_line = cursor_line;
						end_dub_line = begin_dub_line;
					} 
					else 
					{
						cursor_line->pref = end_dub_line;
						end_dub_line->next = cursor_line;
						end_dub_line = cursor_line;
					}

					// Выкидываем из текущего списка
					linezap *abuf = cursor_line->anext;
					if(cursor_line->apref != NULL)
					{
						cursor_line->apref->anext = cursor_line->anext;
					} 
					else
					{
						cursor_line->anext->apref = NULL;
					}
					if(cursor_line->anext != NULL)
					{
						cursor_line->anext->apref = cursor_line->apref;
					}
					else
					{
						cursor_line->apref->anext = NULL;
					}
					cursor_line = abuf;
				}
				else
				{
					cursor_line = cursor_line->anext;
				}
			} while(cursor_line != NULL);
			select_line = select_line->anext; // Следующая строка с которой всё сравнивается
			if((count_lines - position_lines) % 100 == 0) 
			{
				_tprintf_s("Обработано %d строк из %d\n", position_lines, count_lines);
			}

		} while (select_line != NULL);
	}
	
	// Сохранение
	std::wfstream file_output_lines(file_name_output_lines, std::ios::out);
	_tprintf_s("Сохранение файла без дублей\n");
	if(!file_output_lines.is_open())
	{
		_tprintf_s("Ошибка при открытиии файла %s\n", file_name_output_lines);
		file_output_lines.close();
		return 2;
	}
	if(begin_file_line == NULL) 
	{
		_tprintf_s("Нечего сохранять");
		file_output_lines.close();
		return 3;
	} 
	else 
	{
		select_line = begin_file_line;
		
	}
	do {
		file_output_lines << select_line->left << L"\t" << select_line->right << L"\n";
		select_line = select_line->next;
	} while (select_line != NULL);
	file_output_lines.close();
	// Сохранение дублей
	std::wfstream file_output_dub(file_name_output_dub, std::ios::out);
	_tprintf_s("Сохранение файла с дублями\n");
	if(!file_output_dub.is_open())
	{
		_tprintf_s("Ошибка при открытиии файла %s\n", file_name_output_dub);
		file_output_dub.close();
		return 2;
	}
	if(begin_dub_line == NULL) 
	{
		_tprintf_s("Нечего сохранять");
		file_output_dub.close();
		return 3;
	} 
	else 
	{
		select_line = begin_dub_line;
		
	}
	do {
		file_output_dub << select_line->left << L"\t" << select_line->right << L"\n";
		select_line = select_line->next;
	} while (select_line != NULL);
	file_output_dub.close();
	return 0;
}

