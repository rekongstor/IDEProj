IDE Project - LAN Seafight
==========================

## General
Проект для лабораторной работы по дисциплине Инструментальные средства разработки программного обеспечения.
Это игра "Морской бой", работающая по локальной сети.
Цель заключалась в применении навыков работы с github с gitflow, CI/CD в виде Jenkins и небольшом предварительном проектировании с помощью UML диаграмм.

## Code Style
Header files are commented in Doxygen style. You can generate a documentation using Doxygen software (http://www.doxygen.nl/).

## Install
1) Установить/Изменить Microsoft Visual Studio 2019, добавив следующие расришения: 
- Основные компоненты C++
- Адаптер тестов для Boost.Test
- Пакет SDK для Windows 10 (версия 10+)
- Диспетчер пакетов NuGet
- [опционально] Расширение GitHub для Visual Studio
- [опционально] Git для Windows
- [опционально] Языковой пакет: Английский (потом изменить в настройках)
2) Клонировать данный репозиторий, открыть решение в MSVS 2019
3) В любом из проектов восстановить boost пакеты (через Manage NuGet Packages)
4) Установить расширение "1-Click Docs" через контекстное меню Расширения
5) [для генерации документации] Назначить пути до Doxygen в Параметрах (предварительно установить Doxygen с официального сайта http://www.doxygen.nl/download.html)

## Preview
![Seafight](https://github.com/rekongstor/IDEProj/blob/master/preview.png)
