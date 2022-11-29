# XKeysController

Служба для получения сигналов с OPC-сервера и последующего отображения изменений с помощью светодиодов на программируемой клавиатуре X-keys XK-60.

Для успешной работы нужно:
1. Иметь поднятый на ПК OPC-сервер с нужными вам сигналами.
2. Клавиатура X-Keys XK-60.

Важно:
Сигналы должны быть типа UA_Boolean, иначе корректного отображения не получится.
 
Установка:
1. git clone git@github.com:Rayzedan/XKeysControl.git
2. Собрать проект с помощью MSVC 2022, зайти через cmd запущенную с правами администратора в папку /Release, выполнить .\Astra.XKeysDriver.exe run для тестирования проекта.
3. Для установки службы выполнить .\Astra.XKeysDriver.exe install -start-type 2 -account <username> -password <password>.
4. При успешной установке перезапустить ПК.
5. Поменять конфигурацию в файле на нужную вам. (файл нужно переместить в каталог с Astra.XKeysDriver.exe)
Пример конфигурационного файла лежит в репозитории (см. keyboard_config.xml).

Важно:
Учётная запись должна иметь права администратора, чтобы служба могла считывать файл с конфигурацией.

Планы:
1. Доделать логи службы для подробной информации о её работе.
2. Облегчить установку через командную строку.
3. Оптимизировать код.
4. Написать мануал по настройке конфигурационного файла и возможным ошибкам при работе/установке службы.
