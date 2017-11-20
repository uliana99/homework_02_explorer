#include "explorer.hpp"

//  export EXPLORER_LOG_PATH="log.txt"
//  g++ main.cpp -std=c++14 -lboost_system -lboost_filesystem -o open && ./open explorer/file.dat


const path get_name(const path _path);
double get_size(const path _path);
void get_data(const path _path);

int main(int argc, char const *argv[]) {
    try {
        string _path;

        //  Исключение - вызов программы без параметра
        if(argc < 2) throw logic_error("do not have any adress");

        //  Случай если адрес передали example: explorer file.dat
        if(argc > 2) {
            for(int i = 1; i < argc; i++) {
                _path += argv[i];
                if(i != (argc - 1))
                    _path += "/";
            }
        } else {
        //  Случай если адрес передали example: explorer/file.dat
            _path += argv[1];
        }

        //  Проверка на существование файла по заданному адресу
        if(!exists(_path)) throw logic_error("do not found any files");

        //  Проверка существования файла log.txt
        
        if(!exists(getenv("EXPLORER_LOG_PATH"))) {
            //  Создаем файл log.txt
            std::ofstream ofs(getenv("EXPLORER_LOG_PATH"));
            ofs << "the record of modifications:" << endl << endl;
            ofs.close();
        }

        const path filePath = _path;

        // Разбиваем процесс на 3 потока
        std::future<const path> thr1(std::async(get_name, filePath));
        std::future<double> thr2(std::async(get_size, filePath));
        std::future<void> thr3(std::async(std::launch::async, get_data, filePath));

        thr1.wait();
        thr2.wait();

        //  Вывод в конец файла.
        std::ofstream session((getenv("EXPLORER_LOG_PATH")), ios_base::app);

        session << "name:\t" << thr1.get() << endl;
        session << "size:\t" << thr2.get() << " Kb" << endl;

        session.close();

        thr3.wait();

    } catch(const exception& e) {
        cout << e.what() << endl;
    } 

    return 0;
}

const path get_name(const path _path) {
    return _path.filename();
}
double get_size(const path _path) {
    double _size = file_size(_path);
    return (_size / 1000);
}
void get_data(const path _path) {
    std::time_t cftime = last_write_time(_path);
    std::ofstream session_dat((getenv("EXPLORER_LOG_PATH")), ios_base::app);
    session_dat << "date:\t" << asctime(localtime(&cftime)) << endl;
    session_dat.close();
}

