#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

string GetFileContents(string file) {
    ifstream stream(file);
    return {(istreambuf_iterator<char>(stream)), istreambuf_iterator<char>()};
}

bool ProcessInclude(ifstream& in_stream, ofstream& out_stream, const path& in_file,
                    const vector<path>& include_directories, int line_number) {
    string line;
    static regex patternOne(R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
    static regex patternTwo(R"/(\s*#\s*include\s*<([^>]*)>\s*)/");
    smatch matches;

    while (getline(in_stream, line)) {
        line_number++;

        if (regex_match(line, matches, patternOne) || regex_match(line, matches, patternTwo)) {
            string include_file = matches[1].str();
            path full_path = in_file.parent_path() / include_file;
            ifstream include_stream(full_path);

            bool is_found = include_stream.is_open();
            for (const auto& dir : include_directories) {

                if (!is_found) {
                    include_stream.open(dir / include_file);
                    is_found = include_stream.is_open();
                }
            }

            if (!is_found) {
                cout << "unknown include file " << include_file << " at file " << in_file.string()
                     << " at line " << line_number << endl;
                return false;
            } else {

                if (!ProcessInclude(include_stream, out_stream, full_path, include_directories, line_number)) {
                    return false;
                }
                continue;
            }
        }

        out_stream << line << endl;
    }
    return true;
}

bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories) {
    ifstream in_stream(in_file);
    ofstream out_stream(out_file);

    if (!in_stream.is_open() || !out_stream.is_open()) {
        return false;
    }
    return ProcessInclude(in_stream, out_stream, in_file, include_directories, 0);
}

void Test() {
    error_code err;
    filesystem::remove_all("sources"_p, err);
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err);
    filesystem::create_directories("sources"_p / "include1"_p, err);
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err);

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
                "#include \"dir1/b.h\"\n"
                "// text between b.h and c.h\n"
                "#include \"dir1/d.h\"\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"
                "#   include<dummy.txt>\n"
                "}\n"s;
    }
    {
        ofstream file("sources/dir1/b.h");
        file << "// text from b.h before include\n"
                "#include \"subdir/c.h\"\n"
                "// text from b.h after include"s;
    }
    {
        ofstream file("sources/dir1/subdir/c.h");
        file << "// text from c.h before include\n"
                "#include <std1.h>\n"
                "// text from c.h after include\n"s;
    }
    {
        ofstream file("sources/dir1/d.h");
        file << "// text from d.h before include\n"
                "#include \"lib/std2.h\"\n"
                "// text from d.h after include\n"s;
    }
    {
        ofstream file("sources/include1/std1.h");
        file << "// std1\n"s;
    }
    {
        ofstream file("sources/include2/lib/std2.h");
        file << "// std2\n"s;
    }

    assert((!Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p,
                                  {"sources"_p / "include1"_p,"sources"_p / "include2"_p})));

    ostringstream test_out;
    test_out << "// this comment before include\n"
                "// text from b.h before include\n"
                "// text from c.h before include\n"
                "// std1\n"
                "// text from c.h after include\n"
                "// text from b.h after include\n"
                "// text between b.h and c.h\n"
                "// text from d.h before include\n"
                "// std2\n"
                "// text from d.h after include\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"s;

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}

int main() {
    Test();
}