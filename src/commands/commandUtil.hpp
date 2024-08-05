#pragma once

#include <algorithm>
#include <charconv>
#include <map>
#include <string>
#include <vector>

template <class T>
inline std::string generateUniqueName(const std::string& originStr, const std::map<std::string, T>& compareMap)
{
    std::vector<std::string> compareList(compareMap.size());
    std::transform(compareMap.begin(), compareMap.end(), compareList.begin(),
                   [](const auto& itme) { return itme.second->getName(); });

    const bool isOriginStrFound = std::find(compareList.begin(), compareList.end(), originStr) != compareList.end();

    if (!isOriginStrFound)
    {
        return originStr;
    }

    int maxCompNumSuffix = 0;
    for (const std::string& compareStr : compareList)
    {
        if (compareStr.size() > originStr.size())
        {
            const std::string_view compareStrView(compareStr);

            if (compareStrView.substr(0, originStr.size()) == originStr)
            {
                const auto partCompareStrView =
                    compareStrView.substr(originStr.size(), compareStrView.size() - originStr.size());

                if (partCompareStrView.substr(0, 1) == "_")
                {
                    int compNumSuffix;
                    auto start = partCompareStrView.data() + 1;
                    auto last = partCompareStrView.data() + partCompareStrView.size();
                    const auto res = std::from_chars(start, last, compNumSuffix);

                    if (res.ec == std::errc() || res.ptr == last)
                    {
                        maxCompNumSuffix = std::max(maxCompNumSuffix, compNumSuffix);
                    }
                }
            }
        }
    }

    if (maxCompNumSuffix == 0)
    {
        return originStr + "_" + std::to_string(1);
    }
    else if (maxCompNumSuffix > 0)
    {
        return originStr + "_" + std::to_string(maxCompNumSuffix + 1);
    }

    return originStr;
}
