#ifndef CPP_UTILS_STRING_H
#define CPP_UTILS_STRING_H
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
namespace lina { namespace cpp_utils { namespace string{
    inline std::vector<std::string> split(const std::string& str, char delim)
    {
        std::vector<std::string> strings;
        auto iter = str.begin();
        auto iter2 = str.begin();
        while (iter != str.end())
        {
            iter2 = std::find(iter, str.end(), delim);
            strings.emplace_back(iter, iter2);
            if (iter2 == str.end())[[unlikely]]
            {
                break;
            }
            iter = iter2 + 1;
        }
        return strings;
    }
    inline std::vector<std::string> split(const std::string& str, std::string&& delim)
    {
        std::vector<std::string> strings;
        auto start = 0;
        auto curr = 0;
        while (curr < str.size())
        {
            curr = str.find(delim, start);
            if (curr == std::string::npos)
            {
                curr = str.size();
            }
            strings.emplace_back(str.begin() + start, str.begin() + curr);
            start = curr + delim.size();
        }
        return strings;
    }
    inline std::vector<std::string> split(const std::string& str, std::vector<std::string>&& delims)
    {
        std::vector<std::string> strings;
        auto start = 0;
        auto curr = 0;
        auto min_start = (int)str.size();
        auto min_idx = 0;
        while (curr < str.size())
        {
            min_start = str.size();
            min_idx = 0;
            for (int i = 0; i < delims.size(); i++)
            {
                const auto& delim = delims[i];
                curr = str.find(delim, start);
                if (curr == std::string::npos)
                {
                    curr = str.size();
                }
                min_start = std::min(curr, min_start);
            }
            curr = min_start;
            strings.emplace_back(str.begin() + start, str.begin() + curr);
            start = curr + delims[min_idx].size();
        }
        return strings;
    }
    inline std::vector<std::string> split(const std::string& str, std::vector<char>& delims)
    {
        std::vector<std::string> strings;
        auto iter = str.begin();
        auto iter2 = str.begin();
        auto min_iter = str.end();
        while (iter != str.end())
        {
            min_iter = str.end();
            for (auto delim : delims)
            {
                iter2 = std::find(iter, str.end(), delim);
                if (iter2 < min_iter)
                {
                    min_iter = iter2;
                }
            }
            strings.emplace_back(iter, min_iter);
            if (min_iter == str.end())[[unlikely]]
            {
                break;
            }
            iter = min_iter + 1;
        }
        return strings;
    }

    inline std::vector<std::string> split_all_space(const std::string& str)
    {
        std::vector<char> delim = {' ', '\t', '\n'};
        auto vec = split(str, delim);
        return vec;
    }
    inline std::pair<std::string::const_iterator , std::string::const_iterator> trim_iterators(const std::string& str, char delim)
    {
        auto iter = std::find_if(str.begin(), str.end(), [&](char c) {return c != delim;});
        auto iter2 = std::find_if(str.rbegin(), str.rend(), [&](char c) {return c != delim;}).base();
        return std::make_pair(iter, iter2);
    }

    template <typename It, typename Pred, typename Constructible = std::string>
        inline It lfind_if(It first, It last, int predicate_length, Pred p)
        {
            while (first != last)
            {
                It next = first;
                int cnt = 0;
                while (next != last && cnt < predicate_length)
                {
                    ++next;
                    ++cnt;
                }
                if constexpr (requires {p(first, next);})
                {
                    if (p(first, next))
                    {
                        return first;
                    }
                }
                else 
                {
                    if (p(Constructible(first, next)))
                    {
                        return first;
                    }
                }
                first = next;
            }
            return first;
        }
    template <typename It, typename Pred, typename Constructible = std::string>
        inline std::pair<It, It> lfind_if(It first, It last, Pred p)
        {
                It next = first;
                ++next;
                if constexpr (requires {p(first, next);})
                {
                    while (first != last && !p(first, next))
                    {
                        ++first;
                        if (next != last)
                        {
                            ++next;
                        }
                    }
                    while (next != last && p(first, next))
                    {
                        ++next;
                    }
                    return {first, next};
                }
                else 
                {
                    while (first != last && !p(Constructible(first, next)))
                    {
                        ++first;
                        ++next;
                    }
                    while (next != last && p(Constructible(first, next)))
                    {
                        ++next;
                    }
                    return {first, next};
                }
                first = next;
        }
    inline std::pair<std::string::const_iterator , std::string::const_iterator> trim_iterators(const std::string& str, std::string&& delim)
    {
        auto iter = lfind_if(str.begin(), str.end(), delim.size(), [&](std::string str)
                {
                return str != delim;
                });
        std::string rev_delim(delim.rbegin(), delim.rend());
        auto iter2 = lfind_if(str.rbegin(), str.rend(), delim.size(), [&](std::string str)
                {
                return str != rev_delim;
                });

        return std::make_pair(iter, iter2.base());
    }

    inline std::pair<std::string::const_iterator , std::string::const_iterator> trim_iterators(const std::string& str, std::vector<char>&& delims)
    {
        auto iter = std::find_if(str.begin(), str.end(), [&](char c) {
                bool found = true;
                for (auto delim : delims)
                {
                found &= (c != delim);
                }
                return found;
                });
        auto iter2 = std::find_if(str.rbegin(), str.rend(), [&](char c) {
                bool found = true;
                for (auto delim : delims)
                {
                found &= (c != delim);
                }
                return found;
                }).base();
        return std::make_pair(iter, iter2);
    }
    
    inline std::string trim(const std::string& str, char delim)
    {
        auto p = trim_iterators(str, delim);
        return std::string(p.first, p.second);
    }
    inline std::string trim(const std::string& str, std::string&& delim)
    {
        auto p = trim_iterators(str, std::move(delim));
        return std::string(p.first, p.second);
    }
    inline std::string trim(const std::string& str, std::vector<char>&& delim)
    {
        auto p = trim_iterators(str, std::move(delim));
        return std::string(p.first, p.second);
    }
    inline std::string trim_all_space(const std::string& str)
    {
        std::vector<char> delim = {' ', '\t', '\n'};
        auto tstr = trim(str, std::move(delim));
        return tstr;
    }
    inline std::string load_file_to_string(std::string&& path)
    {
        std::ifstream file(path, std::ios::binary);
        std::ostringstream content_stream;
        content_stream << file.rdbuf();
        std::string content = content_stream.str();
        file.close();
        return content;
    }
    inline void write_string_to_file(std::string&& content, std::string&& path)
    {
        std::ofstream file(path);
        std::istringstream content_stream;
        file << content;
        file.close();
    }
    inline void replace_all_inplace(std::string&& from, std::string&& to, std::string& input_string)
    {
        auto start = 0;
        while ((start = input_string.find(from, start)) != std::string::npos)
        {
            input_string.replace(start, from.size(), to);
            start += to.length();
        }
    }
    inline int count_occurences(const std::string& input_string, std::string&& substring)
    {
        int count = 0;
        auto start = 0;
        while ((start = input_string.find(substring, start)) != std::string::npos)
        {
            start += substring.length();
            count++;
        }
        return count;
    }
    inline std::string replace_all(std::string&& from, std::string&& to, std::string input_string)
    {
        auto start = 0;
        while ((start = input_string.find(from, start)) != std::string::npos)
        {
            input_string.replace(start, from.size(), to);
            start += to.length();
        }
        return input_string;
    }
    template <typename Pred, typename T>
    inline std::string replace_and_apply(std::string&& from, std::string&& to, std::string input_string, T& val, Pred pred)
    {
        auto start = 0;
        while ((start = input_string.find(from, start)) != std::string::npos)
        {
            input_string.replace(start, from.size(), to);
            start += to.length();
            pred(val);
        }
        return input_string;
    }
    template <typename ReplaceFunction, typename MatchPredicate, typename T>
    inline std::string replace_and_apply(std::string input_string, T& val, 
            MatchPredicate mp,
            ReplaceFunction rf
            )
    {

        auto start_pos = 0;
        auto end_pos = 0;
        std::string replace_val;
        while (start_pos < input_string.size())
        {
            auto pp = lfind_if(input_string.begin() + start_pos, input_string.end(), mp);
            if (pp.first == input_string.end()) break;
            start_pos = std::distance(input_string.begin(), pp.first);
            replace_val = rf(val);
            end_pos = std::distance(pp.first, pp.second);

            input_string.replace(
                    start_pos,
                    end_pos,
                    replace_val
                    );
            start_pos += replace_val.size();
        }
        return input_string;
    }
    template <typename ReplaceFunction, typename MatchPredicate>
    inline std::string capture_and_replace(std::string input_string, 
            MatchPredicate mp,
            ReplaceFunction rf
            )
    {
        auto start_pos = 0;
        auto end_pos = 0;
        std::string replace_val;
        while (start_pos < input_string.size())
        {
            auto pp = lfind_if(input_string.begin() + start_pos, input_string.end(), mp);
            if (pp.first == input_string.end()) break;
            start_pos = std::distance(input_string.begin(), pp.first);
            replace_val = rf(pp.first, pp.second);

            end_pos = std::distance(pp.first, pp.second);

            input_string.replace(
                    start_pos,
                    end_pos,
                    replace_val
                    );

            start_pos += replace_val.size();
        }
        return input_string;
    }
    inline bool begins_with(const std::string& str, std::string&& str2)
    {
        if (str2.size() == 0) return true;
        auto stv = std::string_view(&(*str.begin()), str2.size());
        return stv == str2;
    }
    inline std::string bool_to_string(bool b, std::string&& true_string = "true", std::string&& false_string = "false")
    {
        if (b)
        {
            return true_string;
        }
        else 
        {
            return false_string;
        }
    }
    inline std::string to_lower_alpha(const std::string& str)
    {
        std::string str2;
        str2.reserve(str.size());
        for (auto c : str)
        {
            if (c <= 'Z' && c >= 'A')
            {
                str2.push_back((c - 'A') + 'a');
                continue;
            }
            str2.push_back(c);
        }
        return str2;
    }
}}}
#endif
