# cpp-middle-project-sprint-4 <!-- omit in toc -->

- [Начало работы](#начало-работы)
- [Сборка проекта и запуск тестов](#сборка-проекта-и-запуск-тестов)
  - [Команды для сборки проекта](#команды-для-сборки-проекта)
  - [Команды для запуска приложения](#команды-для-запуска-приложения)
  - [Команда для запуска тестов](#команда-для-запуска-тестов)
  - [Команды для запуска отладчика](#команды-для-запуска-отладчика)
- [Сборка под Windows](#сборка-под-windows)
  - [Что нужно установить](#что-нужно-установить)
  - [Как собрать](#как-собрать)
  - [Пресеты CMake](#пресеты-cmake)


Шаблон репозитория для практического задания 4-го спринта «Мидл разработчик С++»

## Начало работы

1. Нажмите зелёную кнопку `Use this template`, затем `Create a new repository`.
2. Назовите свой репозиторий.
3. Склонируйте созданный репозиторий командой `git clone your-repository-name`.
4. Создайте новую ветку командой `git switch -c development`.
5. Откройте проект в `Visual Studio Code`.
6. Нажмите `F1` и откройте проект в dev-контейнере командой `Dev Containers: Reopen in Container`.

## Сборка проекта и запуск тестов

Данный репозиторий использует два инструмента:

- **Conan** — свободный менеджер пакетов для C и C++ с открытым исходным кодом (MIT). Позволяет настраивать процесс сборки программ, скачивать и устанавливать сторонние зависимости и необходимые инструменты. Подробнее о Conan:
  - https://habr.com/ru/articles/884464
  - https://docs.conan.io/2.0/tutorial/consuming_packages/build_simple_cmake_project.html
  - https://docs.conan.io/2.0/tutorial/consuming_packages/the_flexibility_of_conanfile_py.html
- **cmake** — генератор систем сборки для C и C++. Позволяет создавать проекты, которые могут компилироваться на различных платформах и с различными компиляторами. Подробнее о cmake:
  - https://dzen.ru/a/ZzZGUm-4o0u-IQlb
  - https://neerc.ifmo.ru/wiki/index.php?title=CMake_Tutorial
  - https://cmake.org/cmake/help/book/mastering-cmake/cmake/Help/guide/tutorial/index.html

### Команды для сборки проекта

```bash
mkdir build ; cd build

# Вызывается один раз перед сборкой проекта, чтобы скачать и установить все необходимые зависимости
conan install --settings=build_type=Debug --build=missing ..

# Вызывается каждый раз, когда необходимо собрать проект
conan build --settings=build_type=Debug ..
```

### Команды для запуска приложения


```bash
mkdir build ; cd build
conan install --settings=build_type=Debug ..
conan build --build=missing ..

./build/analyser -f files/sample.py
```

### Команда для запуска тестов

```bash
cd build
ctest --verbose
```

### Команды для запуска отладчика

В `Visual Studio Code` настройки параметров для запуска отладчика находятся в `.vscode/launch.json` файле. Поскольку в этом файле уже есть одна конфигурация `Launch Analyser` для запуска приложения, которое анализирует python файлы и вычисляет для них метрики, то для запуска отладчика достаточно нажать `F5` или открыть окно `Run and Debug` комбинацией клавиш `Ctrl+Shift+D`.

## Сборка под Windows

Dev-контейнер остаётся основным способом сборки, но проект собирается и нативно —
компилятором MSVC, без Docker и без WSL. Точка входа одна: `scripts\build_windows.ps1`.

### Что нужно установить

- **Visual Studio 2022** (Community достаточно) с рабочей нагрузкой
  «Разработка классических приложений на C++». Нужен компилятор MSVC v143;
  проверялось на `cl.exe` 19.44. Отдельного режима C++26 у MSVC нет, поэтому под
  ним проект собирается в самом новом, который есть, — `/std:c++latest` (CMake
  зовёт его CXX23); на Linux стандарт остаётся 26. Версии постарше не потянут
  `std::println`, `std::ranges::to` и `views::chunk_by`.
- **CMake ≥ 3.25** и **Ninja** на `PATH`. Оба приезжают вместе с компонентом
  Visual Studio «C++ CMake tools for Windows», если ставить их отдельно не хочется.
- **Conan 2** (`pip install conan`) — из него берутся boost, gtest и range-v3.
  Готового бинарника boost под msvc 194 в ConanCenter нет, поэтому первый запуск
  собирает boost из исходников; повторные берут его из кеша Conan.
- **tree-sitter CLI** — нужен не для сборки, а для запуска: и `analyser.exe`, и
  `metric_test` строят AST внешней командой `tree-sitter parse`. Если CLI на
  `PATH` нет, скрипт громко об этом скажет и исключит `metric_test` из прогона.

Отдельно запускать «Developer Command Prompt» не требуется — скрипт сам находит
`vcvars64.bat` (через `vswhere`, с запасными путями) и вносит окружение MSVC в
свой процесс.

### Как собрать

```powershell
powershell -ExecutionPolicy Bypass -File scripts\build_windows.ps1
```

Скрипт ставит зависимости через Conan, конфигурирует CMake, собирает всё,
прогоняет `ctest` и в конце печатает пути к получившимся `.exe`. Результат лежит
в `build\windows`, основной исполняемый файл — `build\windows\analyser.exe`:

```powershell
build\windows\analyser.exe -f files\sample.py
```

Полезные ключи:

| Ключ | Зачем |
| --- | --- |
| `-BuildType Debug` | сборка с отладочной информацией в `build\windows-debug` |
| `-Clean` | удалить каталог сборки и собрать с нуля |
| `-SkipTests` | не запускать `ctest` |

### Пресеты CMake

`CMakePresets.json` описывает обе платформы, поэтому IDE (Visual Studio, VS Code,
CLion) подхватывает конфигурацию сама:

| Пресет | Платформа | Генератор | Каталог сборки |
| --- | --- | --- | --- |
| `linux-default` | Linux / dev-контейнер | Unix Makefiles | `build/` |
| `windows-msvc-release` | Windows, MSVC x64 | Ninja | `build/windows/` |
| `windows-msvc-debug` | Windows, MSVC x64 | Ninja | `build/windows-debug/` |

Каждый пресет предполагает, что `conan install` уже отработал: путь к
`conan_toolchain.cmake` прописан прямо в пресете. Windows-пресеты рассчитаны на
окружение MSVC — если запускать их руками, а не через `scripts\build_windows.ps1`,
делать это надо из «Developer PowerShell for VS 2022», иначе CMake не найдёт
`cl.exe`.
