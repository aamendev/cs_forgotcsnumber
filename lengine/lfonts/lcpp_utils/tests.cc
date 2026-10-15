#include "tests.h"
#include "lstring.h"
#include <cassert>
#include <string>
namespace lina { namespace cpp_utils { namespace tests{
    void string()
    {
        BEGIN_TEST("string");
        std::string split_test = "one piece is real";
        std::string multi_split_test = "one\tpiece is\nreal";

        std::string trim_test = "     one piece is   real   ";
        std::string multi_trim_test = "\n\n\n\n     \t \t  one piece is real\t\n    \t\n";

        auto splitted1 = string::split_all_space(split_test);
        assert(splitted1.size() == 4);
        assert(splitted1[0] == "one");
        assert(splitted1[1] == "piece");
        assert(splitted1[2] == "is");
        assert(splitted1[3] == "real");

        splitted1 = string::split_all_space(multi_split_test);
        assert(splitted1.size() == 4);
        assert(splitted1[0] == "one");
        assert(splitted1[1] == "piece");
        assert(splitted1[2] == "is");
        assert(splitted1[3] == "real");

        auto str = string::trim(trim_test, ' ');
        assert(str == "one piece is   real");
        str = string::trim_all_space(multi_trim_test);
        assert(str == "one piece is real");

        str = "one piece piece is real, a very real piece, try it out piece";
        string::replace_all_inplace("piece", "love", str);
        assert(str == "one love love is real, a very real love, try it out love");
        string::replace_all_inplace("love", "piece", str);
        assert(
        str == "one piece piece is real, a very real piece, try it out piece");

        str = "one piece piece is real, a very real piece, try it out piece";
        auto rep = string::replace_all("piece", "love", str);
        assert(rep == "one love love is real, a very real love, try it out love");
        assert(str == "one piece piece is real, a very real piece, try it out piece");

        auto str1 = "one||piece||is||real";
        auto str2 = "one||piece||is||real||";
        auto str3 = "||one||piece||is||real||";
        auto str4 = "||one||piece||||is||real||";
        auto str5 = "||||one||piece||||is||real|||||";
        splitted1 = string::split(str1, "||");
        auto splitted2 = string::split(str2, "||");
        auto splitted3 = string::split(str3, "||");
        auto splitted4 = string::split(str4, "||");

        assert(splitted1.size() == 4);
        assert(splitted2.size() == 5);
        assert(splitted3.size() == 6);
        assert(splitted4.size() == 7);

        assert(splitted1[0] == "one");
        assert(splitted1[1] == "piece");
        assert(splitted1[2] == "is");
        assert(splitted1[3] == "real");

        assert(splitted2[0] == "one");
        assert(splitted2[1] == "piece");
        assert(splitted2[2] == "is");
        assert(splitted2[3] == "real");
        assert(splitted2[4] == "");

        assert(splitted3[0] == "");
        assert(splitted3[1] == "one");
        assert(splitted3[2] == "piece");
        assert(splitted3[3] == "is");
        assert(splitted3[4] == "real");
        assert(splitted3[5] == "");

        assert(splitted4[0] == "");
        assert(splitted4[1] == "one");
        assert(splitted4[2] == "piece");
        assert(splitted4[3] == "");
        assert(splitted4[4] == "is");
        assert(splitted4[5] == "real");
        assert(splitted4[6] == "");

        auto trimmed1 = string::trim(str1, "||");
        auto trimmed2 = string::trim(str2, "||");
        auto trimmed3 = string::trim(str3, "||");
        auto trimmed4 = string::trim(str4, "||");
        auto trimmed5 = string::trim(str5, "||");
        assert(trimmed1 == str1);
        assert(trimmed2 == str1);
        assert(trimmed3 == str1);
        assert(trimmed4 == "one||piece||||is||real");
        assert(trimmed5 == "one||piece||||is||real|");

        str1 = "one||piece::is||real";
        str2 = "one||piece::is||real::";
        str3 = "||one||piece::||is||real||";

        std::vector<std::string> vec = {"||", "::"};
        splitted1 = string::split(str1, std::move(vec));
        splitted2 = string::split(str2, std::move(vec));
        splitted3 = string::split(str4, std::move(vec));

        assert(splitted1.size() == 4);
        assert(splitted2.size() == 5);
        assert(splitted3.size() == 7);

        assert(splitted1[0] == "one");
        assert(splitted1[1] == "piece");
        assert(splitted1[2] == "is");
        assert(splitted1[3] == "real");

        assert(splitted2[0] == "one");
        assert(splitted2[1] == "piece");
        assert(splitted2[2] == "is");
        assert(splitted2[3] == "real");
        assert(splitted2[4] == "");

        assert(splitted3[0] == "");
        assert(splitted3[1] == "one");
        assert(splitted3[2] == "piece");
        assert(splitted3[3] == "");
        assert(splitted3[4] == "is");
        assert(splitted3[5] == "real");
        assert(splitted3[6] == "");


        str = std::string("one piece is real");
        for (int i = 0; i < str.size(); i++)
        {
            assert(string::begins_with(str, std::string(str.begin(), str.begin() + i)) == true);
        }

        str = std::string("one piece is real");
        std::string from = " ";
        std::string to = "1";

        auto nstr = string::replace_and_apply(
                std::move(from), 
                std::move(to), 
                str, 
                to, 
                [](std::string& local_to){ local_to = std::to_string(1 + std::stoi(local_to));}
                );
        assert(nstr == "one1piece2is3real");

        to = "1";

        struct test_struct 
        {
            test_struct(std::string& s1): ref(s1){}
            std::string& ref;
            int val = 1;
        };

        test_struct ts(to);

        nstr.clear();
        nstr = string::replace_and_apply(
                std::move(from), 
                std::move(to), 
                str, 
                ts, 
                [](test_struct& local_to){ 
                local_to.val+=1; 
                local_to.ref = std::to_string(local_to.val);}
                );
        assert(nstr == "one1piece2is3real");


        str = "one||piece||is||real";
        int x = 0;
        rep = string::replace_and_apply(str, x, [](std::string str){
            return
            (str.size() == 1 && str[0] == '|') || (str.size() > 1 && str[0] == '|' && str[str.size() - 1] != '|');
                },
                [](int& v){return " ";});

        assert(rep == "one piece is real");

        str = "arr[0] + arr[1  ] + arr[  2  ]";
        x = 5;

        rep = string::capture_and_replace(
                str, 
                [](std::string str)
                {
            return
            (str.size() == 1 && str[0] == '[') || (str.size() > 1 && str[0] == '[' && str[str.size() - 1] != ']');
                },
                [x](std::string::const_iterator begin, std::string::const_iterator end)mutable
                {
                    std::string new_str(begin, end);
                    auto last = new_str.back();
                    new_str.pop_back();
                    new_str.push_back('+');
                    new_str += std::to_string(x);
                    new_str.push_back(last);
                    return new_str;
                });
        assert(rep == "arr[0+5] + arr[1  +5] + arr[  2  +5]");
        END_TEST("string");
    }
    void all()
    {
        string();
    }
}}}
