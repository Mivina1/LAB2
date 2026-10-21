//////////////1.2.1
////////////
////////////#include <Windows.h>
////////////#include <iostream>
////////////#include <thread>
////////////#include <mutex>
////////////#include <queue>
////////////#include <chrono>
////////////#include <string>
////////////
////////////using namespace std;
////////////
////////////queue<long long> numbers;
////////////mutex mtx;
////////////bool ready = false;
////////////
////////////bool isPrime(long long number)
////////////{
////////////    if (number < 2)
////////////        return false;
////////////
////////////    for (long long d = 2; d <= number / d; ++d)
////////////        if (number % d == 0)
////////////            return false;
////////////
////////////    return true;
////////////}
////////////
////////////void DataPreparation()
////////////{
////////////    cout << "Введіть цілі числа. Для завершення введіть q:\n";
////////////
////////////    string text;
////////////
////////////    while (cin >> text && text != "q")
////////////    {
////////////        try
////////////        {
////////////            size_t position;
////////////            long long number = stoll(text, &position);
////////////
////////////            if (position != text.size())
////////////                throw invalid_argument("Некоректне число");
////////////
////////////            lock_guard<mutex> lock(mtx);
////////////            numbers.push(number);
////////////        }
////////////        catch (const exception&)
////////////        {
////////////            cout << "Помилка! Введіть ціле число або q.\n";
////////////        }
////////////    }
////////////
////////////    lock_guard<mutex> lock(mtx);
////////////    ready = true;
////////////}
////////////
////////////void DataProcessing()
////////////{
////////////    unique_lock<mutex> lock(mtx);
////////////
////////////    while (!ready)
////////////    {
////////////        lock.unlock();
////////////        this_thread::sleep_for(chrono::milliseconds(100));
////////////        lock.lock();
////////////    }
////////////
////////////    queue<long long> localNumbers;
////////////    localNumbers.swap(numbers);
////////////
////////////    lock.unlock();
////////////
////////////    cout << "Прості числа: ";
////////////
////////////    while (!localNumbers.empty())
////////////    {
////////////        long long number = localNumbers.front();
////////////        localNumbers.pop();
////////////
////////////        if (isPrime(number))
////////////            cout << number << " ";
////////////    }
////////////
////////////    cout << endl;
////////////}
////////////
////////////int main()
////////////{
////////////    SetConsoleCP(1251);
////////////    SetConsoleOutputCP(1251);
////////////
////////////    thread preparation(DataPreparation);
////////////    preparation.detach();
////////////
////////////    thread processing(DataProcessing);
////////////    processing.join();
////////////
////////////    return 0;
////////////}
//////////
//////////
////////////1.2.2
//////////
//////////
//////////#include <Windows.h>
//////////#include <iostream>
//////////#include <thread>
//////////#include <mutex>
//////////#include <condition_variable>
//////////#include <chrono>
//////////
//////////using namespace std;
//////////
//////////mutex mtx;
//////////condition_variable cv;
//////////condition_variable started;
//////////
//////////int i = 0;
//////////int waitingCount = 0;
//////////
//////////void Waits(int number)
//////////{
//////////    unique_lock<mutex> lock(mtx);
//////////
//////////    cout << "Потік " << number << " починає очікування.\n";
//////////
//////////    ++waitingCount;
//////////    started.notify_one();
//////////
//////////    cv.wait(lock, [] { return i == 1; });
//////////
//////////    cout << "Потік " << number << " завершив очікування.\n";
//////////}
//////////
//////////void Awake()
//////////{
//////////    {
//////////        unique_lock<mutex> lock(mtx);
//////////
//////////        started.wait(lock, [] {
//////////            return waitingCount == 3;
//////////            });
//////////    }
//////////
//////////    this_thread::sleep_for(chrono::milliseconds(100));
//////////
//////////    {
//////////        lock_guard<mutex> lock(mtx);
//////////
//////////        cout << "Перше повідомлення: i = 0.\n";
//////////        cv.notify_all();
//////////    }
//////////
//////////    this_thread::sleep_for(chrono::milliseconds(100));
//////////
//////////    {
//////////        lock_guard<mutex> lock(mtx);
//////////
//////////        i = 1;
//////////
//////////        cout << "Друге повідомлення: i = 1.\n";
//////////        cv.notify_all();
//////////    }
//////////}
//////////
//////////int main()
//////////{
//////////    SetConsoleCP(1251);
//////////    SetConsoleOutputCP(1251);
//////////
//////////    thread first(Waits, 1);
//////////    thread second(Waits, 2);
//////////    thread third(Waits, 3);
//////////    thread awake(Awake);
//////////
//////////    awake.join();
//////////    first.join();
//////////    second.join();
//////////    third.join();
//////////
//////////    return 0;
//////////}
////////
//////////1.2.3
////////
////////
////////#include <Windows.h>
////////#include <iostream>
////////#include <thread>
////////#include <mutex>
////////#include <condition_variable>
////////#include <chrono>
////////
////////using namespace std;
////////
////////mutex mtx;
////////condition_variable cv;
////////condition_variable stateChanged;
////////
////////int i = 0;
////////int waitingCount = 0;
////////
////////bool processed = false;
////////bool stop = false;
////////
////////void Waits(int number)
////////{
////////    unique_lock<mutex> lock(mtx);
////////
////////    ++waitingCount;
////////    stateChanged.notify_one();
////////
////////    cv.wait(lock, [] {
////////        return i == 1 || stop;
////////        });
////////
////////    if (stop)
////////        return;
////////
////////    cout << "Повідомлення з потоку " << number << endl;
////////
////////
////////    i = 0;
////////    processed = true;
////////
////////    stateChanged.notify_one();
////////}
////////
////////void Thread1()
////////{
////////    Waits(1);
////////}
////////
////////void Thread2()
////////{
////////    Waits(2);
////////}
////////
////////void Thread3()
////////{
////////    Waits(3);
////////}
////////
////////void Notify()
////////{
////////    {
////////        unique_lock<mutex> lock(mtx);
////////
////////        stateChanged.wait(lock, [] {
////////            return waitingCount == 3;
////////            });
////////    }
////////
////////    this_thread::sleep_for(chrono::milliseconds(100));
////////
////////    lock_guard<mutex> lock(mtx);
////////
////////    cv.notify_one();
////////    i = 1;
////////
////////    
////////}
////////
////////int main()
////////{
////////    SetConsoleCP(1251);
////////    SetConsoleOutputCP(1251);
////////
////////    thread first(Thread1);
////////    thread second(Thread2);
////////    thread third(Thread3);
////////    thread notifier(Notify);
////////
////////    notifier.join();
////////
////////    {
////////        unique_lock<mutex> lock(mtx);
////////
////////        stateChanged.wait(lock, [] {
////////            return processed;
////////            });
////////
////////        stop = true;
////////    }
////////
////////    
////////    cv.notify_all();
////////
////////    first.join();
////////    second.join();
////////    third.join();
////////
////////    return 0;
////////}
//////
////////1.2.4
//////
//////
//////#include <Windows.h>
//////#include <iostream>
//////#include <thread>
//////#include <mutex>
//////#include <condition_variable>
//////#include <queue>
//////#include <optional>
//////#include <string>
//////
//////using namespace std;
//////
//////queue<optional<long long>> numbers;
//////mutex mtx;
//////condition_variable cv;
//////
//////bool isPrime(long long number)
//////{
//////    if (number < 2)
//////        return false;
//////
//////    for (long long d = 2; d <= number / d; ++d)
//////        if (number % d == 0)
//////            return false;
//////
//////    return true;
//////}
//////
//////void DataPreparation()
//////{
//////    queue<optional<long long>> localNumbers;
//////
//////    cout << "Введіть цілі числа. Для завершення введіть q:\n";
//////
//////    string text;
//////
//////    while (cin >> text && text != "q")
//////    {
//////        try
//////        {
//////            size_t position;
//////            long long number = stoll(text, &position);
//////
//////            if (position != text.size())
//////                throw invalid_argument("Некоректне число");
//////
//////            localNumbers.push(number);
//////        }
//////        catch (const exception&)
//////        {
//////            cout << "Помилка! Введіть ціле число або q.\n";
//////        }
//////    }
//////
//////    localNumbers.push(nullopt);
//////
//////    lock_guard<mutex> lock(mtx);
//////
//////    numbers.swap(localNumbers);
//////    cv.notify_one();
//////}
//////
//////void DataProcessing()
//////{
//////    unique_lock<mutex> lock(mtx);
//////
//////    cv.wait(lock, [] {
//////        return !numbers.empty();
//////        });
//////
//////    queue<optional<long long>> localNumbers;
//////    localNumbers.swap(numbers);
//////
//////    lock.unlock();
//////
//////    cout << "Прості числа: ";
//////
//////    while (!localNumbers.empty())
//////    {
//////        optional<long long> number = localNumbers.front();
//////        localNumbers.pop();
//////
//////        if (!number.has_value())
//////            break;
//////
//////        if (isPrime(number.value()))
//////            cout << number.value() << " ";
//////    }
//////
//////    cout << endl;
//////}
//////
//////int main()
//////{
//////    SetConsoleCP(1251);
//////    SetConsoleOutputCP(1251);
//////
//////    thread preparation(DataPreparation);
//////    preparation.detach();
//////
//////    thread processing(DataProcessing);
//////    processing.join();
//////
//////    return 0;
//////}
////
////
//////1.2.5
////#include <Windows.h>
////#include <iostream>
////#include <future>
////#include <chrono>
////#include <cmath>
////
////using namespace std;
////
////bool isPrime(long long number)
////{
////    if (number < 2)
////        return false;
////
////    if (number % 2 == 0)
////        return number == 2;
////
////    for (long long d = 3; d <= number / d; d += 2)
////        if (number % d == 0)
////            return false;
////
////    return true;
////}
////
////long long nthPrime(int n)
////{
////    if (n == 1)
////        return 2;
////
////    int count = 1;
////
////    for (long long number = 3; ; number += 2)
////    {
////        if (isPrime(number))
////        {
////            ++count;
////
////            if (count == n)
////                return number;
////        }
////    }
////}
////
////int main()
////{
////    SetConsoleCP(1251);
////    SetConsoleOutputCP(1251);
////
////    int n;
////    int mode;
////    int choice;
////
////    cout << "Введіть номер простого числа (1..1000000): ";
////
////    if (!(cin >> n) || n < 1 || n > 1000000)
////    {
////        cout << "Некоректний номер.\n";
////        return 1;
////    }
////
////    cout << "Оберіть режим:\n";
////    cout << "1 - deferred\n";
////    cout << "2 - async\n";
////
////    if (!(cin >> mode) || (mode != 1 && mode != 2))
////    {
////        cout << "Некоректний режим.\n";
////        return 1;
////    }
////
////    auto policy = mode == 1
////        ? launch::deferred
////        : launch::async;
////
////    auto start = chrono::steady_clock::now();
////
////    future<long long> result = async(policy, nthPrime, n);
////
////    cout << "Оберіть функцію для числа n:\n";
////    cout << "1 - квадратний корінь\n";
////    cout << "2 - синус (радіани)\n";
////    cout << "3 - натуральний логарифм\n";
////
////    if (!(cin >> choice) || choice < 1 || choice > 3)
////    {
////        cout << "Некоректний вибір.\n";
////        return 1;
////    }
////
////    double value;
////
////    if (choice == 1)
////        value = sqrt(n);
////    else if (choice == 2)
////        value = sin(n);
////    else
////        value = log(n);
////
////    cout << "Значення функції: " << value << endl;
////
////    auto beforeGet = chrono::steady_clock::now();
////
////    long long prime = result.get();
////
////    auto finish = chrono::steady_clock::now();
////
////    double waitingTime =
////        chrono::duration<double, milli>(finish - beforeGet).count();
////
////    double totalTime =
////        chrono::duration<double, milli>(finish - start).count();
////
////    cout << n << "-те просте число: " << prime << endl;
////    cout << "Час очікування в get(): " << waitingTime << " мс\n";
////    cout << "Загальний час: " << totalTime << " мс\n";
////
////    return 0;
////}
////
//
//
////1.2.6
//
//
//#include <Windows.h>
//#include <iostream>
//#include <thread>
//#include <mutex>
//#include <condition_variable>
//#include <future>
//#include <deque>
//#include <vector>
//#include <string>
//#include <utility>
//
//using namespace std;
//
//bool isPrime(long long number)
//{
//    if (number < 2)
//        return false;
//
//    if (number % 2 == 0)
//        return number == 2;
//
//    for (long long d = 3; d <= number / d; d += 2)
//        if (number % d == 0)
//            return false;
//
//    return true;
//}
//
//long long nthPrime(int n)
//{
//    if (n == 1)
//        return 2;
//
//    int count = 1;
//
//    for (long long number = 3; ; number += 2)
//    {
//        if (isPrime(number) && ++count == n)
//            return number;
//    }
//}
//
//int main()
//{
//    SetConsoleCP(1251);
//    SetConsoleOutputCP(1251);
//
//    deque<packaged_task<long long(int)>> tasks;
//    deque<int> indices;
//
//    vector<pair<int, future<long long>>> results;
//
//    mutex mtx;
//    condition_variable cv;
//
//    bool inputFinished = false;
//
//    promise<void> workerFinished;
//    future<void> finished = workerFinished.get_future();
//
//    thread worker([&]() -> void {
//        while (true)
//        {
//            packaged_task<long long(int)> task;
//            int n;
//
//            {
//                unique_lock<mutex> lock(mtx);
//
//                cv.wait(lock, [&] {
//                    return !tasks.empty() || inputFinished;
//                    });
//
//                if (tasks.empty() && inputFinished)
//                    break;
//
//                task = move(tasks.front());
//                tasks.pop_front();
//
//                n = indices.front();
//                indices.pop_front();
//            }
//
//            // Обчислення виконується без утримання м'ютекса.
//            task(n);
//        }
//
//        // Після сигналу робітник більше не використовує спільні дані.
//        workerFinished.set_value();
//        });
//
//    worker.detach();
//
//    cout << "Вводьте номери простих чисел (1..1000000).\n";
//    cout << "Для завершення введіть q:\n";
//
//    string text;
//
//    while (cin >> text && text != "q")
//    {
//        int n;
//
//        try
//        {
//            size_t position;
//            n = stoi(text, &position);
//
//            if (position != text.size() || n < 1 || n > 1000000)
//                throw invalid_argument("Некоректний номер");
//        }
//        catch (const exception&)
//        {
//            cout << "Введіть номер від 1 до 1000000 або q.\n";
//            continue;
//        }
//
//        packaged_task<long long(int)> task(nthPrime);
//        future<long long> result = task.get_future();
//
//        results.emplace_back(n, move(result));
//
//        {
//            lock_guard<mutex> lock(mtx);
//
//            tasks.push_back(move(task));
//            indices.push_back(n);
//        }
//
//        cv.notify_one();
//    }
//
//    {
//        lock_guard<mutex> lock(mtx);
//        inputFinished = true;
//    }
//
//    cv.notify_one();
//
//    for (auto& entry : results)
//    {
//        try
//        {
//            long long prime = entry.second.get();
//
//            cout << entry.first << "-те просте число: "
//                << prime << endl;
//        }
//        catch (const exception& error)
//        {
//            cout << "Помилка обчислення: " << error.what() << endl;
//        }
//    }
//
//    finished.get();
//
//    cout << "Усі завдання завершено.\n";
//
//    return 0;
//}

// 1.2.7 


#include <Windows.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <future>
#include <chrono>
#include <cmath>
#include <utility>

using namespace std;

bool isPrime(long long number)
{
    if (number < 2)
        return false;

    if (number % 2 == 0)
        return number == 2;

    for (long long d = 3; d <= number / d; d += 2)
        if (number % d == 0)
            return false;

    return true;
}

long long nthPrime(int n)
{
    if (n == 1)
        return 2;

    int count = 1;

    for (long long number = 3; ; number += 2)
    {
        if (isPrime(number) && ++count == n)
            return number;
    }
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int n;

    cout << "Введіть n (1..100000): ";

    if (!(cin >> n) || n < 1 || n > 100000)
    {
        cout << "Некоректне значення n.\n";
        return 1;
    }

    promise<long long> firstResult;
    promise<long long> tenthResult;
    promise<bool> startSignal;

    promise<void> firstFinished;
    promise<void> secondFinished;

    future<long long> firstFuture = firstResult.get_future();
    future<long long> tenthFuture = tenthResult.get_future();
    future<bool> signalFuture = startSignal.get_future();

    future<void> firstDone = firstFinished.get_future();
    future<void> secondDone = secondFinished.get_future();

    mutex outputMutex;

    thread first(
        [n,
        p1 = move(firstResult),
        p10 = move(tenthResult),
        signal = move(startSignal),
        done = move(firstFinished)]() mutable -> void
        {
            p1.set_value(nthPrime(n));

            signal.set_value(true);

            p10.set_value(nthPrime(n * 10));

            done.set_value();
        }
    );

    thread second(
        [n,
        &outputMutex,
        signal = move(signalFuture),
        done = move(secondFinished)]() mutable -> void
        {
            if (signal.get())
            {
                this_thread::sleep_for(chrono::seconds(2));

                {
                    lock_guard<mutex> lock(outputMutex);

                    cout << "Квадратний корінь із " << n
                        << ": " << sqrt(n) << endl;
                }
            }

            done.set_value();
        }
    );

    first.detach();
    second.detach();

    long long prime1 = firstFuture.get();

    {
        lock_guard<mutex> lock(outputMutex);

        cout << n << "-те просте число: " << prime1 << endl;
    }

    long long prime10 = tenthFuture.get();

    {
        lock_guard<mutex> lock(outputMutex);

        cout << n * 10 << "-те просте число: " << prime10 << endl;
    }

    firstDone.get();
    secondDone.get();

    return 0;
}
