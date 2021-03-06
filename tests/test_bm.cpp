#include "bm/bm.h"
#include <string>
#include <iostream>

int main()
{
    std::string doc = "level   info   time   2021-03-30T08 22 52 484Z   caller    root gopath src github com influxdata influxdb query executor go 530   msg   Executing query   service   query   query   SELECT last SL_1_Tag7  AS SL_1_Tag7 FROM SZSSLKJYXGS_1OU7 autogen   68885b4c32ff4708a8a649c9b4baae59   WHERE gatewayId =  2c938083783650af01786535768406ac  AND time >=  2021-03-29T08 22 52Z  AND time <=  2021-03-30T08 22 52Z  fill none    batch  1";
    std::string token = "=";
    std::cout << doc << std::endl;
    std::cout << token << std::endl;
    auto result = bm::BoyerMoore(doc, token);
    for (auto offset : result)
        std::cout << offset << ',' << std::endl;
    std::cout << "ok" << std::endl;
    doc = "the  fox is great";
    token = "the fox";
    std::cout << doc << std::endl;
    std::cout << token << std::endl;
    result = bm::BoyerMoore(doc, token);
    for (auto offset : result)
        std::cout << offset << ',' << std::endl;
    std::cout << "ok" << std::endl;
    return 0;
}