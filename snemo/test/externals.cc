#include <iostream>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#ifdef IMP_TEST_BAYEUX
#include <bayeux/datatools/properties.h>

void
run_test()
{
  datatools::properties x{};
  x.store("hello", std::vector<int>{1, 2, 3, 4});
}
#endif

#ifdef IMP_TEST_GITCONDDB
#include <GitCondDB.h>

void
run_test()
{
  auto db = GitCondDB::connect(TOSTRING(IMP_TEST_REPO));
  if (!db.connected()) {
    throw std::runtime_error{"No db connection"};
  }

  auto row = db.get({"HEAD", "smoketest/externals.cc", 0});
  std::cout << std::get<0>(row) << std::endl;

  db.disconnect();
}
#endif

#ifdef IMP_TEST_NLOHMANN
#include <nlohmann/json.hpp>

void
run_test()
{
  using json = nlohmann::json;
  json j;
  j["pi"] = 3.141;
  j["happy"] = true;
  j["nothing"] = nullptr;
  std::cout << j.dump(2) << std::endl;
}
#endif

int
main(int, char**)
{
  run_test();
  return 0;
}
